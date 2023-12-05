/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:53:37
 * @LastEditTime: 2023-09-05 10:52:37
 */
#include <rx8025.h>

void init_8025t() {
    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.begin();
}

void rx8025_reset() {
    // clean flag
    Wire.beginTransmission(RX8025_ADDR);
    uint8_t command[3];
    command[0] = 0xe0;
    command[1] = 0x00;
    command[2] = 0x40;
    Wire.write(command, 3);
    Wire.endTransmission(true);
}

uint8_t toBcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t toDec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rx8025_set_all(uint8_t year,
                    uint8_t month,
                    uint8_t day,
                    uint8_t week,
                    uint8_t hour,
                    uint8_t min,
                    uint8_t sec) {
    Wire.beginTransmission(RX8025_ADDR);
    uint8_t command[8];
    command[0] = 0x00;
    command[1] = toBcd(sec);
    command[2] = toBcd(min);
    command[3] = toBcd(hour);
    command[4] = (0x00 >> week) | 0x01;
    command[5] = toBcd(day);
    command[6] = toBcd(month);
    command[7] = toBcd(year);
    Wire.write(command, 8);
    Wire.endTransmission(true);
}

void rx8025_read_all(rx8025_timeinfo* timeinfo) {
    Wire.beginTransmission(RX8025_ADDR);
    Wire.write(0x00);  // 寄存器地址，用于读取日期和时间
    Wire.endTransmission(false);
    size_t recv = Wire.requestFrom(RX8025_ADDR, 7, 1);
    if (recv != 7) {
        printf("rx8025_read_all 读取错误\n");
        return;
    }
    timeinfo->sec = toDec(Wire.read());
    timeinfo->min = toDec(Wire.read());
    timeinfo->hour = toDec(Wire.read());
    timeinfo->week = Wire.read();
    timeinfo->day = toDec(Wire.read());
    timeinfo->month = toDec(Wire.read());
    timeinfo->year = toDec(Wire.read());
}