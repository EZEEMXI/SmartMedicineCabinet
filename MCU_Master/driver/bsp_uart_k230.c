/**
 ************************************************************
 * @file    bsp_uart_k230.c
 * @brief   K230 UART3 中断接收
 ************************************************************
 */
#include "bsp_uart_k230.h"
#include "usart.h"
#include <string.h>

uint8_t k230_rx_buf[K230_RX_BUF_SIZE];
uint8_t k230_rx_flag = 0;
uint8_t k230_rx_len = 0;
uint8_t k230_rx_idx = 0;
uint8_t k230_rx_byte;

void BSP_UART_K230_Init(void)
{
    HAL_UART_Receive_IT(&huart3, &k230_rx_byte, 1);
}

void BSP_UART_K230_ClearRx(void)
{
    k230_rx_flag = 0;
    k230_rx_len = 0;
    k230_rx_idx = 0;
    memset((void*)k230_rx_buf, 0, K230_RX_BUF_SIZE);
}
