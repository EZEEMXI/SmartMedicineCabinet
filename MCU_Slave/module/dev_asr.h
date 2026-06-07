/**
 ************************************************************
 * @file    dev_asr.h
 * @brief   语音识别控制
 ************************************************************
 */
#ifndef __DEV_ASR_H
#define __DEV_ASR_H

#include "main.h"

typedef enum {
    ASR_CMD_NONE = 0,
    ASR_CMD_HEART_RATE = 1,
    ASR_CMD_OPEN_1 = 2,
    ASR_CMD_OPEN_2 = 3,
    ASR_CMD_UNKNOWN = 0xFF
} ASR_Cmd_t;

void Dev_ASR_Init(void);
void Dev_ASR_Start(void);
void Dev_ASR_Process(void);
ASR_Cmd_t Dev_ASR_GetCommand(void);
void Dev_ASR_Clear(void);

#endif
