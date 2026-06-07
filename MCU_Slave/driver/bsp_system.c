/**
 ************************************************************
 * @file    bsp_system.c
 * @brief   系统时钟 72MHz + SysTick 1ms
 ************************************************************
 */
#include "bsp_system.h"

/*==========================================================
 * @function    BSP_DelayUs
 * @brief       微秒延时（72MHz下，循环9次≈1us）
 *==========================================================*/
void BSP_DelayUs(uint32_t us)
{
    uint32_t i;
    for (; us > 0; us--)
    {
        i = 9;
        while (i--) __NOP();
    }
}

void uart1_print(const char *str)
{
    while (*str)
    {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = *str++;
    }
}

void uart1_print_num(int num)
{
    char buf[16];
    int i = 0;
    if (num == 0)
    {
        uart1_print("0");
        return;
    }
    if (num < 0) { uart1_print("-"); num = -num; }
    while (num > 0)
    {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = i - 1; j >= 0; j--)
    {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = buf[j];
    }
}

void uart1_print_hex(uint8_t val)
{
    uint8_t h = (val >> 4) & 0x0F;
    uint8_t l = val & 0x0F;
    char c1 = (h < 10) ? '0' + h : 'A' + h - 10;
    char c2 = (l < 10) ? '0' + l : 'A' + l - 10;
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c1;
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c2;
}
