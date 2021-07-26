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
     * @brief 最小角度
     */
    Degree minDeg;
    /**
     * @brief 最大角度
     */
    Degree maxDeg;
    /**
     * @brief 中立位置角度
     */
    Degree zeroDeg;
    /**
     * @brief 当前脉宽
     */
    PulseWidth curPW;
    /**
     * @brief 中立位置脉宽
     */
    PulseWidth zeroPW;
    /**
     * @brief 最小角度脉宽
     */
    PulseWidth minPW;
    /**
     * @brief 最大角度脉宽
     */
    PulseWidth maxPW;
} Servo_t, *pServo;

/**
 * @brief 设置舵机角度
 * 
 * @param servo 舵机
 * @param newDeg 角度 - 角度制
 */
void servo_set_degree(pServo servo, Degree newDeg)
{
    log_v("[%d]Set degree:%f", servo, newDeg);
    newDeg = clamp(servo->minDeg, newDeg, servo->maxDeg);
    servo->curDeg = newDeg;
    servo->curPW = newDeg / servo->maxDeg * (servo->maxPW - servo->minPW) + servo->minPW;
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
    log_v("[%d]Set Pulse Width:%f", servo, newPW);
    newPW = clamp(servo->minPW, newPW, servo->maxPW);
    servo->curDeg = (float)(newPW - servo->minPW) / (servo->maxPW - servo->minPW) * servo->maxDeg;
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
    servo->curDeg = servo->zeroDeg;
    servo->curPW = servo->zeroPW;
    *servo->ccr = servo->curPW;
}