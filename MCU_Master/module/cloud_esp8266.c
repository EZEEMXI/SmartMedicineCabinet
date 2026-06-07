/**
 ************************************************************
 * @file    cloud_esp8266.c
 * @brief   ESP8266 通过 MQTT AT 命令连接 OneNet 平台
 ************************************************************
 */
#include "cloud_esp8266.h"
#include "secrets.h"          /* WiFi / OneNet 凭证（不提交到仓库） */
#include "bsp_uart_wifi.h"
#include "bsp_system.h"
#include <string.h>
#include <stdio.h>

/* ===== OneNet MQTT 服务器 ===== */
#define ONENET_IP    "183.230.40.39"
#define ONENET_PORT  1883
#define MQTT_LINK_ID 0

/* ===== Topic 定义（OneNet 规范格式）===== */
/* 数据上报 Topic: $sys/{pid}/{device}/dp/post/json */
#define TOPIC_DATA_UP   "$sys/" PRODUCT_ID "/" DEVICE_ID "/dp/post/json"
/* 命令下发 Topic（订阅以接收 App 发来的指令） */
#define TOPIC_CMD_DOWN  "$sys/" PRODUCT_ID "/" DEVICE_ID "/cmd/request/+"

/* ===== 连接状态 ===== */
static uint8_t mqtt_connected = 0;

/* ---- 内部工具函数 ---- */

/* 发送 AT 命令并等待预期回复 */
static uint8_t at_send_wait(const char* cmd, const char* expect, uint16_t timeout_ms)
{
    BSP_UART_WiFi_ClearRx();
    BSP_UART_WiFi_SendString((char*)cmd);

    uint32_t start = HAL_GetTick();
    uint16_t last_len = 0;

    while ((HAL_GetTick() - start) < timeout_ms)
    {
        uint16_t cur_len = wifi_rx_len;
        if (cur_len != last_len)
        {
            last_len = cur_len;
            if (cur_len > 0 && cur_len < WIFI_RX_BUF_SIZE)
                wifi_rx_buf[cur_len] = '\0';
            if (strstr((const char*)wifi_rx_buf, expect) != NULL)
                return 1;
        }
        HAL_Delay(10);
    }
    return 0;
}

/* ---- 公开接口 ---- */

/** 步骤1: 初始化 ESP8266 并连 WiFi */
void ESP8266_Init(void)
{
    /* 复位模块 */
    at_send_wait("AT+RST\r\n", "ready", 3000);
    HAL_Delay(500);

    /* 设为 Station 模式 */
    at_send_wait("AT+CWMODE=1\r\n", "OK", 1000);

    /* 连接 WiFi */
    char buf[128];
    sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
    at_send_wait(buf, "WIFI GOT IP", 15000);   /* WiFi 连接可能较慢 */

    /* 关闭回显，避免干扰 */
    at_send_wait("ATE0\r\n", "OK", 500);
}

/** 步骤2: 配置 MQTT 用户信息 */
static void mqtt_config_user(void)
{
    /* 
     * AT+MQTTUSERCFG=<LinkID>,<scheme>,<clientID>,<username>,<password>,
     *                  <cert_key_ID>,<CA_ID>,<path>
     * 
     * LinkID=0, scheme=1(不主动清理session), clientID=设备ID, 
     * username=产品ID, password=设备密钥
     */
    char buf[256];
    sprintf(buf, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",
            DEVICE_ID, PRODUCT_ID, AUTH_KEY);
    at_send_wait(buf, "OK", 2000);
}

/** 步骤3: 连接 OneNet MQTT Broker */
void ESP8266_ConnectMQTT(void)
{
    mqtt_config_user();

    /* AT+MQTTCONN=<LinkID>,<host>,<port>,<reconnect> */
    char buf[64];
    sprintf(buf, "AT+MQTTCONN=0,\"%s\",%d,1\r\n", ONENET_IP, ONENET_PORT);
    
    if (at_send_wait(buf, "+MQTTCONN:0,0", 8000))
    {
        mqtt_connected = 1;
        
        /* 连接成功后，订阅命令下发 Topic */
        /* 这样 App 发指令时，ESP8266 能收到 */
        char sub_buf[128];
        sprintf(sub_buf, "AT+MQTTSUB=0,\"%s\",1\r\n", TOPIC_CMD_DOWN);
        at_send_wait(sub_buf, "OK", 1000);
    }
    else
    {
        mqtt_connected = 0;
    }
}

/** 检查 MQTT 连接是否正常 */
uint8_t ESP8266_IsMQTTConnected(void)
{
    return mqtt_connected;
}

/** 发布 JSON 数据到 OneNet（数据上报） */
void ESP8266_PublishData(const char* json_str)
{
    if (!mqtt_connected)
    {
        /* 如果断线了，尝试重连 */
        ESP8266_ConnectMQTT();
        if (!mqtt_connected) return;
    }

    /* 
     * AT+MQTTPUB=<LinkID>,<topic>,<data>,<qos>,<retain>
     * QoS=1（至少送达一次）, retain=0（不保留） 
     */
    char buf[384];
    int len = sprintf(buf, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0\r\n",
                       TOPIC_DATA_UP, json_str);

    /* 注意：如果 json_str 太长（>256字节），AT命令可能失败。
     * 本项目的 JSON 报文都很短（<150字节），不会有问题 */
    if (len > 350)
    {
        /* JSON 太长了，需要分包或缩短内容 */
        return;
    }

    at_send_wait(buf, "OK", 2000);
}

/** 主循环中调用，处理 MQTT 保活和接收 */
void ESP8266_Process(void)
{
    /* 
     * MQTT 协议要求定期发送 PINGREQ 保活。
     * ESP8266 AT 固件会自动处理保活，这里不需要额外操作。
     * 
     * 如果要处理来自 App 的命令（通过 TOPIC_CMD_DOWN 收到的消息），
     * 需要在这里检查 wifi_rx_buf 中是否有 "+MQTTSUBRECV:" 开头的消息。
     * 这部分在第三步实现。
     */
}

/** 断开 MQTT 连接 */
void ESP8266_DisconnectMQTT(void)
{
    at_send_wait("AT+MQTTCLEAN=0\r\n", "OK", 1000);
    mqtt_connected = 0;
}
