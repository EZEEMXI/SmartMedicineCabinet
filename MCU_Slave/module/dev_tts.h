/**
 ************************************************************
 * @file    dev_tts.h
 * @brief   TTS 播报控制
 ************************************************************
 */
#ifndef __DEV_TTS_H
#define __DEV_TTS_H

#include "main.h"

// TTS设备状态
typedef enum {
    TTS_STATE_IDLE = 0,       // 空闲
    TTS_STATE_WAIT_START,     // 已发送文本，等待回传 'A'
    TTS_STATE_BUSY,           // 正在播报，等待回传 'O'
    TTS_STATE_DONE,           // 播报完成（待上层读取）
    TTS_STATE_ERROR           // 超时错误
} TTS_State_t;

void Dev_TTS_Init(void);
void Dev_TTS_Speak(const char *text);
void Dev_TTS_SpeakHex(const uint8_t *hex_data, uint8_t len);
void Dev_TTS_SpeakHeartRate(uint8_t hr);
void Dev_TTS_SpeakMedicine(uint8_t medicine_id);
void Dev_TTS_SpeakWelcome(uint8_t user_type);
void Dev_TTS_SpeakOpenDoor(uint8_t cabinet_id);
void Dev_TTS_Stop(void);
void Dev_TTS_Process(void);

TTS_State_t Dev_TTS_GetState(void);
uint8_t Dev_TTS_IsDone(void);
void Dev_TTS_ClearDone(void);

#endif
