#include <stdint.h>
#include "Math.h"

#define LOG_TAG "Servo"
#include "elog.h"

/**
 * @brief PWM 脉宽 - 单位 us
 * 需要定时器时钟频率为 1MHz - 即 1us
 */
typedef uint16_t PulseWidth;
typedef float Degree;
typedef volatile uint32_t CCRx;

/**
 * @brief 舵机
 */
typedef struct Servo
{
    /**
     * @brief 定时器比较值
     */
    CCRx *ccr;
    /**
     * @brief 当前角度
     */
    Degree curDeg;
    /**
     * @brief 当前脉宽
     */
    PulseWidth curPW;
    /**
     * @brief 系统最小角度
     */
    Degree minDeg;
    /**
     * @brief 系统最大角度
     */
    Degree maxDeg;
    /**
     * @brief 系统零点角度
     */
    Degree zeroDeg;
    /**
     * @brief 设计最小角度脉宽
     */
    PulseWidth designMinPW;
    /**
     * @brief 设计最大角度脉宽
     */
    PulseWidth designMaxPW;
    /**
     * @brief 设计最大角度
     */
    Degree designMaxDeg;
} Servo_t, *pServo;

/**
 * @brief 设置舵机角度
 * 
 * @param servo 舵机
 * @param newDeg 角度 - 角度制
 */
void servo_set_degree(pServo servo, Degree newDeg)
{
    newDeg = clamp(servo->minDeg, newDeg, servo->maxDeg);
    log_v("[%d]Set degree:%f", servo, newDeg);
    servo->curDeg = newDeg;
    servo->curPW = newDeg / servo->designMaxDeg * (servo->designMaxPW - servo->designMinPW) + servo->designMinPW;
    *servo->ccr = servo->curPW;
}

/**
 * @brief 设置舵机相对中立位置角度偏移
 * 
 * @param servo 舵机
 * @param newDegOffset 角度 - 角度制
 */
void servo_set_degree_offset(pServo servo, Degree newDegOffset)
{
    servo_set_degree(servo, servo->zeroDeg + newDegOffset);
}

/**
 * @brief 设置舵机脉宽
 * 
 * @param servo 舵机
 * @param newPW 脉宽
 */
void servo_set_pw(pServo servo, PulseWidth newPW)
{
    newPW = clamp(servo->designMinPW, newPW, servo->designMaxPW);
    log_v("[%d]Set Pulse Width:%f", servo, newPW);
    servo->curDeg = (float)(newPW - servo->designMinPW) / (servo->designMaxPW - servo->designMinPW) * servo->designMaxDeg;
    servo->curPW = newPW;
    *servo->ccr = servo->curPW;
}

/**
 * @brief 舵机回到中立位置
 * 
 * @param servo 舵机
 */
void servo_set_zero(pServo servo)
{
    servo_set_degree(servo, servo->zeroDeg);
}