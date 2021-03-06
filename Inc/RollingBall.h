#pragma once
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
#include "Keyboard.h"
#include "LCD.h"
#include "OLED.h"
#include "LightSensor.h"
#include "MPU6050.h"

#define LOG_TAG "RollingBall"
#include "elog.h"

#pragma region RollingBall Static Data

/**
 * @brief 小球状态
 */
typedef struct Ball
{
    float VelX, VelY, PosX, PosY;
} Ball_t, *pBall;

/**
 * @brief X/Y方向舵机
 */
struct Servo servoX;
struct Servo servoY;

/**
 * @brief 位置环
 * 位置环 -> 速度环 -> 舵机角度
 */
PIDProvider_t pidPosX;
PIDProvider_t pidPosY;

/**
 * @brief 速度环
 * 位置环 -> 速度环 -> 舵机角度
 */
PIDProvider_t pidVelX;
PIDProvider_t pidVelY;

/**
 * @brief 小球目标位置/速度
 * 
 * 目标位置手动指定
 * 目标速度由速度环更新
 */
Ball_t ball_target;

/**
 * @brief 小球当前位置/速度
 * 
 * 当前位置/速度在串口中断获取, 数据来自OpenMV
 */
Ball_t ball_cur;

#pragma endregion

#pragma region Point
typedef struct Point
{
    float X;
    float Y;
} Point_t, *pPoint;

Point_t Pos[9];

/**
 * @brief 初始化点坐标
 */
void pos_init()
{
    Pos[0].X = -20;
    Pos[0].Y = 20;

    Pos[1].X = 0;
    Pos[1].Y = 20;

    Pos[2].X = 20;
    Pos[2].Y = 20;

    Pos[3].X = -20;
    Pos[3].Y = 0;

    Pos[4].X = 0;
    Pos[4].Y = 0;

    Pos[5].X = 20;
    Pos[5].Y = 0;

    Pos[6].X = -20;
    Pos[6].Y = -20;

    Pos[7].X = 0;
    Pos[7].Y = -20;

    Pos[8].X = 20;
    Pos[8].Y = -20;
}

#pragma endregion

#pragma region Keyboard

/**
 * @brief 用于控制的键盘
 */
Keyboard_t keyboard = {
    .row = 4,
    .col = 4,
    .slugTime = 5,
    .repeatTime = 60};

/**
 * @brief 初始化键盘 IO 以及 按键码 信息
 */
void keyboard_init()
{
    keyboard_ctor(&keyboard);

    pPin rowPins = keyboard.rowPins;
    pPin colPins = keyboard.colPins;

    // rowPins[0].port = KRow_1_GPIO_Port;
    // rowPins[0].pin = KRow_1_Pin;
    // rowPins[1].port = KRow_2_GPIO_Port;
    // rowPins[1].pin = KRow_2_Pin;
    // rowPins[2].port = KRow_3_GPIO_Port;
    // rowPins[2].pin = KRow_3_Pin;
    // rowPins[3].port = KRow_4_GPIO_Port;
    // rowPins[3].pin = KRow_4_Pin;

    // colPins[0].port = KCol_1_GPIO_Port;
    // colPins[0].pin = KCol_1_Pin;
    // colPins[1].port = KCol_2_GPIO_Port;
    // colPins[1].pin = KCol_2_Pin;
    // colPins[2].port = KCol_3_GPIO_Port;
    // colPins[2].pin = KCol_3_Pin;
    // colPins[3].port = KCol_4_GPIO_Port;
    // colPins[3].pin = KCol_4_Pin;

    //初始化数字区
    uint8_t code = 1;
    for (size_t row = 0; row < 3; row++)
    {
        for (size_t col = 0; col < 3; col++)
        {
            keyboard.scan_data[row][col].code = code++;
        }
    }
    //0
    keyboard.scan_data[3][1].code = 0;
    //初始化字母区
    code = 'a';
    for (size_t row = 0; row < 4; row++)
    {
        keyboard.scan_data[row][3].code = code++;
    }
    //*号
    keyboard.scan_data[3][0].code = '*';
    //#号
    keyboard.scan_data[3][2].code = '#';
}

/**
 * @brief 按键处理程序
 * 
 * @param keyboard 键盘 
 * @param key 按键
 * @param state 按键状态
 * @param code 按键码
 */
void keyHandler(Keyboard_t keyboard, Key_t key, KeyState_t state, uint8_t code)
{
    switch (code)
    {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        ball_target.PosX = Pos[code - 1].X;
        ball_target.PosY = Pos[code - 1].Y;
        log_i("Ball Target set to:%f, %f", ball_target.PosX, ball_target.PosY);
        break;
    case 'a':
        break;
    case 'b':
        break;
    case 'c':
        break;
    case 'd':
        break;
    case '*':
        break;
    case '#':
        break;
    }
}

#pragma endregion

#pragma region OpenMV

#define OpenMV_Head 0xF4F8F6F3

/**
 * @brief OpenMV数据
 */
typedef struct OpenMV
{
    uint32_t head;
    Ball_t data;
} OpenMV_t, *pOpenMV;

OpenMV_t openMV_buf;

