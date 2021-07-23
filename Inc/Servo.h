#include <stdio.h>

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
#define clamp(vl, v, vh) max(vl, min(v, vh))

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
struct Servo
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
     * @brief 最大角度
     */
    Degree maxDeg;
    /**
     * @brief 当前脉宽
     */
    PulseWidth curPW;
    /**
     * @brief 中立位置脉宽
     */
    PulseWidth middlePW;
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
void servo_set_degree(struct Servo *servo, Degree newDeg)
{
    newDeg = clamp(0, newDeg, servo->maxDeg);
    servo->curDeg = newDeg;
    servo->curPW = newDeg / servo->maxDeg * (servo->maxPW - servo->minPW) + servo->minPW;
    *servo->ccr = servo->curPW;
}

/**
 * @brief 设置舵机脉宽
 * 
 * @param servo 舵机
 * @param newPW 脉宽
 */
void servo_ser_pw(struct Servo *servo, PulseWidth newPW)
{
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
void servo_set_middle(struct Servo *servo)
{
    //中立位置应为最大角度一半
    servo->curDeg = servo->maxDeg / 2;
    servo->curPW = servo->middlePW;
    *servo->ccr = servo->curPW;
}