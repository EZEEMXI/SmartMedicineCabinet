/**
 ************************************************************
 * @file    dev_tts.c
 * @brief   TTS设备抽象层实现
 ************************************************************
 */
#include "dev_tts.h"
#include "bsp_uart_tts.h"
#include "bsp_system.h"
#include "medicine_db.h"
#include <string.h>

static void uart1_print(const char *str)
{
    while (*str)
    {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = *str++;
    }
}

static TTS_State_t tts_state = TTS_STATE_IDLE;
static uint32_t tts_tick_start = 0;
static uint8_t tts_done_flag = 0;

static uint8_t NumberToChinese(uint8_t num, uint8_t *out)
{
    static const uint8_t chinese_digits[10][2] = {
        {0xC1,0xE3},{0xD2,0xBB},{0xB6,0xFE},{0xC8,0xFD},{0xCB,0xC4},
        {0xCE,0xE5},{0xC1,0xF9},{0xC6,0xDF},{0xB0,0xCB},{0xBE,0xC5}
    };
    static const uint8_t ten[2] = {0xCA,0xAE};
    static const uint8_t hundred[2] = {0xB0,0xD9};
    uint8_t idx = 0;

    if (num == 0)
    {
        out[idx++] = chinese_digits[0][0];
        out[idx++] = chinese_digits[0][1];
        return idx;
    }
    if (num >= 100)
    {
        uint8_t bai = num / 100;
        uint8_t rem = num % 100;
        out[idx++] = chinese_digits[bai][0];
        out[idx++] = chinese_digits[bai][1];
        out[idx++] = hundred[0];
        out[idx++] = hundred[1];
        if (rem == 0) return idx;
        if (rem < 10)
        {
            out[idx++] = chinese_digits[0][0];
            out[idx++] = chinese_digits[0][1];
            out[idx++] = chinese_digits[rem][0];
            out[idx++] = chinese_digits[rem][1];
        }
        else if (rem < 20)
        {
            out[idx++] = ten[0];
            out[idx++] = ten[1];
            if (rem - 10 > 0)
            {
                out[idx++] = chinese_digits[rem - 10][0];
                out[idx++] = chinese_digits[rem - 10][1];
            }
        }
        else
        {
            uint8_t shi = rem / 10;
            uint8_t ge = rem % 10;
            out[idx++] = chinese_digits[shi][0];
            out[idx++] = chinese_digits[shi][1];
            out[idx++] = ten[0];
            out[idx++] = ten[1];
            if (ge > 0)
            {
                out[idx++] = chinese_digits[ge][0];
                out[idx++] = chinese_digits[ge][1];
            }
        }
        return idx;
    }
    if (num >= 20)
    {
        uint8_t shi = num / 10;
        uint8_t ge = num % 10;
        out[idx++] = chinese_digits[shi][0];
        out[idx++] = chinese_digits[shi][1];
        out[idx++] = ten[0];
        out[idx++] = ten[1];
        if (ge > 0)
        {
            out[idx++] = chinese_digits[ge][0];
            out[idx++] = chinese_digits[ge][1];
        }
        return idx;
    }
    if (num >= 10)
    {
        out[idx++] = ten[0];
        out[idx++] = ten[1];
        if (num - 10 > 0)
        {
            out[idx++] = chinese_digits[num - 10][0];
            out[idx++] = chinese_digits[num - 10][1];
        }
        return idx;
    }
    out[idx++] = chinese_digits[num][0];
    out[idx++] = chinese_digits[num][1];
    return idx;
}

void Dev_TTS_Init(void)
{
    BSP_TTS_Init();
    HAL_Delay(2000); 
    tts_state = TTS_STATE_IDLE;
    tts_done_flag = 0;
    uart1_print("Dev_TTS: Init OK\r\n");
}

void Dev_TTS_SpeakHex(const uint8_t *hex_data, uint8_t len)
{
    if (tts_state != TTS_STATE_IDLE)
    {
        uart1_print("Dev_TTS: Busy\r\n");
        return;
    }
    BSP_TTS_ClearRx();
    BSP_TTS_SendHexLine(hex_data, len);
    tts_state = TTS_STATE_WAIT_START;
    tts_tick_start = HAL_GetTick();
    tts_done_flag = 0;
}

