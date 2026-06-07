/**
 ************************************************************
 * @file    slave_scheduler.c
 * @brief   从机调度
 ************************************************************
 */
#include "slave_scheduler.h"
#include "bsp_system.h"
#include "bsp_uart_host.h"
#include "dev_health.h"
#include "dev_tts.h"
#include "dev_asr.h"
#include "medicine_db.h"

void Slave_Scheduler_Init(void)
{
    BSP_HostUART_Init();
    Dev_TTS_Init();
    Dev_Health_Init();
    Dev_ASR_Init();

    uart1_print("[SLAVE] Init done\r\n");
}

void Slave_Scheduler_Run(void)
{
    /* 常驻 */
    Dev_Health_Process();
    Dev_TTS_Process();
    Dev_ASR_Process();

    /* 语音指令来了就上报主机 */
    ASR_Cmd_t ac = Dev_ASR_GetCommand();
    if (ac != ASR_CMD_NONE)
    {
        uint8_t d = 0;
        if (ac == ASR_CMD_HEART_RATE) d = 3;
        else if (ac == ASR_CMD_OPEN_1) d = 1;
        else if (ac == ASR_CMD_OPEN_2) d = 2;

        if (d > 0)
        {
            BSP_HostUART_SendFrame(0x14, d);
							uart1_print("[SLAVE] ASR cmd sent: 0x14 ");
							uart1_print((d == 1) ? "1" : (d == 2) ? "2" : "3");
							uart1_print("\r\n");
        }
        Dev_ASR_Clear();
    }


    /* 心率检测完成 */
//    if (Dev_Health_IsDone())
//    {
//        HealthResult_t r = Dev_Health_GetResult();

//        if (r.hr_valid)
//        {
//            BSP_HostUART_SendFrame(0x11, r.heart_rate);
//            BSP_DelayMs(100);
//        }
//        BSP_HostUART_SendFrame(0x13, r.hr_valid ? 1 : 0);

//        Dev_Health_Clear();
//        if (DEBUG_ENABLE)
//        {
//            uart1_print("[SLAVE] Health done: HR=");
//            char buf[4];
//            buf[0] = '0' + r.heart_rate / 100;
//            buf[1] = '0' + (r.heart_rate / 10) % 10;
//            buf[2] = '0' + r.heart_rate % 10;
//            buf[3] = '\0';
//            if (buf[0] != '0') uart1_print(&buf[0]);
//            else if (buf[1] != '0') uart1_print(&buf[1]);
//            else uart1_print(&buf[2]);
//            uart1_print(" SpO2=");
//            if (r.spo2 >= 100) uart1_print("100");
//            else
//            {
//                char s[3];
//                s[0] = '0' + r.spo2 / 10;
//                s[1] = '0' + r.spo2 % 10;
//                s[2] = '\0';
//                uart1_print(s);
//            }
//            uart1_print("\r\n");
//        }
//    }
		if (Dev_Health_IsDone())
		{
				HealthResult_t r = Dev_Health_GetResult();

				BSP_HostUART_SendFrame(0x11, r.heart_rate);
				HAL_Delay(100);
//				BSP_HostUART_SendFrame(0x13, r.hr_valid ? 1 : 0);

				Dev_Health_Clear();

				uart1_print("[SLAVE] Health done: HR=");
				char buf[4];
				buf[0] = '0' + r.heart_rate / 100;
				buf[1] = '0' + (r.heart_rate / 10) % 10;
				buf[2] = '0' + r.heart_rate % 10;
				buf[3] = '\0';
				if (buf[0] != '0') uart1_print(&buf[0]);
				else if (buf[1] != '0') uart1_print(&buf[1]);
				else uart1_print(&buf[2]);
				uart1_print(" SpO2=");
				if (r.spo2 >= 100) uart1_print("100");
				else
				{
						char s[3];
						s[0] = '0' + r.spo2 / 10;
						s[1] = '0' + r.spo2 % 10;
						s[2] = '\0';
						uart1_print(s);
				}
				uart1_print("\r\n");
		}

    /* 主机指令 */
		if (host_rx_flag)
		{
				if (host_rx_len >= 4 && host_rx_buf[3] == 0x0D)
				{
						uint8_t cmd  = host_rx_buf[0];
						uint8_t data = host_rx_buf[1];
						uint8_t xorr = cmd ^ data;

						if (xorr == host_rx_buf[2])
						{
								uart1_print("[SLAVE] CMD=");
								if (cmd == 0x01) uart1_print("01");
								else if (cmd == 0x03) uart1_print("03");
								else if (cmd == 0x02) uart1_print("02");
								else if (cmd == 0x05) uart1_print("05");
								else uart1_print("??");
								uart1_print("\r\n");

								switch (cmd)
								{
										case 0x01: Dev_Health_Start(); break;
										case 0x02: Dev_TTS_ClearDone(); Dev_TTS_SpeakOpenDoor(data); break;
										case 0x03: Dev_TTS_ClearDone(); Dev_TTS_SpeakWelcome(data); break;
										case 0x04: Dev_TTS_ClearDone(); Dev_TTS_SpeakMedicine(data); break;
										case 0x05: Dev_TTS_ClearDone(); Dev_TTS_SpeakHeartRate(data); break;
										default: break;
								}
						}
				}
				BSP_HostUART_ClearRx();
		}
}
