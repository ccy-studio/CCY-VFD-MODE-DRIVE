/*
 * @Description: 
 * @Author: chenzedeng
 * @Date: 2023-08-31 21:01:47
 * @LastEditTime: 2023-08-31 21:57:49
 */
#include <constant.h>

void init_key_gpio() {
    pinMode(KEY1_PIN, INPUT);
    pinMode(KEY2_PIN, INPUT);
    pinMode(KEY3_PIN, INPUT);
}
void init_rgb_gpio() {
    pinMode(RGB_PIN, OUTPUT);
}
void init_buz_gpio() {
    pinMode(BUZ_PIN, OUTPUT);
}
void init_ir_gpio() {
    pinMode(IR_PIN, OUTPUT);
}
void init_vfd_en_gpio() {
    pinMode(VFD_EN_PIN, OUTPUT);
}