/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-09-08 22:41:34
 * @LastEditTime: 2023-09-10 15:04:37
 */
#include <gui.h>
#include <te200k.h>
#include "gb2312_font.h"

#define MAX_X 20  // VFD单行最大位数
#define MAX_Y 2   // VFD最大行

void gui_init() {
    vfd_power(true);
    delay(500);
    Serial1.begin(38400, SERIAL_8N1, RS232_RX, RS232_TX);
    te200k_init();
    te200k_display_brightness(255);
}
void gui_print(u8 x, u8 y, const char* str) {
    if (x > MAX_X) {
        x = MAX_X;
    }
    if (y > MAX_Y) {
        y = MAX_Y;
    }
    te200k_cursor_set(x, y);
    TE_VFD_WRITE_S(str);
}

void gui_print_cn(u8 x, u8 y, const char* str) {
    if (x > MAX_X) {
        x = MAX_X;
    }
    if (y > MAX_Y) {
        y = MAX_Y;
    }
    te200k_cursor_set(x, y);
    u8 len = strlen(str);
    u8 buf[len];
    convert_gb2312(str, buf);
    for (size_t i = 0; i < len; i++) {
        if (!buf[i] || buf[i] == '\0') {
            break;
        }
        TE_VFD_WRITE(buf[i]);
    }
}

void gui_print(const char* str) {
    te200k_display_clear();
    size_t len = strlen(str);
    u8 count = len / MAX_X;
    if (len % MAX_X != 0) {
        count++;
    }
    if (count > MAX_Y) {
        count = MAX_Y;
    }
    char cp[MAX_X + 1];
    for (u8 i = 0; i < count; i++) {
        memset(cp, 0, sizeof(cp));
        memcpy(cp, str, MAX_X * sizeof(char));
        str += MAX_X;
        te200k_cursor_set(0, i + 1);
        TE_VFD_WRITE_S(cp);
    }
}

void gui_clear_all() {
    te200k_display_clear();
}
void gui_clear_line(u8 y) {
    if (y > MAX_Y) {
        y = MAX_Y;
    }
    te200k_cursor_set(1, y);
    te200k_line_clear();
}
void gui_set_brightness(u8 brightness) {
    te200k_display_brightness(brightness);
}