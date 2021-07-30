#pragma once
#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "Base.h"
#include "Math.h"
#include "Font.h"

/**
 * @brief 颜色
 */
typedef volatile uint16_t Color;

#define WHITE ((Color)0xFFFF)
#define BLACK ((Color)0x0000)
#define BLUE ((Color)0x001F)
#define BRED ((Color)0xF81F)
#define GRED ((Color)0xFFE0)
#define GBLUE ((Color)0x07FF)
#define RED ((Color)0xF800)
#define MAGENTA ((Color)0xF81F)
#define GREEN ((Color)0x07E0)
#define CYAN ((Color)0x7FFF)
#define YELLOW ((Color)0xFFE0)
#define BROWN ((Color)0XBC40) //棕色
#define BRRED ((Color)0XFC07) //棕红色
#define GRAY ((Color)0X8430)  //灰色

#define DARKBLUE ((Color)0X01CF)   //深蓝色
#define LIGHTBLUE ((Color)0X7D7C)  //浅蓝色
#define GRAYBLUE ((Color)0X5458)   //灰蓝色
#define LIGHTGREEN ((Color)0X841F) //浅绿色
#define LIGHTGRAY ((Color)0XEF5B)  //浅灰色(PANEL)
#define LGRAY ((Color)0XC618)      //浅灰色(PANEL),窗体背景色
#define LGRAYBLUE ((Color)0XA651)  //浅灰蓝色(中间层颜色)
#define LBBLUE ((Color)0X2B12)     //浅棕蓝色(选择条目的反色)

/**
 * @brief 扫描起始位置
 * 
 * Left_Up - 从左向右, 从上到下
 */
typedef enum ScanDirection
{
    Left_Up,
    Left_Down,
    Right_Up,
    Right_Down,
    Up_Left,
    Up_Right,
    Down_Left,
    Down_Right
} ScanDirection_t;

/**
 * @brief 方向
 */
typedef enum Direction
{
    Vertical,
    Horizontal

} Direction_t;

typedef struct FSMC
{
    volatile uint16_t cmd;
    volatile uint16_t param;
} FSMC_t, *pFSMC;

typedef struct LCD
{
    pFSMC fsmc;
    Pin_t backLight;
    /**
     * @brief 屏幕宽度 - 短边为宽
     */
    uint16_t width;
    /**
     * @brief 屏幕高度 - 长边为高
     */
    uint16_t height;
    /**
     * @brief 显示宽度
     */
    uint16_t display_width;
    /**
     * @brief 显示高度
     */
    uint16_t display_height;
    ScanDirection_t scan_direction;
    Direction_t screen_direction;
    Direction_t display_direction;
    bool backLightOn;
} LCD_t, *pLCD;

/**
 * @brief 设置内存光标位置
 * 
 * @param sx 起始 X坐标
 * @param sy 起始 Y坐标
 */
void lcd_set_cursor(pLCD lcd, uint16_t sx, uint16_t sy)
{
    //Column Address Set
    lcd->fsmc->cmd = 0x2A;
    lcd->fsmc->param = sx >> 8;
    lcd->fsmc->param = sx & 0xFF;

    //Page Address Set
    lcd->fsmc->cmd = 0x2B;
    lcd->fsmc->param = sy >> 8;
    lcd->fsmc->param = sy & 0xFF;
}

/**
 * @brief 设置窗口
 * 
 * @param sx 起始 X坐标
 * @param sy 起始 Y坐标
 * @param width 窗口宽度 - 返回合法的宽度
 * @param height 窗口高度 - 返回合法高度
 * @return 返回窗口数据区
 */
