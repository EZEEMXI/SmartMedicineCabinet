/**
 ************************************************************
 * @file    dev_asr.c
 * @brief   天问51语音识别
 ************************************************************
 */
#include "dev_asr.h"
#include "bsp_uart_asr.h"
#include "bsp_system.h"
#include <string.h>

static ASR_Cmd_t cmd = ASR_CMD_NONE;
static uint32_t t_start = 0;

void Dev_ASR_Init(void)
{
    BSP_ASR_Init();
}

void Dev_ASR_Start(void)
{
    BSP_ASR_ClearRx();
    cmd = ASR_CMD_NONE;
    t_start = HAL_GetTick();
    uint8_t start[] = {0x01, 0x00, 0x00};
    BSP_ASR_SendCmd(start, 3);
}

void Dev_ASR_Process(void)
{
    if (!asr_rx_flag) return;
    asr_rx_flag = 0;

    if (strstr((const char*)asr_rx_buf, "HEART_RATE") != NULL)
        cmd = ASR_CMD_HEART_RATE;
    else if (strstr((const char*)asr_rx_buf, "CABINET_1_ON") != NULL)
        cmd = ASR_CMD_OPEN_1;
    else if (strstr((const char*)asr_rx_buf, "CABINET_2_ON") != NULL)
        cmd = ASR_CMD_OPEN_2;

    BSP_ASR_ClearRx();
}

ASR_Cmd_t Dev_ASR_GetCommand(void) { return cmd; }
void Dev_ASR_Clear(void) { cmd = ASR_CMD_NONE; }
