#ifndef __BSP_UART_SCANNER_H
#define __BSP_UART_SCANNER_H

#include "main.h"

#define SCANNER_RX_BUF_SIZE  32

extern uint8_t scanner_rx_buf[SCANNER_RX_BUF_SIZE];
extern uint8_t scanner_rx_flag;
extern uint8_t scanner_rx_len;
extern uint8_t scanner_rx_idx;
extern uint8_t scanner_rx_byte;

void BSP_UART_Scanner_ClearRx(void);

#endif
