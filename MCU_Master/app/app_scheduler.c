/**
 ************************************************************
 * @file    app_scheduler.c
 * @brief   主机状态机
 *
 * 状态流转:
 *   IDLE → FACE_WAIT (K230人脸)
 *   FACE_WAIT → FACE_DONE (识别成功)
 *   FACE_WAIT → IDLE (超时/陌生人)
 *   FACE_DONE → COURIER_WAIT (快递员) / IDLE (普通用户)
 *   COURIER_WAIT → CABINET_OPEN (语音开柜) / IDLE (超时)
 *   IDLE → QR_SCANNED (二维码扫描)
 *   QR_SCANNED → CABINET_OPEN
 *   CABINET_OPEN → IDLE (关门)
 ************************************************************
 */
#include "app_scheduler.h"
#include "bsp_system.h"
#include "bsp_relay.h"
#include "bsp_light_sensor.h"
#include "bsp_ultrasonic.h"
#include "ws2812.h"
#include "bsp_uart_k230.h"
#include "bsp_uart_scanner.h"
#include "bsp_uart_wifi.h"
#include "bsp_uart_slave.h"
#include "cloud_esp8266.h"
#include "cloud_proto.h"
#include "medicine_db.h"
#include "usart.h"
#include <string.h>

typedef enum {
    S_IDLE = 0,
    S_FACE_DONE,
    S_COURIER_WAIT,
    S_QR_SCANNED,
    S_CABINET_OPEN,
} State_t;

static State_t		state = S_IDLE;
static uint32_t		state_tick = 0;
static UserType_t	current_user = USER_TYPE_UNKNOWN;
static uint8_t		current_cabinet = 0;
static uint8_t		open_method = 0;
static uint8_t		wifi_needed = 0;     // 是否有数据等待上传
static uint8_t		wifi_busy = 0;       // WiFi 是否正在传输
static uint32_t		wifi_timer = 0;
volatile uint8_t uart2_mode = UART2_MODE_SCANNER;  // 默认扫码模式

static void enter_state(State_t new_state)
{
    state = new_state;
    state_tick = HAL_GetTick();
}

static void process_env(void)
{
    static uint32_t last_check = 0;
    uint32_t now = HAL_GetTick();
    if ((now - last_check) < 200) return;
    last_check = now;

    uint8_t dark = BSP_LightSensor_IsDark();
    uint8_t near = BSP_Ultrasonic_IsPersonNear();

    static uint8_t light_state = 0;
    static uint8_t on_cnt  = 0;
    static uint8_t off_cnt = 0;

    if (dark && near)
    {
        if (on_cnt < ON_CONFIRM)  on_cnt++;
        off_cnt = 0;
    }
    else
    {
        on_cnt = 0;
        if (off_cnt < OFF_CONFIRM) off_cnt++;
    }

    if (on_cnt >= ON_CONFIRM && !light_state)
    {
        WS2812_AllOn();
        light_state = 1;
    }
    else if (off_cnt >= OFF_CONFIRM && light_state)
    {
        WS2812_AllOff();
        light_state = 0;
    }
}

static void process_k230(void)
{
    if (!k230_rx_flag) return;

    if (k230_rx_len >= 3 && k230_rx_buf[0] == 0xFF)
    {
        uint8_t utype = k230_rx_buf[1];
        if (utype == 1 || utype == 2)
        {
            current_user = (utype == 2) ? USER_TYPE_COURIER : USER_TYPE_NORMAL;
            enter_state(S_FACE_DONE);
        }
    }
    else if (k230_rx_len >= 2 && k230_rx_buf[0] >= '1' && k230_rx_buf[0] <= '9')
    {
        uint8_t cab = k230_rx_buf[0] - '0';
        if (cab >= 1 && cab <= CABINET_COUNT)
        {
            current_cabinet = cab;
            open_method = 1;
            enter_state(S_QR_SCANNED);
        }
    }

    BSP_UART_K230_ClearRx();
}

static void process_scanner(void)
{
    if (!scanner_rx_flag) return;

    // 条码是纯数字字符串，找对应药品索引
    uint8_t idx = Medicine_FindByCode((const char*)scanner_rx_buf);
    if (idx < MEDICINE_COUNT)
    {
        // 发给从机播报
        BSP_UART_Slave_SendFrame(0x04, idx);
    }

    BSP_UART_Scanner_ClearRx();
}

/* 切换到扫码模式 */
void Scanner_Mode_Enter(void)
{
    if (uart2_mode == UART2_MODE_SCANNER) return;
    HAL_UART_AbortReceive_IT(&huart2);
    huart2.Init.BaudRate = 9600;
    HAL_UART_Init(&huart2);
    uart2_mode = UART2_MODE_SCANNER;
    HAL_UART_Receive_IT(&huart2, &scanner_rx_byte, 1);
}

