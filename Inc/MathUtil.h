#pragma once

#include <math.h>

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
/**
 * @brief 区间限定, 使数值在指定区间内 [vl, vh]
 * 
 * @param vl 低位区间
 * @param v 需要限定区间的值
 * @param vh 高位区间
 * @return 区间限定后的值
 */
#define clamp(vl, v, vh) max(vl, min(v, vh))

/**
 * @brief 整数除法, 向上取整
 * 
 * @param divided 被除数
 * @param divisor 除数
 * @return 向上取整的结果
 */
#define IntegerCeilingDivision(dividend, divisor) ((dividend + (divisor - 1)) / divisor)