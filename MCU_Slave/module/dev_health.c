/**
 ************************************************************
 * @file    dev_health.c
 * @brief   心率检测 (采集+算法)
 ************************************************************
 */
#include "dev_health.h"
#include "bsp_i2c_max30102.h"
#include "max30102_alg.h"
#include "bsp_system.h"
#include <string.h>

#define REG_INTR_STATUS_1   0x00
#define REG_INTR_STATUS_2   0x01
#define REG_FIFO_DATA       0x07

typedef enum {
    H_IDLE,
    H_COLLECTING,
    H_DONE
} HState_t;

static HState_t h_state = H_IDLE;
static HealthResult_t result;
static uint8_t done = 0;

static uint32_t ir_buf[HEALTH_SAMPLE_COUNT];
static uint32_t red_buf[HEALTH_SAMPLE_COUNT];
static uint16_t idx = 0;
static uint32_t t_start = 0;
static uint8_t retry_cnt = 0;

void Dev_Health_Init(void)
{
    MAX30102_IIC_Init();

    MAX30102_WriteReg(0x09, 0x40); /* reset */
    MAX30102_WriteReg(0x09, 0x40);
    MAX30102_WriteReg(0x02, 0xC0);
    MAX30102_WriteReg(0x03, 0x00);
    MAX30102_WriteReg(0x08, 0x0F);
    MAX30102_WriteReg(0x09, 0x03);
    MAX30102_WriteReg(0x0A, 0x27);
    MAX30102_WriteReg(0x0C, 0x24);
    MAX30102_WriteReg(0x0D, 0x24);
    MAX30102_WriteReg(0x10, 0x7F);

    h_state = H_IDLE;
    done = 0;
}

void Dev_Health_Start(void)
{
		retry_cnt = 0;
    if (h_state != H_IDLE) return;

    /* 复位 FIFO */
    MAX30102_WriteReg(0x04, 0x00);  // FIFO 写指针清零
    MAX30102_WriteReg(0x05, 0x00);  // FIFO 溢出计数器清零
    MAX30102_WriteReg(0x06, 0x00);  // FIFO 读指针清零

    /* 重新配置，确保模式正确 */
    MAX30102_WriteReg(0x09, 0x40);  // 复位
    HAL_Delay(10);
    MAX30102_WriteReg(0x09, 0x03);  // SpO2 模式
    MAX30102_WriteReg(0x0A, 0x27);  // 100Hz
    MAX30102_WriteReg(0x0C, 0x24);  // LED1
    MAX30102_WriteReg(0x0D, 0x24);  // LED2

    idx = 0;
    done = 0;
    memset(&result, 0, sizeof(result));
    memset(ir_buf, 0, sizeof(ir_buf));
    memset(red_buf, 0, sizeof(red_buf));
    h_state = H_COLLECTING;
    t_start = HAL_GetTick();
}

void Dev_Health_Process(void)
{
    uint8_t fifo[6];
    uint8_t dummy;
				
    if (h_state != H_COLLECTING) return;

    if (MAX30102_INT_IsLow())
    {
        MAX30102_ReadReg(REG_INTR_STATUS_1, &dummy);
        MAX30102_ReadReg(REG_INTR_STATUS_2, &dummy);
        MAX30102_ReadFIFO(REG_FIFO_DATA, fifo);

        red_buf[idx] = ((uint32_t)(fifo[0] & 0x03) << 16) | ((uint32_t)fifo[1] << 8) | fifo[2];
        ir_buf[idx]  = ((uint32_t)(fifo[3] & 0x03) << 16) | ((uint32_t)fifo[4] << 8) | fifo[5];
        idx++;

        if (idx >= HEALTH_SAMPLE_COUNT)
        {
            int32_t spo2 = 0, hr = 0;
            int8_t sv = 0, hv = 0;

            maxim_heart_rate_and_oxygen_saturation(
                ir_buf, HEALTH_SAMPLE_COUNT, red_buf,
                &spo2, &sv, &hr, &hv);

            result.hr_valid = (hv && hr > 0 && hr < 250) ? 1 : 0;
            result.heart_rate = result.hr_valid ? (uint8_t)hr : 0;
            result.spo2_valid = (sv && spo2 > 0 && spo2 <= 100) ? 1 : 0;
            result.spo2 = result.spo2_valid ? (uint8_t)spo2 : 0;

						if (result.hr_valid == 0)
						{
								if (++retry_cnt < 3)  // 最多重试3次
								{
										idx = 0;
										t_start = HAL_GetTick();
										return;
								}
								// 超过重试次数，也结束
								h_state = H_DONE;
								done = 1;
								return;
						}
        }
    }

    if ((HAL_GetTick() - t_start) > HEALTH_TIMEOUT_MS)
    {
        h_state = H_DONE;
        done = 1;
    }
}

uint8_t Dev_Health_IsDone(void) { return done; }
HealthResult_t Dev_Health_GetResult(void) { return result; }

void Dev_Health_Clear(void)
{
    done = 0;
    h_state = H_IDLE;
}
