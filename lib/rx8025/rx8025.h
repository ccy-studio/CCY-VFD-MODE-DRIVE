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
#include <constant.h>

#define RX8025_ADDR 0x32

typedef struct {
    u8 year;
    u8 month;
    u8 day;
    u8 week;
    u8 hour;
    u8 min;
    u8 sec;
} rx8025_timeinfo;

void init_8025t();
void rx8025_reset();
void rx8025_set_all(u8 year,
                    u8 month,
                    u8 day,
                    u8 week,
                    u8 hour,
                    u8 min,
                    u8 sec);
void rx8025_read_all(rx8025_timeinfo* timeinfo);

#endif