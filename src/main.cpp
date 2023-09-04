/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-31 20:52:37
 * @LastEditTime: 2023-09-04 14:38:11
 */
#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <constant.h>
#include <rx8025.h>
#include <te200k.h>

void led_run(void* pvParameters);
void handle_key_interrupt();

u32 key_last_time;

Adafruit_AHTX0 aht;
rx8025_timeinfo timeinfo;

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
    rx8025_set_all(23, 9, 4, 1, 18, 0, 0);

    digitalWrite(VFD_EN_PIN, HIGH);
    delay(100);
    Serial1.begin(38400, SERIAL_8N1, RS232_RX, RS232_TX);
    te200k_init();
    // pinMode(RS232_IN, OUTPUT);
    // pinMode(RS232_OUT, OUTPUT);
    // pinMode(RS232_TX, OUTPUT);
    // pinMode(RS232_RX, OUTPUT);

    // analogWrite(VFD_EN_PIN, 128);
    // analogWrite(RS232_TX, 128);
    // analogWrite(RS232_IN, 128);
    // analogWrite(RS232_OUT, 128);

    attachInterrupt(KEY1_PIN, handle_key_interrupt, CHANGE);
    attachInterrupt(KEY2_PIN, handle_key_interrupt, CHANGE);
    attachInterrupt(KEY3_PIN, handle_key_interrupt, CHANGE);

    // xTaskCreate(led_run, "LED Task", 800, NULL, 1, NULL);
}

void loop() {
    // delay(1000);
    // te200k_display_clear();
    // te200k_cursor_set(1, 1);
    // TE_VFD_WRITE_S("Hello");
    // delay(1000);
    // te200k_cursor_set(1, 2);
    // TE_VFD_WRITE_S("VFDTest Success");

    // sensors_event_t humidity, temp;
    // aht.getEvent(&humidity, &temp);
    // printf("温度：%f -- 湿度: %f\n", temp.temperature,
    //        humidity.relative_humidity);

    delay(500);
    rx8025_read_all(&timeinfo);
    printf("Time: %d-%d-%d %d:%d:%d\n", timeinfo.year, timeinfo.month,
           timeinfo.day, timeinfo.hour, timeinfo.min, timeinfo.sec);
}

void handle_key_interrupt() {
    u32 filter_sec = (micros() - key_last_time) / 1000;
    if (filter_sec < 500) {
        return;
    }
    if (!digitalRead(KEY1_PIN)) {
        Serial.print("KEY1 Press\n");
    } else if (!digitalRead(KEY2_PIN)) {
        Serial.print("KEY2 Press\n");
    } else if (!digitalRead(KEY3_PIN)) {
        Serial.print("KEY3 Press\n");
    }
    key_last_time = micros();
}

void led_run(void* pvParameters) {
    while (1) {
    }
}