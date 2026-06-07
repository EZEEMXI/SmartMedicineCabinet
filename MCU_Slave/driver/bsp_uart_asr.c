/**
 ************************************************************
 * @file    bsp_uart_asr.c
 * @brief   天问51 UART3 驱动
 ************************************************************
 */
#include "bsp_uart_asr.h"
#include "usart.h"
#include <string.h>

uint8_t asr_rx_buf[ASR_RX_BUF_SIZE];
uint8_t asr_rx_flag = 0;
uint8_t asr_rx_len  = 0;
uint8_t asr_rx_byte;
uint8_t asr_rx_idx = 0;

void BSP_ASR_Init(void)
{
    HAL_UART_Receive_IT(&huart3, &asr_rx_byte, 1);
}

void BSP_ASR_SendByte(uint8_t d)
{
    HAL_UART_Transmit(&huart3, &d, 1, 100);
}

void BSP_ASR_SendCmd(uint8_t* data, uint8_t len)
{
    BSP_ASR_SendByte(0xAA);
    BSP_ASR_SendByte(0x55);
    for (uint8_t i = 0; i < len; i++) BSP_ASR_SendByte(data[i]);
    BSP_ASR_SendByte(0x55);
    BSP_ASR_SendByte(0xAA);
}

void BSP_ASR_ClearRx(void)
{
    asr_rx_flag = 0;
    asr_rx_len  = 0;
    asr_rx_idx  = 0;
    memset((void*)asr_rx_buf, 0, ASR_RX_BUF_SIZE);
}
