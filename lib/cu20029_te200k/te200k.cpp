/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-20 18:46:14
 * @LastEditTime: 2023-08-20 22:58:47
 */
#include <te200k.h>

void asynchro_send(u8 buf) {
    // while (digitalRead(TE_VFD_DTR)) {
    // }
    TE_VFD_WRITE(buf);
}

void te200k_init(void) {
    pinMode(TE_VFD_DTR, INPUT);
    delay(1000);

    // Initialize display
    asynchro_send(0x1B);
    asynchro_send(0x40);

    // International font set
    // 1Bh 52h n
    asynchro_send(0x1B);
    asynchro_send(0x52);
    asynchro_send(0x08);

    // Character table type
    // 1Bh 74h n
    asynchro_send(0x1B);
    asynchro_send(0x74);
    asynchro_send(0x01);

    // Open Setting Simplified Chinese
    //- Open Support 1Fh 28h 67h 02h 1
    asynchro_send(0x1F);
    asynchro_send(0x28);
    asynchro_send(0x67);
    asynchro_send(0x02);
    asynchro_send(0x01);
    //- Set Type Simplified Chinese
    // 1Fh 28h 67h 03h n
    asynchro_send(0x1F);
    asynchro_send(0x28);
    asynchro_send(0x67);
    asynchro_send(0x03);
    asynchro_send(0x02);
    // 1Fh 28h 67h 0Fh n
    asynchro_send(0x1F);
    asynchro_send(0x28);
    asynchro_send(0x67);
    asynchro_send(0x0F);
    asynchro_send(0x02);

    te200k_cursor_set(1, 1);
    te200k_display_power(1);
    te200k_display_reverse(0);
    te200k_display_brightness(1);
    
    // te200k_display_blink(255);
}

void te200k_cursor_set(u8 x, u8 y) {
    asynchro_send(0x1F);
    asynchro_send(0x24);
    asynchro_send(x);
    asynchro_send(y);
}

void te200k_cursor_display(u8 enable) {
    asynchro_send(0x1F);
    asynchro_send(0x43);
    asynchro_send(enable);
}

void te200k_display_power(u8 enable) {
    // 1Fh 28h 61h 40h n
    asynchro_send(0x1F);
    asynchro_send(0x28);
    asynchro_send(0x61);
    asynchro_send(0x40);
    asynchro_send(enable);
}

void te200k_display_reverse(u8 enable) {
    // 1Fh 72h n
    asynchro_send(0x1F);
    asynchro_send(0x72);
    asynchro_send(enable);
}

void te200k_display_brightness(u8 level) {
    // 1Fh 58h n
    asynchro_send(0x1F);
    asynchro_send(0x58);
    asynchro_send(level);
}

void te200k_display_blink(u8 level) {
    // 1Fh 45h n
    asynchro_send(0x1F);
    asynchro_send(0x45);
    asynchro_send(level);
}

void te200k_display_clear(void) {
    asynchro_send(0x0C);
}

void te200k_line_clear(void) {
    asynchro_send(0x18);
}

void te200k_test(void) {
    for (u8 data = 0x20; data <= 0x7F; data++) {
        asynchro_send(data);
    }
}