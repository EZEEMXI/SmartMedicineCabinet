/**
 ************************************************************
 * @file    bsp_uart_host.c
 * @brief   从机UART1 固定4字节帧
 ************************************************************
 */
#include "bsp_uart_host.h"
#include "usart.h"
#include <string.h>

uint8_t host_rx_buf[HOST_RX_BUF];
uint8_t host_rx_flag = 0;
uint8_t host_rx_len  = 0;
uint8_t host_rx_byte;
uint8_t host_rx_idx  = 0;

void BSP_HostUART_Init(void)
{
    HAL_UART_Receive_IT(&huart1, &host_rx_byte, 1);
}

void BSP_HostUART_SendFrame(uint8_t cmd, uint8_t data)
{
    uint8_t frame[4];
    frame[0] = cmd;
    frame[1] = data;
    frame[2] = cmd ^ data;
    frame[3] = 0x0D;
    HAL_UART_Transmit(&huart1, frame, 4, 100);
}

void BSP_HostUART_ClearRx(void)
{
    host_rx_flag = 0;
    host_rx_len  = 0;
    host_rx_idx  = 0;
    memset((void*)host_rx_buf, 0, HOST_RX_BUF);
}
