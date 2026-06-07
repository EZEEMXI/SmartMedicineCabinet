/**
 ************************************************************
 * @file    bsp_i2c_max30102.c
 * @brief   软件I2C实现 (PB7=SCL, PB8=SDA)
 ************************************************************
 */
#include "bsp_i2c_max30102.h"
#include "bsp_system.h"

#define I2C_ADDR    0xAE

#define SCL_H   HAL_GPIO_WritePin(GPIOB, MAX30102_SCL_Pin, GPIO_PIN_SET)
#define SCL_L   HAL_GPIO_WritePin(GPIOB, MAX30102_SCL_Pin, GPIO_PIN_RESET)
#define SDA_H   HAL_GPIO_WritePin(GPIOB, MAX30102_SDA_Pin, GPIO_PIN_SET)
#define SDA_L   HAL_GPIO_WritePin(GPIOB, MAX30102_SDA_Pin, GPIO_PIN_RESET)
#define SDA_R   HAL_GPIO_ReadPin(GPIOB, MAX30102_SDA_Pin)

static void sda_out(void)
{
    GPIO_InitTypeDef g;
    g.Pin   = MAX30102_SDA_Pin;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOB, &g);
}

static void sda_in(void)
{
    GPIO_InitTypeDef g;
    g.Pin  = MAX30102_SDA_Pin;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &g);
}

void MAX30102_IIC_Init(void)
{
    SCL_H; SDA_H;
}

static void i2c_start(void)
{
    sda_out(); SDA_H; SCL_H; BSP_DelayUs(4);
    SDA_L; BSP_DelayUs(4); SCL_L;
}

static void i2c_stop(void)
{
    sda_out(); SCL_L; SDA_L; BSP_DelayUs(4);
    SCL_H; SDA_H; BSP_DelayUs(4);
}

static uint8_t i2c_wait_ack(void)
{
    uint8_t t = 0;
    sda_in(); SDA_H; BSP_DelayUs(1); SCL_H; BSP_DelayUs(1);
    while (SDA_R) { if (++t > 250) { i2c_stop(); return 1; } }
    SCL_L; return 0;
}

static void i2c_ack(void)
{
    SCL_L; sda_out(); SDA_L; BSP_DelayUs(2);
    SCL_H; BSP_DelayUs(2); SCL_L;
}

static void i2c_nack(void)
{
    SCL_L; sda_out(); SDA_H; BSP_DelayUs(2);
    SCL_H; BSP_DelayUs(2); SCL_L;
}

static void i2c_send_byte(uint8_t d)
{
    sda_out(); SCL_L;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (d & 0x80) SDA_H; else SDA_L;
        d <<= 1; BSP_DelayUs(2);
        SCL_H; BSP_DelayUs(2); SCL_L; BSP_DelayUs(2);
    }
}

static uint8_t i2c_read_byte(uint8_t ack)
{
    uint8_t r = 0;
    sda_in();
    for (uint8_t i = 0; i < 8; i++)
    {
        SCL_L; BSP_DelayUs(2); SCL_H;
        r <<= 1; if (SDA_R) r++;
        BSP_DelayUs(1);
    }
    if (ack) i2c_ack(); else i2c_nack();
    return r;
}

uint8_t MAX30102_WriteReg(uint8_t reg, uint8_t data)
{
    i2c_start();
    i2c_send_byte(I2C_ADDR & 0xFE); if (i2c_wait_ack()) return 1;
    i2c_send_byte(reg); if (i2c_wait_ack()) return 1;
    i2c_send_byte(data); if (i2c_wait_ack()) return 1;
    i2c_stop();
    return 0;
}

uint8_t MAX30102_ReadReg(uint8_t reg, uint8_t* data)
{
    i2c_start();
    i2c_send_byte(I2C_ADDR & 0xFE); if (i2c_wait_ack()) { i2c_stop(); return 1; }
    i2c_send_byte(reg); if (i2c_wait_ack()) { i2c_stop(); return 1; }
    i2c_start();
    i2c_send_byte(I2C_ADDR | 0x01); if (i2c_wait_ack()) { i2c_stop(); return 1; }
    *data = i2c_read_byte(0);
    i2c_stop();
    return 0;
}

void MAX30102_ReadFIFO(uint8_t reg, uint8_t* data)
{
    i2c_start();
    i2c_send_byte(I2C_ADDR & 0xFE); i2c_wait_ack();
    i2c_send_byte(reg); i2c_wait_ack();
    i2c_start();
    i2c_send_byte(I2C_ADDR | 0x01); i2c_wait_ack();
    for (uint8_t i = 0; i < 5; i++) data[i] = i2c_read_byte(1);
    data[5] = i2c_read_byte(0);
    i2c_stop();
}

uint8_t MAX30102_INT_IsLow(void)
{
    return (HAL_GPIO_ReadPin(MAX30102_INT_GPIO_Port, MAX30102_INT_Pin) == GPIO_PIN_RESET) ? 1 : 0;
}
