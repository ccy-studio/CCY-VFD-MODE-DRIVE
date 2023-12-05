/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-09-10 19:42:34
 * @LastEditTime: 2023-09-10 20:18:43
 */
#ifndef __gu3900X128_H
#define __gu3900X128_H

#include <Arduino.h>

#define GU_VFD_WRITE(b) Serial1.write(b)
#define GU_VFD_WRITE_S(s) Serial1.print(PSTR(s))

typedef enum {
    Font_6x8 = 1,
    Font_8x16,
    Font_12x24,
    Font_16x32,
} font_type_t;

void gu3900_init();

void gu3900_display_clear();

/**
 * 亮度调节 0~4 | 0=0%, 1=25%, 2=50%, 3=75%, 4=100%
 */
void gu3900_set_brightness(uint8_t level);

void gu3900_cursor_set(uint16_t x, uint16_t y);

/**
 * 字体大小选择  (m=1,2,4 / 1="6X8", 2="8X16", 4="16X32")
 */
void gu3900_set_font_size(font_type_t font);

void gu3900_set_font_bold(bool open);

void gu3900_print(const char* str);

void gu3900_write_byte(uint8_t* bytes, size_t len);

/**
 * 回退
 */
void gu3900_backspace();
/**
 * 回车
 */
void gu3900_carriage_return();

/**
 * 开关覆盖模式
 */
void gu3900_overwrite_mode(bool open);

/**
 * 点画
 */
void gu3900_dot_drawing(bool pen, uint16_t x, uint16_t y);

/**
 * 实时位图显示
 */
void gu3900_realtime_bit_image_display(uint16_t x,
                                       uint16_t y,
                                       uint8_t* bytes,
                                       size_t len);

#endif