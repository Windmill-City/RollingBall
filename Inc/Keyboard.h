#pragma once
#include <stdlib.h>
#include "Base.h"
#include "MathUtil.h"

#define LOG_TAG "Keyboard"
#include "elog.h"

/**
 * @brief 按键状态
 */
typedef enum KeyState
{
    KeyUp = 0,
    KeyDown,
    KeyRepeat,
} KeyState_t;

/**
 * @brief 键盘按键状态
 */
typedef struct Key
{
    /**
     * @brief 按键码
     * 
     * 应唯一, 用于识别按键
     */
    uint8_t code;
    KeyState_t state;

    /**
     * @brief 按键状态改变冷却时间
     */
    uint8_t changeCooldown;

    /**
     * @brief 按键下一次产生 KeyRepeat 冷却时间
     */
    uint8_t repeatCooldown;
} Key_t, *pKey;

/**
 * @brief 行列键盘
 * 
 * 行线 需要为 GPIO_OUTPUT
 * 列线 需要为 GPIO_INPUT
 */
typedef struct Keyboard
{
    /**
     * @brief 键盘行数
     */
    uint8_t row;
    /**
     * @brief 键盘列数
     */
    uint8_t col;

    /**
     * @brief 键盘行 Pin
     * 
     * 一维数组[row]
     */
    Pin_t *rowPins;
    /**
     * @brief 键盘列 Pin
     * 
     * 一维数组[col]
     */
    Pin_t *colPins;

    /**
     * @brief 迟滞时间
     * 
     * 按下/弹起按键后, 经历多长时间按键状态才能再改变
     */
    uint8_t slugTime;

    /**
     * @brief 按键转为 KeyRepeat 状态最小时间
     * 
     * 至少按下按键多长时间, 按键才转为 KeyRepeat 状态
     */
    uint8_t repeatTime;

    /**
     * @brief 键盘扫描结果
     * 
     * 二维数组[row][col]
     */
    Key_t **scan_data;
} Keyboard_t, *pKeyboard;

/**
 * @brief 键盘对象析构器
 */
void keyboard_dtor(pKeyboard keyboard)
{
    free(keyboard->rowPins);
    free(keyboard->colPins);
    if (keyboard->scan_data != NULL)
        for (size_t row = 0; row < keyboard->row; row++)
        {
            free(keyboard->scan_data[row]);
        }

    free(keyboard->scan_data);
}

/**
 * @brief 键盘对象构造器
 * 
 * @param keyboard 要初始化的键盘对象
 * @return 0 成功
 */
int keyboard_ctor(pKeyboard keyboard)
{
    if ((keyboard->rowPins = calloc(keyboard->row, sizeof(Pin_t))) == NULL)
        goto Fail;
    if ((keyboard->colPins = calloc(keyboard->col, sizeof(Pin_t))) == NULL)
        goto Fail;
    if ((keyboard->scan_data = calloc(keyboard->row, sizeof(pKey))) == NULL)
        goto Fail;
    for (size_t row = 0; row < keyboard->row; row++)
    {
        if ((keyboard->scan_data[row] = calloc(keyboard->col, sizeof(Key_t))) == NULL)
            goto Fail;
    }

    return 0;
Fail:
    keyboard_dtor(keyboard);
    return 1;
}

/**
 * @brief 按键处理回调
 * 
 * @param keyboard 键盘 
 * @param key 按键
 * @param state 按键状态
 * @param code 按键码
 */
typedef void (*KeyboardCallback)(Keyboard_t keyboard, Key_t key, KeyState_t state, uint8_t code);

void scan_keyboard(pKeyboard keyboard, KeyboardCallback handler)
{
    log_v("Keyboard scan start!");

    /**
     * @brief 依次给每行通上高电平, 并读取列线电平信息
     * 若列线是 高电平 则代表 对应行列 的按键被 按下
     */
    for (size_t row = 0; row < keyboard->row; row++)
    {
        HAL_GPIO_WritePin(keyboard->rowPins[row].port, keyboard->rowPins[row].pin, GPIO_PIN_SET);
        log_v("Scanning Row:%d, Port:%d, Pin:%d", row, keyboard->rowPins[row].port, keyboard->rowPins[row].pin);
        for (size_t col = 0; col < keyboard->col; col++)
        {
            log_v("Scanning Row:%d, Col%d", row, col);
            bool isUp = !HAL_GPIO_ReadPin(keyboard->colPins[col].port, keyboard->colPins[col].pin);
            pKey key = &keyboard->scan_data[row][col];
            log_v("Key[%d]isUp:%d, Port:%d Pin:%d", key->code, isUp, keyboard->colPins[col].port, keyboard->colPins[col].pin);

            switch (key->state)
            {
            case KeyDown:
            case KeyRepeat:
                key->repeatCooldown -= min(key->repeatCooldown, 1);
                log_v("Key[%d]Repeat Cooldown:%d", key->code, key->repeatCooldown);
                if (key->repeatCooldown == 0)
                {
                    key->state = KeyRepeat;
                    log_d("Key[%d] Repeated!", key->code);
                    handler(*keyboard, *key, key->state, key->code);
                    key->repeatCooldown = keyboard->repeatTime;
                }
            case KeyUp:
                key->changeCooldown -= min(key->changeCooldown, 1);
                //异或, 按键状态相同为 0, 相异为 1
                if (isUp ^ (key->state == KeyUp))
                {
                    log_v("Key[%d]Changing state, Old State:%d, Change Cooldown:%d", key->code, key->state, key->changeCooldown);
                    if (key->changeCooldown == 0)
                    {
                        key->changeCooldown = keyboard->slugTime;
                        key->repeatCooldown = keyboard->repeatTime;
                        key->state = key->state == KeyUp ? KeyDown : KeyUp;
                        log_d("Key[%d]Changed state, New State:%d", key->code, key->state);
                        handler(*keyboard, *key, key->state, key->code);
                    }
                }
            }
        }
        HAL_GPIO_WritePin(keyboard->rowPins[row].port, keyboard->rowPins[row].pin, GPIO_PIN_RESET);
    }
}
