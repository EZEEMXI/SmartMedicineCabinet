/**
 ************************************************************
 * @file    bsp_uart_wifi.c
 * @brief   ESP8266 UART2 (HAL, 分时复用扫码模块)
 ************************************************************
 */
#include "bsp_uart_wifi.h"
#include "usart.h"
#include <string.h>

/* ESP8266 专用缓冲 */
volatile uint8_t  wifi_rx_buf[WIFI_RX_BUF_SIZE];
volatile uint8_t  wifi_rx_flag = 0;
volatile uint16_t wifi_rx_len = 0;
uint8_t           wifi_rx_byte;                /* HAL 单字节接收缓冲 */
uint16_t          wifi_rx_idx = 0;             /* 全局，供回调使用 */


void BSP_UART2_Init(void)
{
    HAL_UART_Receive_IT(&huart2, &wifi_rx_byte, 1);
}

/*==================================================================
 * @brief  发送一个字节 (WiFi 模式专用)
 *==================================================================*/
void BSP_UART_WiFi_SendByte(uint8_t byte)
{
    HAL_UART_Transmit(&huart2, &byte, 1, 100);
}

void BSP_UART_WiFi_SendArray(uint8_t* data, uint16_t len)
{
    HAL_UART_Transmit(&huart2, data, len, 500);
}

void BSP_UART_WiFi_SendString(char* str)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 500);
}

/*==================================================================
 * @brief  清空 WiFi 接收缓冲
 *==================================================================*/
void BSP_UART_WiFi_ClearRx(void)
{
    wifi_rx_idx  = 0;
    wifi_rx_flag = 0;
    wifi_rx_len  = 0;
    memset((void*)wifi_rx_buf, 0, WIFI_RX_BUF_SIZE);
}
