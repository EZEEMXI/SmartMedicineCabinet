/**
 ************************************************************
 * @file    bsp_uart_host.h
 * @brief   从机端主从通信 UART1 驱动
 ************************************************************
 */
#ifndef __BSP_UART_HOST_H
#define __BSP_UART_HOST_H

#include "main.h"

#define HOST_RX_BUF  16

extern uint8_t host_rx_buf[HOST_RX_BUF];
extern uint8_t host_rx_flag;
extern uint8_t host_rx_len;
extern uint8_t host_rx_byte;
extern uint8_t host_rx_idx;

void BSP_HostUART_Init(void);
void BSP_HostUART_SendFrame(uint8_t cmd, uint8_t data);
void BSP_HostUART_ClearRx(void);

#endif