Color *lcd_set_window(pLCD lcd, uint16_t sx, uint16_t sy, uint16_t *width, uint16_t *height)
{
    //因为包含起始点, 所以减去重复的 1 点
    uint16_t maxWidth = min(sx + *width, lcd->display_width) - 1;
    uint16_t maxHeight = min(sy + *height, lcd->display_height) - 1;

    //Column Address Set
    lcd->fsmc->cmd = 0x2A;
    lcd->fsmc->param = sx >> 8;
    lcd->fsmc->param = sx & 0xFF;
    lcd->fsmc->param = maxWidth >> 8;
    lcd->fsmc->param = maxWidth & 0xFF;

    //Page Address Set
    lcd->fsmc->cmd = 0x2B;
    lcd->fsmc->param = sy >> 8;
    lcd->fsmc->param = sy & 0xFF;
    lcd->fsmc->param = maxHeight >> 8;
    lcd->fsmc->param = maxHeight & 0xFF;

    //包含起始点, 加上多减的 1 点
    *width = maxWidth - sx + 1;
    *height = maxHeight - sy + 1;

    //Memory Write
    lcd->fsmc->cmd = 0x2C;
    return (Color *)&lcd->fsmc->param;
}

/**
 * @brief 根据屏幕方向刷新当前显示范围
 */
void lcd_refresh_display_region(pLCD lcd)
{
    switch (lcd->screen_direction)
    {
    case Vertical:
        lcd->display_width = lcd->width;
        lcd->display_height = lcd->height;
        break;
    case Horizontal:
        lcd->display_width = lcd->height;
        lcd->display_height = lcd->width;
        break;
    }

    lcd_set_window(lcd, 0, 0, &lcd->display_width, &lcd->display_height);
}

/**
 * @brief 设置 LCD 扫描方向
 */
void lcd_set_scan_direction(pLCD lcd, ScanDirection_t scan_direction)
{
    //Memory Access Control
    lcd->fsmc->cmd = 0x36;
    switch (scan_direction)
    {
    case Left_Up:
        lcd->fsmc->param = (0 << 7) | (0 << 6) | (0 << 5) | 0x08;
        break;
    case Left_Down:
        lcd->fsmc->param = (1 << 7) | (0 << 6) | (0 << 5) | 0x08;
        break;
    case Right_Up:
        lcd->fsmc->param = (0 << 7) | (1 << 6) | (0 << 5) | 0x08;
        break;
    case Right_Down:
        lcd->fsmc->param = (1 << 7) | (1 << 6) | (0 << 5) | 0x08;
        break;
    case Up_Left:
        lcd->fsmc->param = (0 << 7) | (0 << 6) | (1 << 5) | 0x08;
        break;
    case Up_Right:
        lcd->fsmc->param = (0 << 7) | (1 << 6) | (1 << 5) | 0x08;
        break;
    case Down_Left:
        lcd->fsmc->param = (1 << 7) | (0 << 6) | (1 << 5) | 0x08;
        break;
    case Down_Right:
        lcd->fsmc->param = (1 << 7) | (1 << 6) | (1 << 5) | 0x08;
        break;
    }
    lcd->scan_direction = scan_direction;
}

/**
 * @brief 设置 LCD 显示方向
 */
void lcd_set_display_direction(pLCD lcd, Direction_t direction)
{
    lcd->display_direction = direction;
    switch (lcd->display_direction)
    {
    case Vertical:
        switch (lcd->screen_direction)
        {
        case Vertical:
            lcd_set_scan_direction(lcd, Left_Up);
            break;
        case Horizontal:
            lcd_set_scan_direction(lcd, Right_Down);
            break;
        }
        break;
    case Horizontal:
        switch (lcd->screen_direction)
        {
        case Vertical:
            lcd_set_scan_direction(lcd, Up_Right);
            break;
        case Horizontal:
            lcd_set_scan_direction(lcd, Down_Left);
            break;
        }
        break;
    }
}

/**
 * @brief 设置屏幕方向
 */
void lcd_set_screen_direction(pLCD lcd, Direction_t direction)
{
    lcd->screen_direction = direction;
    lcd_refresh_display_region(lcd);
    lcd_set_display_direction(lcd, lcd->display_direction);
}

/**
 * @brief 开启/关闭屏幕背光及显示
 * 
 * @param on true 开启, false 关闭
 */
