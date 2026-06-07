/**
 ************************************************************
 * @file    bsp_ultrasonic.c
 * @brief   超声波驱动
 ************************************************************
 */
#include "bsp_ultrasonic.h"
#include "bsp_system.h"

#define PERSON_DISTANCE_CM      80

static float    distance = 0;
static uint8_t  valid = 0;
static uint8_t  measuring = 0;
static uint32_t measure_start = 0;


void BSP_Ultrasonic_Process(void)
{
    uint32_t now = HAL_GetTick();

    if (!measuring)
    {
        HAL_GPIO_WritePin(us_trig_GPIO_Port, us_trig_Pin, GPIO_PIN_SET);
        BSP_DelayUs(20);
        HAL_GPIO_WritePin(us_trig_GPIO_Port, us_trig_Pin, GPIO_PIN_RESET);
        measure_start = now;
        measuring = 1;
        return;
    }

    if (HAL_GPIO_ReadPin(us_echo_GPIO_Port, us_echo_Pin) == GPIO_PIN_SET)
    {
        uint32_t start_us = HAL_GetTick() * 1000;
        uint32_t timeout = 30000;
        while (HAL_GPIO_ReadPin(us_echo_GPIO_Port, us_echo_Pin) == GPIO_PIN_SET && timeout--)
        {
            if ((HAL_GetTick() * 1000 - start_us) > 30000) break;
        }
        uint32_t duration = (HAL_GetTick() * 1000) - start_us;
        if (duration > 50 && duration < 30000)
        {
            distance = (float)duration / 58.0f;
            valid = 1;
        }
        else
        {
            valid = 0;
        }
        measuring = 0;
        return;
    }

    if ((now - measure_start) > 100)
    {
        valid = 0;
        measuring = 0;
    }
}

uint8_t BSP_Ultrasonic_IsPersonNear(void)
{
    return (distance > 0 && distance < PERSON_DISTANCE_CM) ? 1 : 0;
}
