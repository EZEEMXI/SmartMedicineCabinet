#include "bsp_uart_scanner.h"
#include "usart.h"
#include <string.h>

uint8_t scanner_rx_buf[SCANNER_RX_BUF_SIZE];
uint8_t scanner_rx_flag = 0;
uint8_t scanner_rx_len  = 0;
uint8_t scanner_rx_idx  = 0;
uint8_t scanner_rx_byte;

void BSP_UART_Scanner_ClearRx(void)
{
    scanner_rx_flag = 0;
    scanner_rx_len  = 0;
    scanner_rx_idx  = 0;
    memset((void*)scanner_rx_buf, 0, SCANNER_RX_BUF_SIZE);
}
