/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-31 20:52:37
 * @LastEditTime: 2023-09-10 19:31:13
 */
#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <constant.h>
#include <gui.h>
#include <rx8025.h>
#include <te200k.h>
#include "LittleFS.h"
#include "store.h"
#include "web_server.h"

void handle_key_interrupt();
void handle_wifi_config(WiFiManager* myWiFiManager);
void handle_wifi_timeout();
u8 set_time(String* date);
u8 set_aht20_val(String* str);

u32 key_last_time;

Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
rx8025_timeinfo timeinfo;
String date_str;
u8 last_date_len;
u8 last_second_line_len;
String second_line_str;

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
    // rx8025_set_all(23, 9, 10, 7, 14, 36, 0);

    delay(500);
    gui_init();
    gui_set_brightness(4);
    gui_print("Hello Design By Saisaiwa");
    delay(1000);
    gui_clear_all();
    store_init();
    // wifi_start(handle_wifi_config, handle_wifi_timeout);
}

void loop() {
    delay(500);
    if (set_time(&date_str)) {
        gui_clear_line(1);
    }
    gui_print(1, 1, date_str.c_str());

    if (set_aht20_val(&second_line_str)) {
        gui_clear_line(2);
    }
    gui_print_cn(1, 2, second_line_str.c_str());

    // wifi_loop();
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

u8 set_time(String* date) {
    date->clear();
    rx8025_read_all(&timeinfo);
    *date += "20";
    *date += timeinfo.year;
    *date += "-";
    if (timeinfo.month < 10) {
        *date += "0";
    }
    *date += timeinfo.month;
    *date += "-";
    if (timeinfo.day < 10) {
        *date += "0";
    }
    *date += timeinfo.day;

    *date += " ";
    if (timeinfo.hour < 10) {
        *date += "0";
    }
    *date += timeinfo.hour;
    *date += ":";

    if (timeinfo.min < 10) {
        *date += "0";
    }
    *date += timeinfo.min;
    *date += ":";

    if (timeinfo.sec < 10) {
        *date += "0";
    }
    *date += timeinfo.sec;
    u8 result = last_date_len != date->length();
    last_date_len = date->length();
    return result;
}

u8 set_aht20_val(String* str) {
    str->clear();
    aht.getEvent(&humidity, &temp);
    *str += "温度";
    *str += ":";
    *str += (int)temp.temperature;
    *str += "℃ ";
    *str += "湿度";
    *str += ":";
    *str += (int)humidity.relative_humidity;
    *str += "%";

    u8 result = last_second_line_len != str->length();
    last_second_line_len = str->length();
    return result;
}