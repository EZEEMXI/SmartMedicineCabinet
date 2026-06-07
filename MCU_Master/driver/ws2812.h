#ifndef __WS2812_H
#define __WS2812_H

#include "main.h"

// ==================== 配置区域 ====================


// =================================================

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} WS2812_Color;

void WS2812_Init(void);
void WS2812_SetColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SetAllColor(uint8_t r, uint8_t g, uint8_t b);
void WS2812_Clear(void);
void WS2812_Show(void);
uint16_t WS2812_GetLedNum(void);
void WS2812_AllOn(void);
void WS2812_AllOff(void);

void WS2812_Show_Single(void);

#endif
