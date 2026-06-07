/**
 ************************************************************
 * @file    dev_health.h
 * @brief   心率血氧检测状态机
 ************************************************************
 */
#ifndef __DEV_HEALTH_H
#define __DEV_HEALTH_H

#include "main.h"

#define HEALTH_SAMPLE_COUNT  100
#define HEALTH_TIMEOUT_MS    10000

typedef struct {
    uint8_t heart_rate;
    uint8_t hr_valid;
    uint8_t spo2;
    uint8_t spo2_valid;
} HealthResult_t;

void Dev_Health_Init(void);
void Dev_Health_Start(void);
void Dev_Health_Process(void);
uint8_t Dev_Health_IsDone(void);
HealthResult_t Dev_Health_GetResult(void);
void Dev_Health_Clear(void);

#endif
