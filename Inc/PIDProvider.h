#pragma once
#include "MathUtil.h"

#define LOG_TAG "PIDProvider"
#include "elog.h"

/**
 * @brief PID 参数
 */
typedef struct PIDParam
{
    float P;
    float I;
    float D;
    /**
     * @brief 积分项上下限
     */
    float maxIntegral;
    /**
     * @brief 最大积分误差
     * 用于积分分离
     */
    float maxIntegralErr;
} PIDParam_t, *pPIDParam;
typedef struct PIDProvider
{
    struct PIDParam param;
    /**
     * @brief 上一次误差值
     */
    float err;
    /**
     * @brief 误差积分值
     */
    float integral;
    /**
     * @brief 误差微分值
     */
    float diff;
} PIDProvider_t, *pPIDProvider;

float pid_push_new_err(pPIDProvider provider, float new_err)
{
    //积分分离, 大于最大积分误差则清零积分
    if (abs(new_err) <= provider->param.maxIntegralErr)
    {
        provider->integral += new_err;
        //积分限幅
        provider->integral = clamp(-provider->param.maxIntegral, provider->integral, provider->param.maxIntegral);
    }
    else
        provider->integral = 0;

    provider->diff = new_err - provider->err;
    provider->err = new_err;
    log_v("Err:%f, Integral:%f, Diff:%f", provider->err, provider->integral, provider->diff);
    return provider->param.P * new_err + provider->param.I * provider->integral + provider->param.D * provider->diff;
}

void pid_reset_all(pPIDProvider provider)
{
    provider->integral = 0;
    provider->diff = 0;
    provider->err = 0;
    log_d("pid reset all");
}
