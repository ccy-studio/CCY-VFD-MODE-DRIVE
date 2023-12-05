/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:46:28
 * @LastEditTime: 2023-09-04 14:10:49
 */
#ifndef __RX8025_H
#define __RX8025_H

#include <Wire.h>

#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define RX8025_ADDR 0x32

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} rx8025_timeinfo;

void init_8025t();
void rx8025_reset();
void rx8025_set_all(uint8_t year,
                    uint8_t month,
                    uint8_t day,
                    uint8_t week,
                    uint8_t hour,
                    uint8_t min,
                    uint8_t sec);
void rx8025_read_all(rx8025_timeinfo* timeinfo);

#endif