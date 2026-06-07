/**
 ************************************************************
 * @file    bsp_uart_slave.h
 * @brief   主从通信 UART1 驱动 (主机端)
 ************************************************************
 */
#ifndef __BSP_UART_SLAVE_H
#define __BSP_UART_SLAVE_H

#include "main.h"

#define SLAVE_RX_BUF_SIZE   16

extern uint8_t slave_rx_buf[SLAVE_RX_BUF_SIZE];
extern uint8_t slave_rx_flag;
extern uint8_t slave_rx_len;
extern uint8_t slave_rx_idx;
extern uint8_t slave_rx_byte;

void BSP_UART_Slave_Init(void);
void BSP_UART_Slave_SendFrame(uint8_t cmd, uint8_t data);
void BSP_UART_Slave_ClearRx(void);

#endif
