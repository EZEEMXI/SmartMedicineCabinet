/**
 ************************************************************
 * @file    bsp_relay.c
 * @brief   继电器驱动实现
 * @note    开锁脉冲1.86秒后自动关断
 ************************************************************
 */
#include "bsp_relay.h"
#include "bsp_system.h"

#define OPEN_PULSE_MS   1860
#define DOOR_COUNT      2

typedef struct {
    DoorId_t id;
    GPIO_TypeDef* ctrl_port;
    uint16_t ctrl_pin;
    uint32_t pulse_start;
    uint8_t pulse_active;
} DoorCtrl_t;

static DoorCtrl_t door_ctrl[DOOR_COUNT];

void BSP_Relay_Init(void)
{
    door_ctrl[0] = (DoorCtrl_t){DOOR_1, GPIOB, relay1_Pin, 0, 0};
    door_ctrl[1] = (DoorCtrl_t){DOOR_2, GPIOB, relay2_Pin, 0, 0};
}

void BSP_Relay_Open(DoorId_t door)
{
    if (door < 1 || door > 2) return;
    DoorCtrl_t *ctrl = &door_ctrl[door - 1];
    HAL_GPIO_WritePin(ctrl->ctrl_port, ctrl->ctrl_pin, GPIO_PIN_RESET);
    ctrl->pulse_start = HAL_GetTick();
    ctrl->pulse_active = 1;
}

void BSP_Relay_Close(DoorId_t door)
{
    if (door < 1 || door > 2) return;
    DoorCtrl_t *ctrl = &door_ctrl[door - 1];
    HAL_GPIO_WritePin(ctrl->ctrl_port, ctrl->ctrl_pin, GPIO_PIN_SET);
    ctrl->pulse_active = 0;
}

uint8_t BSP_Relay_IsLocked(DoorId_t door)
{
    if (door < 1 || door > 2) return 1;
    return (door_ctrl[door - 1].pulse_active == 0) ? 1 : 0;
}

void BSP_Relay_Process(void)
{
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0; i < DOOR_COUNT; i++)
    {
        if (door_ctrl[i].pulse_active)
        {
            if ((now - door_ctrl[i].pulse_start) >= OPEN_PULSE_MS)
            {
                HAL_GPIO_WritePin(door_ctrl[i].ctrl_port, door_ctrl[i].ctrl_pin, GPIO_PIN_SET);
                door_ctrl[i].pulse_active = 0;
            }
        }
    }
}