void lcd_set_bl(pLCD lcd, bool on)
{
    lcd->backLightOn = on;
    HAL_GPIO_WritePin(lcd->backLight.port, lcd->backLight.pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    lcd->fsmc->cmd = on ? 0x29 : 0x28;
}
/**
 * @brief 初始化 LCD屏幕
 */
void lcd_init(pLCD lcd, GPIO_TypeDef *bl_port, uint16_t bl_pin)
{
    //Power control B (CFh)
    lcd->fsmc->cmd = 0xCF;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0xC1;
    lcd->fsmc->param = 0x30;

    //Power on sequence control
    lcd->fsmc->cmd = 0xed;
    lcd->fsmc->param = 0x64;
    lcd->fsmc->param = 0x03;
    lcd->fsmc->param = 0x12;
    lcd->fsmc->param = 0x81;

    //Driver timing control A
    lcd->fsmc->cmd = 0xe8;
    lcd->fsmc->param = 0x85;
    lcd->fsmc->param = 0x10;
    lcd->fsmc->param = 0x7A;

    //Power control A
    lcd->fsmc->cmd = 0xcb;
    lcd->fsmc->param = 0x39;
    lcd->fsmc->param = 0x2c;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x34;
    lcd->fsmc->param = 0x02;

    //Pump ratio control
    lcd->fsmc->cmd = 0xf7;
    lcd->fsmc->param = 0x20;

    //Driver timing control B
    lcd->fsmc->cmd = 0xea;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x00;

    //Frame Rate Control (In Normal Mode/Full Colors)
    lcd->fsmc->cmd = 0xb1;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x1b;

    //Display Function Control
    lcd->fsmc->cmd = 0xb6;
    lcd->fsmc->param = 0x0a;
    lcd->fsmc->param = 0xa2;

    //Power Control 1
    lcd->fsmc->cmd = 0xc0;
    lcd->fsmc->param = 0x1B;

    //Power Control 2
    lcd->fsmc->cmd = 0xc1;
    lcd->fsmc->param = 0x01;

    //VCOM Control 1
    lcd->fsmc->cmd = 0xc5;
    lcd->fsmc->param = 0x30;
    lcd->fsmc->param = 0x30;

    //VCOM Control 2
    lcd->fsmc->cmd = 0xc7;
    lcd->fsmc->param = 0xB7;

    //Enable 3G
    lcd->fsmc->cmd = 0xf2;
    lcd->fsmc->param = 0x00;

    //Gamma Set
    lcd->fsmc->cmd = 0x26;
    lcd->fsmc->param = 0x01;

    //Positive Gamma Correction
    lcd->fsmc->cmd = 0xe0;
    lcd->fsmc->param = 0x0f;
    lcd->fsmc->param = 0x2a;
    lcd->fsmc->param = 0x28;
    lcd->fsmc->param = 0x08;
    lcd->fsmc->param = 0x0e;
    lcd->fsmc->param = 0x08;
    lcd->fsmc->param = 0x54;
    lcd->fsmc->param = 0xa9;
    lcd->fsmc->param = 0x43;
    lcd->fsmc->param = 0x0a;
    lcd->fsmc->param = 0x0f;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x00;

    //Negative Gamma Correction
    lcd->fsmc->cmd = 0xe1;
    lcd->fsmc->param = 0x00;
    lcd->fsmc->param = 0x15;
    lcd->fsmc->param = 0x17;
    lcd->fsmc->param = 0x07;
    lcd->fsmc->param = 0x11;
    lcd->fsmc->param = 0x06;
    lcd->fsmc->param = 0x2b;
    lcd->fsmc->param = 0x56;
    lcd->fsmc->param = 0x3c;
    lcd->fsmc->param = 0x05;
    lcd->fsmc->param = 0x10;
    lcd->fsmc->param = 0x0f;
    lcd->fsmc->param = 0x3f;
    lcd->fsmc->param = 0x3f;
    lcd->fsmc->param = 0x0f;

    //Pixel Format Set
    lcd->fsmc->cmd = 0x3a;
    lcd->fsmc->param = 0x55;

    // Sleep Out
    lcd->fsmc->cmd = 0x11;
    HAL_Delay(100);

    lcd_set_screen_direction(lcd, lcd->screen_direction);
    //BackLight
    lcd->backLight.port = bl_port;
    lcd->backLight.pin = bl_pin;
    lcd_set_bl(lcd, lcd->backLightOn);
}

/**
 * @brief 填充矩形区域 - 纯色
 * 
 * @param sx 起始 X坐标
 * @param sy 起始 Y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 * @param color 颜色
 */
void lcd_fill_pure(pLCD lcd, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height, Color color)
{
    Color *toFill = lcd_set_window(lcd, sx, sy, &width, &height);

    lcd->fsmc->cmd = 0x2C;
    for (size_t row = 0; row < height; row++)
    {
        for (size_t col = 0; col < width; col++)
        {
            *toFill = color;
        }
    }
}

/**
 * @brief 清屏
 * 
 * @param color 清屏颜色
 */
void lcd_clear(pLCD lcd, Color color)
{
    lcd_fill_pure(lcd, 0, 0, lcd->display_width, lcd->display_height, color);
}

/**
 * @brief 在指定坐标画字符
 * 
 * @param sx 起始 X坐标
 * @param sy 起始 Y坐标
 * @param ch 要显示的字符
 * @param ch_size 字符大小
 * @param forgrand 前景色
 * @param background 背景色
 * @return 字符宽度
 */
uint8_t lcd_draw_char(pLCD lcd, uint16_t sx, uint16_t sy, char ch, uint8_t ch_size, Color forgrand, Color background)
{
    //计算字符在字库中的偏移量
    size_t index = ch - ' ';
    //判断字符是否可渲染, 否则返回宽度 0
    if (index < 0 || index >= 95)
        return 0;
    uint16_t ch_width = ch_size / 2;
    uint16_t ch_height = ch_size;
    const uint8_t *ch_data;

    //选择字库
    switch (ch_size)
    {
    case 12:
        ch_data = asc2_1206[index];
        break;
    case 16:
        ch_data = asc2_1608[index];
        break;
    case 24:
        ch_data = asc2_2412[index];
        break;
    default:
        //无对应字库, 返回宽度为 0
        return 0;
    }

    uint16_t actual_width = ch_width, actual_height = ch_height;
    Color *toFill = lcd_set_window(lcd, sx, sy, &actual_width, &actual_height);

    for (size_t row = 0; row < actual_height; row++)
    {
        for (size_t col = 0; col < actual_width; col++)
        {
            /**
             * @brief 字库数据格式
             * 数字 0 - 字库字符按列存储数据, 存满 8bit存到下一字节, 不足8bit后面补零
             * 1206字库, 一列高12像素, 需要12bit, 所以每列用2字节存储
             * 2412字库, 一列高24像素, 需要24bit, 所以每列用3字节存储
             * 1206 - 高12宽6      2412 - 高24宽12(24*12/8=36字节)
             * [ 1       1  ]       [     1 11 1              1  11  1   ]
             * [   1 1 1    ]       [    1  11  1              1 11 1    ]
             * [  1 1 1     ]       [    1  11  1              1 11 1    ]
             * [1       1   ]       [    1  1   1  1        1  1 1  1    ]
             * [1       1   ]       [    1  1      1  1  1  1    1  1    ]
             * [1       1   ]       [    1  1    1  1  1  1      1  1    ]
             * [1       1   ]       [    1  1 1           1  1   1  1    ]
             * [1       1   ]       [    1 11                1  11  1    ]
             */
            //位于连续存储数据的第几个像素, IntegerCeilingDivision(ch_height, 8) -> 求每列需要的字节数
            uint16_t pixel_index = col * IntegerCeilingDivision(ch_height, 8) * 8 + row;
            uint8_t data_col = ch_data[pixel_index / 8];
            *toFill = (data_col << (pixel_index % 8)) & 0x80 ? forgrand : background;
        }
    }
    return ch_width;
}

/**
 * @brief 在指定坐标画字符串
 * 
 * @param sx 起始 X坐标
 * @param sy 起始 Y坐标
 * @param str 要显示的字符串
 * @param ch_size 字符大小
 * @param forgrand 前景色
 * @param background 背景色
 * @return 字符串宽度
 */
uint16_t lcd_draw_str(pLCD lcd, uint16_t sx, uint16_t sy, const char *str, uint8_t ch_size, Color forgrand, Color background)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        sx += lcd_draw_char(lcd, sx, sy, str[i], ch_size, forgrand, background);
    }
    return sx;
}