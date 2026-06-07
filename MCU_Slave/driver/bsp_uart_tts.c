/**
 ************************************************************
 * @file    bsp_uart_tts.c
 * @brief   USART2驱动 - TTS模块专用
 ************************************************************
 */

#include "bsp_uart_tts.h"
#include "usart.h"
#include <string.h>

uint8_t tts_rx_buf[TTS_RX_BUF_SIZE];
uint8_t tts_rx_flag    = 0;
uint8_t tts_rx_len     = 0;
uint8_t tts_new_byte   = 0;
uint8_t tts_last_byte  = 0;
uint8_t tts_rx_byte;
uint8_t tts_rx_idx     = 0;

void BSP_TTS_Init(void)
{
    HAL_UART_Receive_IT(&huart2, &tts_rx_byte, 1);
}

void BSP_TTS_SendByte(uint8_t data)
{
    HAL_UART_Transmit(&huart2, &data, 1, 100);
}

void BSP_TTS_SendString(const char *str)
{
    while (*str) BSP_TTS_SendByte((uint8_t)*str++);
}

void BSP_TTS_SendLine(const char *str)
{
    BSP_TTS_SendString(str);
    BSP_TTS_SendByte('\n');
}

void BSP_TTS_ClearRx(void)
{
    tts_rx_flag  = 0;
    tts_rx_len   = 0;
    tts_new_byte = 0;
    tts_last_byte= 0;
    tts_rx_idx   = 0;
    memset((void*)tts_rx_buf, 0, TTS_RX_BUF_SIZE);
}

void BSP_TTS_SendHex(const uint8_t *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) BSP_TTS_SendByte(data[i]);
}

void BSP_TTS_SendHexLine(const uint8_t *data, uint8_t len)
{
    BSP_TTS_SendHex(data, len);
    BSP_TTS_SendByte('\n');
}
