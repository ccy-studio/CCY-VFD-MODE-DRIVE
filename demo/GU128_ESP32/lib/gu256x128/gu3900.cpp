/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-09-10 19:42:42
 * @LastEditTime: 2023-09-10 20:27:31
 */
#include "gu3900.h"

void gu3900_init() {
    // Initialize Display
    GU_VFD_WRITE(0x1B);
    GU_VFD_WRITE(0x40);

    // 2-byte Character
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x28);
    GU_VFD_WRITE(0x67);
    GU_VFD_WRITE(0x02);
    GU_VFD_WRITE(0x01);

    // // 2-byte character type
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x28);
    GU_VFD_WRITE(0x67);
    GU_VFD_WRITE(0x03);
    GU_VFD_WRITE(0x02);

    // Display Power ON/OFF
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x28);
    GU_VFD_WRITE(0x61);
    GU_VFD_WRITE(0x40);
    GU_VFD_WRITE(0x01);
}

void gu3900_display_clear() {
    GU_VFD_WRITE(0x0C);
}

/**
 * 亮度调节 0~4 | 0=0%, 1=25%, 2=50%, 3=75%, 4=100%
 */
void gu3900_set_brightness(uint8_t level) {
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x58);
    GU_VFD_WRITE(level);
}

/**
 * Input 512×128   x255 y128
 * 0000h ≤ (xL + xH×100h) ≤ Max_Xdot 1FFh = 511 /1dot
 * 0000h ≤ (yL + yH×100h) ≤ Max_Ybyte 0Fh =  15 /8 dots
 */
void gu3900_cursor_set(uint16_t x, uint16_t y) {
    y = y / 8;
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x24);
    GU_VFD_WRITE(((uint8_t)x));
    GU_VFD_WRITE(((uint8_t)x >> 8));
    GU_VFD_WRITE(((uint8_t)y));
    GU_VFD_WRITE(((uint8_t)y >> 8));
}

void gu3900_set_font_size(font_type_t font) {
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x28);
    GU_VFD_WRITE(0x67);
    GU_VFD_WRITE(0x01);
    GU_VFD_WRITE(font);
}

void gu3900_set_font_bold(bool open) {
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x28);
    GU_VFD_WRITE(0x67);
    GU_VFD_WRITE(0x41);
    GU_VFD_WRITE(open);
}

void gu3900_print(const char* str) {
    GU_VFD_WRITE_S(str);
}

void gu3900_write_byte(uint8_t* bytes, size_t len) {
    Serial1.write(bytes, len);
}

/**
 * 回退
 */
void gu3900_backspace() {
    GU_VFD_WRITE(0x08);
}
/**
 * 回车
 */
void gu3900_carriage_return() {
    GU_VFD_WRITE(0x0D);
}

/**
 * 开关覆盖模式
 */
void gu3900_overwrite_mode(bool open) {
    // Over-Write mode
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(open);
}

/**
 * 点画
 */
void gu3900_dot_drawing(bool pen, uint16_t x, uint16_t y) {
    uint8_t byte_arr[9];
    byte_arr[0] = 0x1F;
    byte_arr[1] = 0x28;
    byte_arr[2] = 0x64;
    byte_arr[3] = 0x10;
    byte_arr[4] = pen;
    byte_arr[5] = (x >> 0);
    byte_arr[6] = (x >> 8);
    byte_arr[7] = (y >> 0);
    byte_arr[8] = (y >> 8);
    gu3900_write_byte(byte_arr, sizeof(byte_arr));
}

/**
 * 实时位图显示
 */
void gu3900_realtime_bit_image_display(uint16_t x,
                                       uint16_t y,
                                       uint8_t* bytes,
                                       size_t len) {
    GU_VFD_WRITE(0x1F);
    GU_VFD_WRITE(0x28);
    GU_VFD_WRITE(0x66);
    GU_VFD_WRITE(0x11);

    GU_VFD_WRITE(x);
    GU_VFD_WRITE(x >> 8);
    GU_VFD_WRITE(y);
    GU_VFD_WRITE(y >> 8);

    GU_VFD_WRITE(0x01);
    Serial1.write(bytes, len);
}
