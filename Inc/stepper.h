#pragma once
#include "Base.h"

enum Degree
{
    Degree_0,
    Degree_45,
    Degree_90,
    Degree_135,
    Degree_180,
    Degree_225,
    Degree_270,
    Degree_315
};
typedef struct Stepper
{
    /**
     * @brief 当前角度
     */
    enum Degree cur;
    /**
     * @brief 控制脚位
     */
    pPin pins;
} Stepper_t, *pStepper;

/**
 * @brief 设置步进电机转动角度
 * 
 * @param i 
 */
void stepper_set_degree(pStepper stepper, enum Degree new_deg)
{
    switch (new_deg)
    {
    case Degree_0:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_RESET);
        break;
    case Degree_45:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_RESET);
        break;
    case Degree_90:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_RESET);
        break;
    case Degree_135:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_RESET);
        break;
    case Degree_180:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_RESET);
        break;
    case Degree_225:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_SET);
        break;
    case Degree_270:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_SET);
        break;
    case Degree_315:
        HAL_GPIO_WritePin(stepper->pins[0].port, stepper->pins[0].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(stepper->pins[1].port, stepper->pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[2].port, stepper->pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(stepper->pins[3].port, stepper->pins[3].pin, GPIO_PIN_SET);
        break;
    }
}
