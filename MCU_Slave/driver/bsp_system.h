/**
 ************************************************************
 * @file    bsp_system.h
 * @brief   从机系统时钟
 ************************************************************
 */
#ifndef __BSP_SYSTEM_H
#define __BSP_SYSTEM_H

#include "main.h"

void BSP_DelayUs(uint32_t us);

void uart1_print(const char *str);
void uart1_print_num(int num);
void uart1_print_hex(uint8_t val);

#endif
