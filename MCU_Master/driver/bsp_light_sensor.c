/**
 ************************************************************
 * @file    bsp_light_sensor.c
 * @brief   光敏 ADC 驱动
 ************************************************************
 */
#include "bsp_light_sensor.h"
#include "adc.h"

void BSP_LightSensor_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
}

uint16_t BSP_LightSensor_Read(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    return HAL_ADC_GetValue(&hadc1);
}

uint8_t BSP_LightSensor_IsDark(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        sum += BSP_LightSensor_Read();
        HAL_Delay(2);
    }
    return ((sum / 8) > LIGHT_DARK_THRESHOLD) ? 1 : 0;
}
