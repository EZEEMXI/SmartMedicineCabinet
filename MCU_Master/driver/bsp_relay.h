/**
 ************************************************************
 * @file    bsp_relay.h
 * @brief   继电器+电磁锁驱动 (2门)
 ************************************************************
 */
#ifndef __BSP_RELAY_H
#define __BSP_RELAY_H

#include "main.h"

typedef enum {
    DOOR_1 = 1,
    DOOR_2 = 2
} DoorId_t;

void BSP_Relay_Init(void);
void BSP_Relay_Open(DoorId_t door);       /* 开锁脉冲 */
void BSP_Relay_Close(DoorId_t door);      /* 强制关锁 */
uint8_t BSP_Relay_IsLocked(DoorId_t door); /* 读反馈引脚，1=锁定 */
void BSP_Relay_Process(void);             /* 主循环调用，处理脉冲超时 */

#endif
