/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-31 20:52:37
 * @LastEditTime: 2023-09-05 18:09:34
 */
#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <constant.h>
#include <gui.h>
#include <rx8025.h>
#include <te200k.h>
#include "store.h"
#include "web_server.h"
#include "LittleFS.h"

void handle_key_interrupt();
void handle_wifi_config(WiFiManager* myWiFiManager);
void handle_wifi_timeout();

u32 key_last_time;

Adafruit_AHTX0 aht;
rx8025_timeinfo timeinfo;

extern WiFiManager wifiManager;

void setup() {
    Serial.begin(115200);
    init_key_gpio();
    init_buz_gpio();
    init_vfd_en_gpio();
    init_ir_gpio();
    attachInterrupt(KEY1_PIN, handle_key_interrupt, CHANGE);
    attachInterrupt(KEY2_PIN, handle_key_interrupt, CHANGE);
    attachInterrupt(KEY3_PIN, handle_key_interrupt, CHANGE);

    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!aht.begin(&Wire, 0L, 0x38)) {
        Serial.println("Could not find AHT? Check wiring");
        while (1)
            delay(10);
    }
    Serial.println("AHT10 or AHT20 found");
    init_8025t();
    rx8025_reset();
    // rx8025_set_all(23, 9, 4, 1, 23, 59, 0);

    gui_init();

    delay(1000);
    store_init();
    wifi_start(handle_wifi_config, handle_wifi_timeout);
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

    delay(100);
    // rx8025_read_all(&timeinfo);
    // printf("Time: %d-%d-%d %d:%d:%d Week:%x\n", timeinfo.year,
    // timeinfo.month,
    //        timeinfo.day, timeinfo.hour, timeinfo.min, timeinfo.sec,
    //        timeinfo.week);

    wifi_loop();
    printf("Sycce\n");

    // Serial.print("IP address: ");
    // Serial.println(WiFi.localIP());
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

void handle_wifi_config(WiFiManager* myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
}
void handle_wifi_timeout() {
    printf("Wifi Config Timeout!\n");
}