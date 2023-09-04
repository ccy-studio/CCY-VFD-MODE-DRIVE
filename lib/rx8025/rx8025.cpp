#include <rx8025.h>

void init_8025t() {
    pinMode(RX8025_IN_PIN, INPUT);
    Wire.begin();
}

void rx8025_reset() {
    Wire.beginTransmission(RX8025_ADDR_W);
    u8 command[2];
    command[0] = 0x0F;
    command[1] = 0x01;
    Wire.write(command, 2);
    delay(1200);
    Wire.endTransmission(true);
}

u8 toBcd(u8 val) {
    return ((val / 10) << 4) | (val % 10);
}

u8 toDec(u8 bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rx8025_set_all(u8 year,
                    u8 month,
                    u8 day,
                    u8 week,
                    u8 hour,
                    u8 min,
                    u8 sec) {
    Wire.beginTransmission(RX8025_ADDR_W);
    u8 command[8];
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
    Wire.beginTransmission(RX8025_ADDR_W);
    Wire.write(0x00);  // 寄存器地址，用于读取日期和时间
    Wire.endTransmission(false);
    size_t recv = Wire.requestFrom(RX8025_ADDR_R, 7, true);
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