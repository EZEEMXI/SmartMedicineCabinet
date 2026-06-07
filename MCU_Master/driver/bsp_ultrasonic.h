/**
 ************************************************************
 * @file    bsp_ultrasonic.h
 * @brief   超声波传感器
 ************************************************************
 */
#ifndef __BSP_ULTRASONIC_H
#define __BSP_ULTRASONIC_H

#include "main.h"

void    BSP_Ultrasonic_Init(void);
void    BSP_Ultrasonic_Process(void);
uint8_t BSP_Ultrasonic_IsPersonNear(void);

#endif
