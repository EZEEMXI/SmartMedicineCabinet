/**
 ************************************************************
 * @file    bsp_uart_k230.h
 * @brief   K230 串口驱动 (UART3)
 ************************************************************
 */
#ifndef __BSP_UART_K230_H
#define __BSP_UART_K230_H

#include "main.h"

#define K230_RX_BUF_SIZE    64

extern uint8_t  k230_rx_buf[K230_RX_BUF_SIZE];
extern uint8_t  k230_rx_flag;
extern uint8_t  k230_rx_len;
extern uint8_t k230_rx_idx;
extern uint8_t k230_rx_byte;

void BSP_UART_K230_Init(void);
void BSP_UART_K230_ClearRx(void);

#endif
