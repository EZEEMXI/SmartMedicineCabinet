/**
 ************************************************************
 * @file    bsp_uart_asr.h
 * @brief   天问51 ASR UART3 驱动
 ************************************************************
 */
#ifndef __BSP_UART_ASR_H
#define __BSP_UART_ASR_H

#include "main.h"

#define ASR_RX_BUF_SIZE 64
#define ASR_TIMEOUT_MS  10000

extern uint8_t asr_rx_buf[ASR_RX_BUF_SIZE];
extern uint8_t asr_rx_flag;
extern uint8_t asr_rx_len;
extern uint8_t asr_rx_byte;
extern uint8_t asr_rx_idx;

void BSP_ASR_Init(void);
void BSP_ASR_SendByte(uint8_t d);
void BSP_ASR_SendCmd(uint8_t* data, uint8_t len);
void BSP_ASR_ClearRx(void);

#endif
