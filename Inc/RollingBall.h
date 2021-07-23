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
#include "usart.h"
#include "Servo.h"
#include "PIDProvider.h"

/**
 * @brief 小球状态
 */
typedef struct Ball
{
    float VelX, VelY, PosX, PosY;
}Ball_t, *pBall;

/**
 * @brief X/Y方向舵机
 */
struct Servo servoX;
struct Servo servoY;

/**
 * @brief 位置环
 * 位置环 -> 速度环 -> 舵机角度
 */
struct PIDProvider pidPosX;
struct PIDProvider pidPosY;

/**
 * @brief 速度环
 * 位置环 -> 速度环 -> 舵机角度
 */
struct PIDProvider pidVelX;
struct PIDProvider pidVelY;

/**
 * @brief 小球目标位置/速度
 * 
 * 目标位置手动指定
 * 目标速度由速度环更新
 */
struct Ball ball_target;

/**
 * @brief 小球当前位置/速度
 * 
 * 当前位置/速度在串口中断获取, 数据来自OpenMV
 */
struct Ball ball_cur;

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

    //接收OpenMV关于小球位置的信息
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&ball_cur, sizeof(struct Ball));
}

/**
 * @brief 处理来自OpenMV信息
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //继续接收OpenMV关于小球位置的信息
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&ball_cur, sizeof(struct Ball));
}

/**
 * @brief 更新舵机角度
 */
void update_servo()
{
    //X方向
    float errPosX = ball_target.PosX - ball_cur.PosX;
    ball_target.VelX = pid_push_new_err(&pidPosX, errPosX);
    float errVelX = ball_target.VelX - ball_cur.VelX;
    servo_set_degree_offset(&servoX, pid_push_new_err(&pidVelX, errVelX));

    //Y方向
    float errPosY = ball_target.PosY - ball_cur.PosY;
    ball_target.VelY = pid_push_new_err(&pidPosY, errPosY);
    float errVelY = ball_target.VelY - ball_cur.VelY;
    servo_set_degree_offset(&servoY, pid_push_new_err(&pidVelY, errVelY));
}