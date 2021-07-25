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

#define LOG_TAG "RollingBall"
#include "elog.h"

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
 * @brief 用于控制的键盘
 */
struct Keyboard keyboard = {
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

    rowPins[0].port = KRow_1_GPIO_Port;
    rowPins[0].pin = KRow_1_Pin;
    rowPins[1].port = KRow_2_GPIO_Port;
    rowPins[1].pin = KRow_2_Pin;
    rowPins[2].port = KRow_3_GPIO_Port;
    rowPins[2].pin = KRow_3_Pin;
    rowPins[3].port = KRow_4_GPIO_Port;
    rowPins[3].pin = KRow_4_Pin;

    colPins[0].port = KCol_1_GPIO_Port;
    colPins[0].pin = KCol_1_Pin;
    colPins[1].port = KCol_2_GPIO_Port;
    colPins[1].pin = KCol_2_Pin;
    colPins[2].port = KCol_3_GPIO_Port;
    colPins[2].pin = KCol_3_Pin;
    colPins[3].port = KCol_4_GPIO_Port;
    colPins[3].pin = KCol_4_Pin;

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
void keyHandler(KeyBoard_t keyboard, Key_t key, KeyState_t state, uint8_t code)
{
    if (code == 1)
        servo_set_degree(&servoX, servoX.curDeg + 5);
}

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

    log_i("Initialing Keyboard!");
    keyboard_init();

    log_i("Initialing Servo!");
    servoX.ccr = &TIM1->CCR1;
    servoX.maxDeg = 180;
    servoX.middlePW = 1500;
    servoX.minPW = 500;
    servoX.maxPW = 2500;
    servo_set_degree(&servoX, 90);

    servoY.ccr = &TIM1->CCR2;
    servoY.maxDeg = 180;
    servoY.middlePW = 1500;
    servoY.minPW = 500;
    servoY.maxPW = 2500;
    servo_set_degree(&servoY, 90);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    //等待舵机复位
    HAL_Delay(2000);
    log_i("Servo back to middle!");

    log_i("UART[1]:%d", huart1.Instance);
    log_i("UART[2]:%d", huart2.Instance);

    //接收OpenMV关于小球位置的信息
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&ball_cur, sizeof(Ball_t));
    log_i("Initialize completed!");
}

/**
 * @brief 处理来自OpenMV信息
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    log_d("[OpenMV]VelX:%f, VelY:%f, PosX:%f, PosY:%f", ball_cur.VelX, ball_cur.VelY, ball_cur.PosX, ball_cur.PosY);
    //继续接收OpenMV关于小球位置的信息
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&ball_cur, sizeof(Ball_t));
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    log_e("UART:%d Error:%d on transimition!", huart->Instance, huart->ErrorCode);
    __HAL_UART_CLEAR_FLAG(huart, huart->ErrorCode);
    if (huart->Instance == huart2.Instance)
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&ball_cur, sizeof(Ball_t));
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