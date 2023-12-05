#include <Adafruit_AHTX0.h>
#include <Arduino.h>
#include <Wire.h>
#include <rx8025.h>
#include <vfd.h>
#include "constant.h"

Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
rx8025_timeinfo timeinfo;

void demo_welcome(void);
void demo_dimming(void);
void demo_tmark(void);
void demo_end(void);

void setup() {
    Serial.begin(115200);
    init_key_gpio();
    init_buz_gpio();
    init_vfd_en_gpio();
    init_ir_gpio();
    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!aht.begin(&Wire, 0L, 0x38)) {
        Serial.println("Could not find AHT? Check wiring");
        while (1)
            delay(10);
    }
    Serial.println("AHT10 or AHT20 found");
    init_8025t();
    vfd_power(true);
    delay(500);
    Serial1.begin(9600, SERIAL_8N1, RS232_RX, RS232_TX);
    delay(500);
    vfd_init();
}

void loop() {
    delay(1000);
    demo_welcome();
    demo_dimming();
    demo_tmark();
    demo_end();
}

void demo_welcome(void) {
    vfd_clear();
    // \xae,\xb0 : special character
    vfd_puts_P("\xae\xae TOSHIBA  LIUST \xaf\xaf");
    vfd_gotoxy(1, 2);
    vfd_puts_P("\xb0 RS-232C INTERFACE");
    delay(2000);
}
void demo_end(void) {
    vfd_clear();
    vfd_gotoxy(4, 1);
    vfd_puts_P("Thank you for");
    vfd_gotoxy(6, 2);
    vfd_puts_P("Watching!.");
    delay(4000);

    vfd_clear();
    delay(4000);
}
void demo_dimming(void) {
    vfd_clear();
    for (uint8_t i = 0; i < VFD_DISP_LENGTH; i++)
        vfd_putc(0xdb);
    vfd_gotoxy(1, 2);
    for (uint8_t i = 0; i < VFD_DISP_LENGTH; i++)
        vfd_putc(0xdb);

    vfd_gotoxy(5, 1);
    vfd_puts_P(" Brightness ");

    vfd_gotoxy(8, 2);
    vfd_puts_P(" 100% ");
    vfd_dimming(VFD_DIMMING_100);
    delay(1500);

    vfd_gotoxy(8, 2);
    vfd_putc(0xdb);
    vfd_puts_P(" 80% ");
    vfd_dimming(VFD_DIMMING_80);
    delay(1000);

    vfd_gotoxy(9, 2);
    vfd_puts_P(" 60% ");
    vfd_dimming(VFD_DIMMING_60);
    delay(1000);

    vfd_gotoxy(9, 2);
    vfd_puts_P(" 40% ");
    vfd_dimming(VFD_DIMMING_40);
    delay(1000);

    vfd_gotoxy(9, 2);
    vfd_puts_P(" 20% ");
    vfd_dimming(VFD_DIMMING_20);
    delay(3000);

    vfd_gotoxy(8, 2);
    vfd_puts_P(" 100% ");
    vfd_dimming(VFD_DIMMING_100);
    delay(3000);
}

void demo_tmark(void) {
    uint32_t tmark = 0;

    demo_welcome();

    vfd_tmark(0xfffff);  // TMark all On
    delay(1000);

    vfd_tmark(0x0);  // TMark all Off
    delay(1000);

    //         20....+...10....+...1
    //          09876543210987654321
    vfd_tmark(0b11111000001111100000);  // TMark may be configured in bit unit.
    delay(1000);

    vfd_tmark(0b00000111110000011111);  // TMark may be configured in bit unit.
    delay(1000);

    for (uint8_t i = 0; i < 20; i++) {
        vfd_tmark(tmark << i);
        delay(100);
    }
    delay(1000);

    tmark = 0x80000;
    for (uint8_t i = 0; i < 20; i++) {
        vfd_tmark(tmark >> i);
        delay(100);
    }
    delay(2000);
}
