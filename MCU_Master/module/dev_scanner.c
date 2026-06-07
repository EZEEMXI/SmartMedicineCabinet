#include "dev_scanner.h"
#include "bsp_uart_scanner.h"
#include "medicine_db.h"
#include <string.h>

static uint8_t done = 0;
static char code_buf[16];

void Dev_Scanner_Init(void)
{
    done = 0;
}

void Dev_Scanner_Process(void)
{
    if (scanner_rx_flag)
    {
        strncpy(code_buf, (const char*)scanner_rx_buf, 15);
        code_buf[15] = '\0';
        done = 1;
        BSP_UART_Scanner_ClearRx();
    }
}

uint8_t Dev_Scanner_IsDone(void)
{
    return done;
}

const char* Dev_Scanner_GetCode(void)
{
    return code_buf;
}

void Dev_Scanner_Clear(void)
{
    done = 0;
    memset(code_buf, 0, sizeof(code_buf));
}
