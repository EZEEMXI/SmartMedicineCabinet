/**
 ************************************************************
 * @file    bsp_uart_wifi.h
 * @brief   ESP8266 串口驱动 (UART2)
 ************************************************************
 */
#ifndef __BSP_UART_WIFI_H
#define __BSP_UART_WIFI_H

#include "main.h"
#include "bsp_uart_scanner.h"

#define WIFI_RX_BUF_SIZE    512
#define UART2_MODE_WIFI     0
#define UART2_MODE_SCANNER  1

extern volatile uint8_t wifi_rx_buf[WIFI_RX_BUF_SIZE];
extern volatile uint8_t wifi_rx_flag;
extern volatile uint16_t wifi_rx_len;
extern uint8_t wifi_rx_byte;
extern uint16_t wifi_rx_idx;

void BSP_UART2_Init(void);
void BSP_UART_WiFi_SendByte(uint8_t byte);
void BSP_UART_WiFi_SendArray(uint8_t* data, uint16_t len);
void BSP_UART_WiFi_SendString(char* str);
void BSP_UART_WiFi_ClearRx(void);

#endif
