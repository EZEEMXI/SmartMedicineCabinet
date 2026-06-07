/**
 ************************************************************
 * @file    bsp_i2c_max30102.h
 * @brief   MAX30102 I2C 驱动
 ************************************************************
 */
#ifndef __BSP_I2C_MAX30102_H
#define __BSP_I2C_MAX30102_H

#include "main.h"

void MAX30102_IIC_Init(void);
uint8_t MAX30102_WriteReg(uint8_t reg, uint8_t data);
uint8_t MAX30102_ReadReg(uint8_t reg, uint8_t* data);
void MAX30102_ReadFIFO(uint8_t reg, uint8_t* data);
uint8_t MAX30102_INT_IsLow(void);

#endif
