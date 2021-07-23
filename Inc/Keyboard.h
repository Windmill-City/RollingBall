#include <stdlib.h>
#include "Base.h"
#include "Math.h"

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
    KeyState_t state;

    /**
     * @brief 按键状态改变持续的时间
     */
    uint8_t changeLastTime;

    /**
     * @brief 按键下一次产生 KeyRepeat 冷却时间
     */
    uint8_t repeatCoolDown;
} Key_t, *pKey;

/**
 * @brief 行列键盘
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
    Pin_t *rowPin;
    /**
     * @brief 键盘列 Pin
     * 
     * 一维数组[col]
     */
    Pin_t *colPin;

    /**
     * @brief 迟滞时间 - 毫秒ms
     * 
     * 至少按下/弹起按键多长时间, 按键状态才改变
     */
    uint8_t slugTime;

    /**
     * @brief 按键转为 KeyRepeat 状态最小时间 - 毫秒ms
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

    /**
     * @brief 上一次扫描的时间
     */
    uint32_t lastScanTick;
} KeyBoard_t, pKeyBoard;

typedef void (*KeyboardCallback)(KeyBoard_t keyboard, Key_t key, KeyState_t state);

void scan_keyboard(pKeyBoard keyboard, KeyboardCallback handler)
{
    uint32_t curTick = HAL_GetTick();
    uint8_t deltaT = curTick - keyboard.lastScanTick;

    /**
     * @brief 依次给每行通上高电平, 并读取列线电平信息
     * 若列线是 高电平 则代表 对应行列 的按键被 按下
     */
    for (size_t row = 0; row < keyboard.row; row++)
    {
        HAL_GPIO_WritePin(keyboard.rowPin->port, keyboard.rowPin->pin, GPIO_PIN_SET);
        for (size_t col = 0; col < keyboard.col; col++)
        {
            _Bool isUp = HAL_GPIO_ReadPin(keyboard.colPin->port, keyboard.colPin->pin) == GPIO_PIN_RESET;
            Key_t key = keyboard.scan_data[row][col];

            switch (key.state)
            {
            case KeyDown:
            case KeyRepeat:
                key.repeatCoolDown -= min(key.repeatCoolDown, deltaT);
                if (key.repeatCoolDown == 0)
                {
                    key.state = KeyRepeat;
                    handler(keyboard, key, key.state);
                    key.repeatCoolDown = keyboard.repeatTime;
                }
            case KeyUp:
                //异或, 按键状态相同为 0, 相异为 1
                if (isUp ^ key.state == KeyUp)
                {
                    key.changeLastTime += deltaT;
                    if (key.changeLastTime > keyboard.slugTime)
                    {
                        key.changeLastTime = 0;
                        key.repeatCoolDown = keyboard.repeatTime;
                        key.state = key.state == KeyUp ? KeyDown : KeyUp;
                        handler(keyboard, key, key.state);
                    }
                }
                else
                {
                    key.changeLastTime -= min(key.changeLastTime, deltaT);
                }
            }
        }
        HAL_GPIO_WritePin(keyboard.rowPin->port, keyboard.rowPin->pin, GPIO_PIN_RESET);
    }
    keyboard.lastScanTick = curTick;
}
