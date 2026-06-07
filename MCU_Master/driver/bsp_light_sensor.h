/**
 ************************************************************
 * @file    bsp_light_sensor.h
 * @brief   光敏传感器
 ************************************************************
 */
#ifndef __BSP_LIGHT_SENSOR_H
#define __BSP_LIGHT_SENSOR_H

#include "main.h"

#define LIGHT_DARK_THRESHOLD    3000    /* ADC值，大于此值=黑暗 */

void     BSP_LightSensor_Init(void);
uint16_t BSP_LightSensor_Read(void);       /* 单次ADC */
uint8_t  BSP_LightSensor_IsDark(void);     /* 1=黑暗 */

#endif
