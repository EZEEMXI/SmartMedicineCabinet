/**
 ************************************************************
 * @file    bsp_uart_slave.c
 * @brief   主从UART1 (主机端) 固定4字节帧
 *          帧格式: [CMD][DATA][XOR][0x0D]
 ************************************************************
 */
#include "bsp_uart_slave.h"
#include "usart.h"
#include <string.h>

uint8_t slave_rx_buf[SLAVE_RX_BUF_SIZE];
uint8_t slave_rx_flag = 0;
uint8_t slave_rx_len = 0;
uint8_t slave_rx_idx = 0;
uint8_t slave_rx_byte;

void BSP_UART_Slave_Init(void)
{
    HAL_UART_Receive_IT(&huart1, &slave_rx_byte, 1);
}

void BSP_UART_Slave_SendFrame(uint8_t cmd, uint8_t data)
{
    uint8_t frame[4];
    frame[0] = cmd;
    frame[1] = data;
    frame[2] = cmd ^ data;
    frame[3] = 0x0D;
    HAL_UART_Transmit(&huart1, frame, 4, 100);
}

void BSP_UART_Slave_ClearRx(void)
{
    slave_rx_flag = 0;
    slave_rx_len = 0;
    slave_rx_idx = 0;
    memset((void*)slave_rx_buf, 0, SLAVE_RX_BUF_SIZE);
}