void Dev_TTS_SpeakHeartRate(uint8_t hr)
{
		if (tts_state != TTS_STATE_IDLE)
        uart1_print("TTS BUSY!\r\n");
	
    uint8_t prefix[] = {0xD0,0xC4,0xC2,0xCA,0xCE,0xAA};
    uint8_t suffix[] = {0xB4,0xCE,0xC3,0xBF,0xB7,0xD6,0xD6,0xD3};
    uint8_t num_chinese[12];
    uint8_t num_len = NumberToChinese(hr, num_chinese);
    uint8_t full_data[64];
    uint8_t idx = 0;
    memcpy(&full_data[idx], prefix, sizeof(prefix)); idx += sizeof(prefix);
    memcpy(&full_data[idx], num_chinese, num_len); idx += num_len;
    memcpy(&full_data[idx], suffix, sizeof(suffix)); idx += sizeof(suffix);
    Dev_TTS_SpeakHex(full_data, idx);
//		uint8_t test[] = {0xBB,0xB6,0xD3,0xAD};
//    Dev_TTS_SpeakHex(test, sizeof(test));
}

void Dev_TTS_SpeakMedicine(uint8_t idx)
{
    if (idx >= MEDICINE_COUNT) return;

    const Medicine_t *med = &medicine_db[idx];

    /* 播报：药品名 + 描述 */
    uint8_t full_data[256];
    uint8_t pos = 0;

    memcpy(&full_data[pos], med->name_gb, med->name_gb_len);
    pos += med->name_gb_len;

    full_data[pos++] = 0xA3;  // "，"  GB2312逗号，隔开
    full_data[pos++] = 0xAC;

    memcpy(&full_data[pos], med->desc_gb, med->desc_gb_len);
    pos += med->desc_gb_len;

    Dev_TTS_SpeakHex(full_data, pos);
}

void Dev_TTS_SpeakWelcome(uint8_t user_type)
{
    if (user_type == 1)
    {
        uint8_t text[] = {0xBB,0xB6,0xD3,0xAD,0xCA,0xB9,0xD3,0xC3,
                          0xD6,0xC7,0xC4,0xDC,0xD2,0xA9,0xB9,0xF1};
        Dev_TTS_SpeakHex(text, sizeof(text));
    }
    else if (user_type == 2)
    {
        uint8_t text[] = {0xBF,0xEC,0xB5,0xDD,0xD4,0xB1,0xC4,0xE3,
                          0xBA,0xC3,0xC7,0xEB,0xB7,0xC5,0xC8,0xEB,
                          0xD2,0xA9,0xC6,0xB7};
        Dev_TTS_SpeakHex(text, sizeof(text));
    }
    else
    {
        uint8_t text[] = {0xBB,0xB6,0xD3,0xAD};
        Dev_TTS_SpeakHex(text, sizeof(text));
    }
}

void Dev_TTS_SpeakOpenDoor(uint8_t cabinet_id)
{
    uint8_t prefix[] = {0xD2,0xA9,0xB9,0xF1};
    uint8_t suffix[] = {0xD2,0xD1,0xB4,0xF2,0xBF,0xAA};
    uint8_t num_chinese[12];
    uint8_t num_len = NumberToChinese(cabinet_id, num_chinese);
    uint8_t full_data[64];
    uint8_t idx = 0;
    memcpy(&full_data[idx], prefix, sizeof(prefix)); idx += sizeof(prefix);
    memcpy(&full_data[idx], num_chinese, num_len); idx += num_len;
    memcpy(&full_data[idx], suffix, sizeof(suffix)); idx += sizeof(suffix);
    Dev_TTS_SpeakHex(full_data, idx);
}

void Dev_TTS_Stop(void)
{
    tts_state = TTS_STATE_IDLE;
    tts_done_flag = 0;
}

void Dev_TTS_Process(void)
{
    switch (tts_state)
    {
        case TTS_STATE_IDLE:
        case TTS_STATE_DONE:
        case TTS_STATE_ERROR:
            break;

        case TTS_STATE_WAIT_START:
            if (tts_new_byte && tts_last_byte == 'A')
            {
                tts_new_byte = 0;
                tts_state = TTS_STATE_BUSY;
                tts_tick_start = HAL_GetTick();
            }
            else if (HAL_GetTick() - tts_tick_start > 2000)
            {
                tts_state = TTS_STATE_BUSY;
                tts_tick_start = HAL_GetTick();
            }
            break;

        case TTS_STATE_BUSY:
            if (tts_new_byte && tts_last_byte == 'O')
            {
                tts_new_byte = 0;
                tts_state = TTS_STATE_DONE;
                tts_done_flag = 1;
            }
            else if (HAL_GetTick() - tts_tick_start > TTS_TIMEOUT_MS)
            {
                tts_state = TTS_STATE_ERROR;
                tts_done_flag = 1;
            }
            break;
    }
}

TTS_State_t Dev_TTS_GetState(void)
{
    return tts_state;
}

uint8_t Dev_TTS_IsDone(void)
{
    return tts_done_flag;
}

void Dev_TTS_ClearDone(void)
{
    tts_done_flag = 0;
    tts_state = TTS_STATE_IDLE;
}
