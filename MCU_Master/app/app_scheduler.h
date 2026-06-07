/**
 ************************************************************
 * @file    app_scheduler.h
 * @brief   主机主调度器
 ************************************************************
 */
#ifndef __APP_SCHEDULER_H
#define __APP_SCHEDULER_H

#include "app_config.h"

extern volatile uint8_t uart2_mode;

void App_Scheduler_Init(void);
void App_Scheduler_Run(void);

#endif
