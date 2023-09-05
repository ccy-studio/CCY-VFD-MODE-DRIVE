#include <gui.h>
#include <te200k.h>

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
    if (x > MAX_X - 1) {
        x = MAX_X - 1;
    }
    if (y > MAX_Y - 1) {
        y = MAX_Y - 1;
    }
    te200k_cursor_set(x, y);
    TE_VFD_WRITE_S(str);
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
        te200k_cursor_set(0, i);
        TE_VFD_WRITE_S(cp);
    }
}

void gui_clear_all() {
    te200k_display_clear();
}
void gui_clear_line(u8 y) {
    if (y + 1 > MAX_Y) {
        y = MAX_Y - 1;
    }
    te200k_cursor_set(0, 1);
    te200k_line_clear();
}
void gui_set_brightness(u8 brightness) {
    te200k_display_brightness(brightness);
}