/* 切换到 WiFi 模式 */
void WiFi_Mode_Enter(void)
{
    if (uart2_mode == UART2_MODE_WIFI) return;
    HAL_UART_AbortReceive_IT(&huart2);
    huart2.Init.BaudRate = 115200;
    HAL_UART_Init(&huart2);
    uart2_mode = UART2_MODE_WIFI;
    HAL_UART_Receive_IT(&huart2, &wifi_rx_byte, 1);
}

void WiFi_TriggerUpload(void)
{
    wifi_needed = 1;
}

static void process_slave_rx(void)
{
    if (!slave_rx_flag) return;

    if (slave_rx_len >= 4 && slave_rx_buf[3] == 0x0D)
    {
        uint8_t cmd  = slave_rx_buf[0];
        uint8_t data = slave_rx_buf[1];

        if ((cmd ^ data) == slave_rx_buf[2])
        {
            switch (cmd)
            {
                case 0x14:
                    if (data == 1 || data == 2)
                    {
                        current_cabinet = data;
                        open_method = 2;
                        BSP_Relay_Open((DoorId_t)current_cabinet);
                        BSP_UART_Slave_SendFrame(0x02, current_cabinet);
                        WiFi_TriggerUpload();
                        CloudProto_ReportOpen(current_cabinet, open_method);
                        enter_state(S_CABINET_OPEN);
                    }
                    else if (data == 3)
                    {
                        BSP_UART_Slave_SendFrame(0x01, 0x00);
                    }
                    break;
                case 0x13:
                    break;
                case 0x11:
                    CloudProto_ReportHeartRate(data);
                    WiFi_TriggerUpload();
                    BSP_UART_Slave_SendFrame(0x05, data);
                    break;
                case 0x12:
                    CloudProto_ReportSpO2(data);
                    WiFi_TriggerUpload();
                    BSP_UART_Slave_SendFrame(0x06, data);
                    break;
								case 0x04:
										BSP_UART_Slave_SendFrame(0x04, data);
										break;
                default: break;
            }
        }
    }

    BSP_UART_Slave_ClearRx();
}

void App_Scheduler_Init(void)
{
    BSP_Relay_Init();
    BSP_LightSensor_Init();
    WS2812_Init();
    BSP_UART_K230_Init();
    BSP_UART_Slave_Init();
    BSP_UART2_Init();
    state = S_IDLE;

    /* 先切 WiFi 模式完成 ESP8266 初始化，再切回扫码 */
    WiFi_Mode_Enter();
    ESP8266_Init();
    ESP8266_ConnectMQTT();
    Scanner_Mode_Enter();       /* 默认回到扫码模式 */
}

void App_Scheduler_Run(void)
{
    BSP_Relay_Process();
    BSP_Ultrasonic_Process();
	
		process_scanner();

    /* === WiFi 上传调度 === */
    if (wifi_needed && !wifi_busy)
    {
        WiFi_Mode_Enter();           // 切到 WiFi
        wifi_busy = 1;
        wifi_timer = HAL_GetTick();
    }

    if (wifi_busy)
    {
        CloudProto_Flush();     /* 每次只发一条，避免阻塞太久 */

        /* 全部发完 或 超时放弃 → 切回扫码 */
        if (!CloudProto_HasPending() || (HAL_GetTick() - wifi_timer > 5000))
        {
            Scanner_Mode_Enter();
            wifi_busy = 0;
            wifi_needed = 0;
        }
    }
    process_env();
    process_k230();
    process_slave_rx();

    switch (state)
    {
        case S_FACE_DONE:
            BSP_UART_Slave_SendFrame(0x03, (uint8_t)current_user);
            WiFi_TriggerUpload();
            CloudProto_ReportUser((uint8_t)current_user);
            if (current_user == USER_TYPE_COURIER)
                enter_state(S_COURIER_WAIT);
            else
                enter_state(S_IDLE);
            break;

        case S_COURIER_WAIT:
            if ((HAL_GetTick() - state_tick) > COURIER_CMD_TIMEOUT_MS)
                enter_state(S_IDLE);
            break;

        case S_QR_SCANNED:
            BSP_Relay_Open((DoorId_t)current_cabinet);
            BSP_UART_Slave_SendFrame(0x02, current_cabinet);
						WiFi_TriggerUpload(); 
						CloudProto_ReportOpen(current_cabinet, open_method);
            enter_state(S_CABINET_OPEN);
            break;

        case S_CABINET_OPEN:
            if (BSP_Relay_IsLocked((DoorId_t)current_cabinet))
            {
								WiFi_TriggerUpload();
                CloudProto_ReportTaken(current_cabinet);
                current_cabinet = 0;
                current_user = USER_TYPE_UNKNOWN;
                enter_state(S_IDLE);
            }
            break;

        default: break;
    }
}
