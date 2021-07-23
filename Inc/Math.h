/**
 * @file Math.h
 * @author Windmill_City
 * @brief 数学库
 * @version 0.1
 * @date 2021-07-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
#define clamp(vl, v, vh) max(vl, min(v, vh))