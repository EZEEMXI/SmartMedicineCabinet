/**
 ************************************************************
 * @file    bsp_uart_tts.h
 * @brief   TTS UART2 驱动
 ************************************************************
 */
#ifndef __BSP_UART_TTS_H
#define __BSP_UART_TTS_H

#include "main.h"

#define TTS_RX_BUF_SIZE 64
#define TTS_TIMEOUT_MS  15000

extern uint8_t  tts_rx_buf[TTS_RX_BUF_SIZE];
extern uint8_t  tts_rx_flag;
extern uint8_t  tts_rx_len;
extern uint8_t  tts_new_byte;
extern uint8_t  tts_last_byte;
extern uint8_t tts_rx_byte;
extern uint8_t tts_rx_idx;

void BSP_TTS_Init(void);
void BSP_TTS_SendByte(uint8_t data);
void BSP_TTS_SendString(const char *str);
void BSP_TTS_SendLine(const char *str);
void BSP_TTS_ClearRx(void);
void BSP_TTS_SendHex(const uint8_t *data, uint8_t len);
void BSP_TTS_SendHexLine(const uint8_t *data, uint8_t len);

#endif
