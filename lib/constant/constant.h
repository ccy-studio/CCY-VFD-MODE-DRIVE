/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:19:42
 * @LastEditTime: 2023-09-04 10:54:37
 */
/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-31 20:57:30
 * @LastEditTime: 2023-08-31 22:30:56
 */
#ifndef __CONSTANT_H
#define __CONSTANT_H
#include <Arduino.h>

#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1

#define KEY1_PIN 5
#define KEY2_PIN 10
#define KEY3_PIN 4

#define RX8025_IN_PIN 6
#define BUZ_PIN 7
#define IR_PIN 2

#define RS232_IN 8
#define RS232_OUT 9
#define RS232_TX 19
#define RS232_RX 18

#define VFD_EN_PIN 3

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define V_DELAY_MS(ms) vTaskDelay(pdMS_TO_TICKS(ms))

void init_key_gpio();
void init_buz_gpio();
void init_ir_gpio();
void init_vfd_en_gpio();

#endif