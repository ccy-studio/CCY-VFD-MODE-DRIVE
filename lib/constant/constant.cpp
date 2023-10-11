/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-31 21:01:47
 * @LastEditTime: 2023-10-11 18:01:38
 */
#include <constant.h>

void init_key_gpio() {
    pinMode(KEY1_PIN, INPUT);
    pinMode(KEY2_PIN, INPUT);
    pinMode(KEY3_PIN, INPUT);
}
void init_buz_gpio() {
    pinMode(BUZ_PIN, OUTPUT);
    digitalWrite(BUZ_PIN, LOW);
}
void init_ir_gpio() {
    pinMode(IR_PIN, OUTPUT);
    digitalWrite(IR_PIN, LOW);
}
void init_vfd_en_gpio() {
    pinMode(VFD_EN_PIN, OUTPUT);
    digitalWrite(VFD_EN_PIN, LOW);
}

/**
 * VFD电源Enable
 */
void vfd_power(bool openState) {
    digitalWrite(VFD_EN_PIN, openState ? HIGH : LOW);
}