/**
 * @brief 处理来自OpenMV信息
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (openMV_buf.head == OpenMV_Head)
    {
        ball_cur = openMV_buf.data;
        log_v("[OpenMV]VelX:%f, VelY:%f, PosX:%f, PosY:%f", ball_cur.VelX, ball_cur.VelY, ball_cur.PosX, ball_cur.PosY);
    }
    else
    {
        log_e("[OpenMV]Error!");
    }
    //继续接收OpenMV关于小球位置的信息
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&openMV_buf, sizeof(OpenMV_t));
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    log_v("UART:%d Error:%d on transimition!", huart->Instance, huart->ErrorCode);
    __HAL_UART_CLEAR_FLAG(huart, huart->ErrorCode);
    if (huart->Instance == huart2.Instance)
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&openMV_buf, sizeof(OpenMV_t));
}

#pragma endregion

#pragma region LCD

LCD_t lcd = {
    .fsmc = (pFSMC)(0x6C000000 | 0X0000007E),
    .width = 240,
    .height = 320,
    .display_direction = Vertical,
    .screen_direction = Vertical,
    .backLightOn = true};

#pragma endregion

#pragma region MPU
typedef struct Attribute
{
    /**
     * @brief 欧拉角
     */
    struct Euler euler;
    /**
     * @brief 角速度
     */
    float omegaTheta;
    float omegaPhi;
    /**
     * @brief 角加速度
     */
    float accOmegaTheta;
    float accOmegaPhi;
} Attribute_t, *pAttribute;

Attribute_t fetchAttr()
{
    long data[9];
    int8_t accuracy;
    unsigned long timestamp;

    struct Attribute attr;

    //欧拉角
    inv_get_sensor_type_euler(data, &accuracy, (inv_time_t *)&timestamp);

    attr.euler.Pitch = ToFloat(data[0], 16);
    attr.euler.Roll = ToFloat(data[1], 16);
    attr.euler.Yaw = ToFloat(data[2], 16);

    log_v("Euler: Pitch:%f Roll:%f Yaw:%f", attr.euler.Pitch, attr.euler.Roll, attr.euler.Yaw);

    //角速度
    inv_get_sensor_type_gyro(data, &accuracy, (inv_time_t *)&timestamp);

    attr.omegaTheta = ToFloat(data[0], 16);
    attr.omegaPhi = ToFloat(data[1], 16);

    log_v("Omega: %f %f %f", attr.omegaTheta, attr.omegaPhi, ToFloat(data[2], 16));
    //角加速度
    inv_get_sensor_type_accel(data, &accuracy, (inv_time_t *)&timestamp);

    attr.accOmegaTheta = ToFloat(data[0], 16);
    attr.accOmegaPhi = ToFloat(data[1], 16);

    log_v("Acc: %f %f %f", attr.accOmegaTheta, attr.accOmegaPhi, ToFloat(data[2], 16));

    return attr;
}

#pragma endregion

#pragma region Update Handler

typedef void (*UpdateHandler)();

UpdateHandler updateHandler;

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

/**
 * @brief 更新循环, 一直追踪目标点
 */
void update_trace_point()
{
    update_servo();
    scan_keyboard(&keyboard, keyHandler);
    HAL_Delay(5);
}

float degX = 0;
float degY = 0;

void update_debug_degree()
{
    servo_set_degree_offset(&servoX, degX);
    servo_set_degree_offset(&servoY, degY);
    HAL_Delay(20);
}

#include <stdio.h>
#include <string.h>
char str_buf[512];
void update_mpu()
{
    bool newData = false;
    while (read_fifo(&newData))
    {
    }
    if (newData)
    {
        Attribute_t attr = fetchAttr();
        HAL_Delay(20);
    }
}

void update_empty() {}

#pragma endregion

/**
 * @brief 初始化滚球控制系统
 */
void rolling_ball_init()
{
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL | ELOG_FMT_TAG);
    /* start EasyLogger */
    elog_start();

    //log_i("Initialing Keyboard!");
    //keyboard_init();

    log_i("Initialing LCD");
    lcd_init(&lcd, LCD_BL_GPIO_Port, LCD_BL_Pin);
    lcd_clear(&lcd, LIGHTBLUE);

    log_i("Initialing OLED");
    OLED_Init();
    //OLED_Test();
    OLED_Clear();

    // log_i("Initialing Pos");
    // pos_init();

    // log_i("Initialing Servo!");
    // servoX.ccr = &TIM1->CCR1;
    // servoX.minDeg = 10;
    // servoX.maxDeg = 170;
    // servoX.zeroDeg = 120;
    // servoX.designMinPW = 500;
    // servoX.designMaxPW = 2500;
    // servoX.designMaxDeg = 180;
    // servo_set_zero(&servoX);

    // servoY.ccr = &TIM1->CCR2;
    // servoY.minDeg = 10;
    // servoY.maxDeg = 170;
    // servoY.zeroDeg = 120;
    // servoY.designMinPW = 500;
    // servoY.designMaxPW = 2500;
    // servoY.designMaxDeg = 180;
    // servo_set_zero(&servoY);

    // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    // //等待舵机复位
    // HAL_Delay(2000);
    // log_i("Servo back to zero!");

    // log_i("UART[1]:%d", huart1.Instance);
    // log_i("UART[2]:%d", huart2.Instance);

    //注册更新循环
    updateHandler = update_mpu;

    // log_i("Start OpemMV Recv");
    // //接收OpenMV关于小球位置的信息
    // HAL_UART_Receive_IT(&huart2, (uint8_t *)&openMV_buf, sizeof(OpenMV_t));

    // log_i("Start ADC Recv");
    // HAL_ADC_Start_IT(&hadc1);

    log_i("Initializing MPU6050");
    mpu6050_init(20, true);

    log_i("Initialize completed!");
}