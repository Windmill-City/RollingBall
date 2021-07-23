/**
 * @file RollingBall.h
 * @author Windmill_City
 * @brief 滚球控制系统 - 核心文件
 * @version 0.1
 * @date 2021-07-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "tim.h"
#include "Servo.h"

/**
 * @brief X/Y方向舵机
 */
struct Servo servoX;
struct Servo servoY;

float targetDegX;
float targetDegY;

/**
 * @brief 初始化滚球控制系统
 */
void rolling_ball_init()
{
    servoX.ccr = &TIM1->CCR1;
    servoX.maxDeg = 180;
    servoX.middlePW = 1500;
    servoX.minPW = 500;
    servoX.maxPW = 2500;
    servo_set_middle(&servoX);

    servoY.ccr = &TIM1->CCR2;
    servoY.maxDeg = 180;
    servoY.middlePW = 1500;
    servoY.minPW = 500;
    servoY.maxPW = 2500;
    servo_set_middle(&servoY);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    //等待舵机复位
    HAL_Delay(2000);
}

/**
 * @brief 更新舵机角度
 */
void update_servo()
{
    servo_set_degree(&servoX, targetDegX++);
    HAL_Delay(1000);
    if (targetDegX > 180)
        targetDegX = 0;
}