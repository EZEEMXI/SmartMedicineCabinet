/**
  * @file    ws2812.c
  * @brief   WS2812B 灯带驱动
  * @note    关键问题：STM32的3.3V信号需要配合4.5V供电，且'0'码高电平必须极短
  *          最终方案：'0'码高电平只用3个__NOP()（约0.04us）
  */

#include "ws2812.h"

#define WS2812_LED_NUM     30

// 每个灯珠3字节，G、R、B
static uint8_t led_data[WS2812_LED_NUM * 3];

static void delay_nops(uint32_t n)
{
    while (n--) { __NOP(); }
}

/** @brief  发送一个字节
  * @param  byte : 要发送的字节数据
  * @note   关键时序说明：
  *         1. WS2812B判断逻辑'0'和'1'的依据是高电平持续的时间
  *         2. 标准要求：'0'高电平 < 0.45us，'1'高电平 > 0.45us
  *         3. 实测发现：'0'高电平必须极短（约0.04us），否则会被误判为'1'
  *         
  *         时序参数（72MHz主频）：
  *         - '0'码：高电平 3个NOP ≈ 0.04us，低电平 80个NOP ≈ 1.11us
  *         - '1'码：高电平 60个NOP ≈ 0.83us，低电平 30个NOP ≈ 0.42us
  *         - 总周期：约1.25us（符合800kHz要求）
  */
static void send_byte(uint8_t byte)
{
    if (byte & 0x80)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x40)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x20)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x10)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x08)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x04)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x02)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
    if (byte & 0x01)
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET); delay_nops(60);
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET); delay_nops(30);
    }
    else
    {
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_SET);
        __NOP(); __NOP(); __NOP();
        HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
        delay_nops(80);
    }
}

void WS2812_Init(void)
{
    HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
    WS2812_Clear();
}

void WS2812_SetColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= WS2812_LED_NUM) return;
    led_data[index * 3 + 0] = g;
    led_data[index * 3 + 1] = r;
    led_data[index * 3 + 2] = b;
}

void WS2812_SetAllColor(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint16_t i = 0; i < WS2812_LED_NUM; i++)
    {
        led_data[i * 3 + 0] = g;
        led_data[i * 3 + 1] = r;
        led_data[i * 3 + 2] = b;
    }
}

void WS2812_Clear(void)
{
    for (uint16_t i = 0; i < WS2812_LED_NUM * 3; i++)
    {
        led_data[i] = 0;
    }
}

void WS2812_Show(void)
{
    __disable_irq();

    HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
    delay_nops(5000);

    for (uint32_t i = 0; i < WS2812_LED_NUM * 3; i++)
    {
        send_byte(led_data[i]);
    }

    HAL_GPIO_WritePin(ws2812_GPIO_Port, ws2812_Pin, GPIO_PIN_RESET);
    delay_nops(5000);

    __enable_irq();
}

uint16_t WS2812_GetLedNum(void)
{
    return WS2812_LED_NUM;
}

void WS2812_AllOn(void)
{
    WS2812_SetAllColor(255, 255, 255);
    WS2812_Show();
}

void WS2812_AllOff(void)
{
    WS2812_Clear();
    WS2812_Show();
}
