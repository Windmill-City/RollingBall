#pragma once

#include "adc.h"
#include "stm32f4xx_hal_adc.h"

#define LOG_TAG "ADC"
#include "elog.h"

uint32_t ADC_ConvertedValue;

// 用于保存转换计算后的电压值
float ADC_Vol;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    ADC_ConvertedValue = HAL_ADC_GetValue(hadc);
    ADC_Vol = (float)ADC_ConvertedValue / 4096 * 3.3;
    log_i("Vol:%f", ADC_Vol);
    //HAL_ADC_Start_IT(&hadc1);
}
