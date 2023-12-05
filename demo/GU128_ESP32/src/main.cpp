/*
 * @Description: 3900
 * @Author: chenzedeng
 * @Date: 2023-08-17 00:19:55
 * @LastEditTime: 2023-09-11 23:38:03
 */
#include <Adafruit_AHTX0.h>
#include <Arduino.h>
#include "gu3900.h"
#include "rx8025.h"

#define KEY1_PIN 5
#define KEY2_PIN 10
#define KEY3_PIN 4

#define RS232_TX 19
#define RS232_RX 18
#define VFD_EN_PIN 3

typedef struct {
    uint8_t gpio;
    uint8_t state;  // 0 press, 1 release, 2 long press
    uint32_t press_time;
} button_t;

typedef struct {
    uint8_t* val;
    char prefix[8];
    int max;
    int min;
} set_content_t;

button_t btn_arr[3] = {{KEY1_PIN, 1}, {KEY2_PIN, 1}, {KEY3_PIN, 1}};
QueueHandle_t queue;

#define CONTENT_CLOCK 0
#define CONTENT_SET_CLOCK 1
volatile uint8_t current_content = CONTENT_CLOCK;
uint8_t brightness = 4;

Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
rx8025_timeinfo timeinfo, set_timeinfo;
char str_buf[50];
uint8_t loop_count = 0, acg_type = 0;
set_content_t set_content_arr[6];  // 年月日时分秒
uint8_t set_pos = 0;

void rt_task_get_aht(void* params);
void handle_key_task(void* params);
void button_callback(void* params);
void init_set_content();
void set_cotent_update();

void setup() {
    Serial.begin(115200);
    Serial1.begin(38400, SERIAL_8N1, RS232_RX, RS232_TX);
    pinMode(KEY1_PIN, INPUT);
    pinMode(KEY2_PIN, INPUT);
    pinMode(KEY3_PIN, INPUT);
    queue = xQueueCreate(3, sizeof(button_t));

    init_8025t();
    if (aht.begin()) {
        Serial.println("Found AHT20");
    } else {
        Serial.println("Didn't find AHT20");
    }
    pinMode(VFD_EN_PIN, OUTPUT);
    digitalWrite(VFD_EN_PIN, HIGH);

    delay(1000);

    gu3900_init();
    gu3900_set_brightness(brightness);
    gu3900_overwrite_mode(true);

    xTaskCreate(rt_task_get_aht, "AHT", 1024, NULL, 2, NULL);
    xTaskCreate(handle_key_task, "keyScan", 1024, NULL, 2, NULL);
    xTaskCreate(button_callback, "BtnCall", 2048, NULL, 2, NULL);
}

void loop() {
    delay(500);
    if (CONTENT_CLOCK == current_content) {
        rx8025_read_all(&timeinfo);
        memset(str_buf, 0, sizeof(str_buf));
        sprintf(str_buf, "20%02d-%02d-%02d", timeinfo.year, timeinfo.month,
                timeinfo.day);
        gu3900_set_font_size(Font_6x8);
        // 10个字符单字符12dot,255/2-(6*10)
        gu3900_cursor_set(97, 24);
        gu3900_print(str_buf);

        gu3900_set_font_size(Font_16x32);
        gu3900_set_font_bold(1);
        memset(str_buf, 0, sizeof(str_buf));
        sprintf(str_buf, "%02d:%02d:%02d", timeinfo.hour, timeinfo.min,
                timeinfo.sec);
        // 8字符:
        // 1. x单字符16 255/2-(16*8/2)
        // 2. y单字符32 128/2-(32/2)
        gu3900_cursor_set(63, 48);
        gu3900_print(str_buf);

        memset(str_buf, 0, sizeof(str_buf));
        sprintf(str_buf, "Temp:%02.1f Hum:%02.1f ", temp.temperature,
                humidity.relative_humidity);
        gu3900_set_font_size(Font_6x8);
        gu3900_set_font_bold(false);
        gu3900_cursor_set(0, 120);
        gu3900_print(str_buf);

        loop_count++;
        if (loop_count >= 2) {
            // 关闭反显
            GU_VFD_WRITE(0x1f);
            GU_VFD_WRITE(0x72);
            GU_VFD_WRITE(0);
        }
        if (loop_count >= 50) {
            acg_type = (acg_type + 1) % 4;
            if (acg_type == 0) {
                GU_VFD_WRITE(0x1f);
                GU_VFD_WRITE(0x28);
                GU_VFD_WRITE(0x61);
                GU_VFD_WRITE(0x14);
                GU_VFD_WRITE(0x01);
                GU_VFD_WRITE((uint8_t)0x1FFF);
                GU_VFD_WRITE((uint8_t)0x1FFF >> 8);
            } else if (acg_type == 1) {
                GU_VFD_WRITE(0x1f);
                GU_VFD_WRITE(0x28);
                GU_VFD_WRITE(0x61);
                GU_VFD_WRITE(0x12);
                GU_VFD_WRITE(0x00);
                GU_VFD_WRITE(0x01);
                GU_VFD_WRITE(0x01);
            } else if (acg_type == 2) {
                GU_VFD_WRITE(0x1f);
                GU_VFD_WRITE(0x28);
                GU_VFD_WRITE(0x61);
                GU_VFD_WRITE(0x10);
                GU_VFD_WRITE((uint8_t)0x1FFF);
                GU_VFD_WRITE((uint8_t)0x1FFF >> 8);
                GU_VFD_WRITE(60);
                GU_VFD_WRITE(0);
                GU_VFD_WRITE(0x01);
            } else if (acg_type == 3) {
                GU_VFD_WRITE(0x1f);
                GU_VFD_WRITE(0x72);
                GU_VFD_WRITE(1);
            }
            gu3900_display_clear();
            loop_count = 0;
        }
    }
}

void rt_task_get_aht(void* params) {
    while (1) {
        aht.getEvent(&humidity, &temp);
        delay(2000);
    }
}

void button_callback(void* params) {
    portBASE_TYPE xStatus;
    button_t btn;
    while (1) {
        delay(1);
        xStatus = xQueueReceive(queue, &btn, 0);
        if (xStatus != pdTRUE) {
            continue;
        }
        printf("按键触发:IO%d,state:%d\n", btn.gpio, btn.state);

        if (current_content == CONTENT_CLOCK) {
            if (btn.gpio == KEY1_PIN) {
                if (btn.state == 0) {
                    brightness--;
                    if (brightness == 0) {
                        brightness = 1;
                    }
                    gu3900_set_brightness(brightness);
                } else if (btn.state == 2) {
                }
            } else if (btn.gpio == KEY2_PIN) {
                if (btn.state == 0) {
                    brightness++;
                    if (brightness == 5) {
                        brightness = 4;
                    }
                    gu3900_set_brightness(brightness);
                } else if (btn.state == 2) {
                }
            } else if (btn.gpio == KEY3_PIN) {
                if (btn.state == 0) {
                } else if (btn.state == 2) {
                    // memcpy(&set_timeinfo, &timeinfo, sizeof(timeinfo));
                    set_timeinfo.year = timeinfo.year;
                    set_timeinfo.month = timeinfo.month;
                    set_timeinfo.day = timeinfo.day;
                    set_timeinfo.hour = timeinfo.hour;
                    set_timeinfo.min = timeinfo.min;
                    set_timeinfo.sec = timeinfo.sec;
                    init_set_content();
                    current_content = CONTENT_SET_CLOCK;
                    set_cotent_update();
                }
            }
        } else if (current_content == CONTENT_SET_CLOCK) {
            if (btn.gpio == KEY1_PIN) {
                if (btn.state == 0) {
                    set_content_t* obj = &set_content_arr[set_pos];
                    if (*(obj->val) - 1 < obj->min) {
                        *(obj->val) = obj->max;
                    } else {
                        *(obj->val) -= 1;
                    }
                    set_cotent_update();
                }
            } else if (btn.gpio == KEY2_PIN) {
                if (btn.state == 0) {
                    set_content_t* obj = &set_content_arr[set_pos];
                    if (btn.state == 0) {
                        if (*(obj->val) + 1 > obj->max) {
                            *(obj->val) = obj->min;
                        } else {
                            *(obj->val) += 1;
                        }
                    }
                    set_cotent_update();
                }
            } else if (btn.gpio == KEY3_PIN) {
                if (btn.state == 0) {
                    if (set_pos + 1 >= 6) {
                        set_pos = 0;
                    } else {
                        set_pos++;
                    }
                    set_cotent_update();
                } else if (btn.state == 2) {
                    // 保存时间
                    rx8025_set_all(set_timeinfo.year, set_timeinfo.month,
                                   set_timeinfo.day, 0, set_timeinfo.hour,
                                   set_timeinfo.min, set_timeinfo.sec);
                    gu3900_display_clear();
                    current_content = CONTENT_CLOCK;
                }
            }
        }
    }
}

void handle_key_logic(button_t* btn) {
    if (btn->press_time >= 2000 && btn->state == 0) {
        // 如果大于4秒就全部忽略
        btn->press_time += 50;
        return;
    }
    uint8_t level = digitalRead(btn->gpio);
    if (level) {
        if (btn->state == 0) {
            btn->state = 1;
            btn->press_time = 0;
            // 松开手了
            xQueueSend(queue, btn, 0);
            return;
        }
        btn->state = 1;
        btn->press_time = 0;
    } else {
        // is press
        if (btn->state == 0) {
            // ignore
            btn->press_time += 50;
        } else if (btn->state == 1) {
            btn->state = 0;
            btn->press_time = 0;
            // call btn
            xQueueSend(queue, btn, 0);
            return;
        }
        if (btn->press_time >= 2000) {
            if (btn->state != 2) {
                btn->press_time += 50;
                btn->state = 2;
                // call long press
                xQueueSend(queue, btn, 0);
                return;
            }
        }
    }
}

void handle_key_task(void* params) {
    while (1) {
        handle_key_logic(&btn_arr[0]);
        handle_key_logic(&btn_arr[1]);
        handle_key_logic(&btn_arr[2]);
        delay(50);
    }
}

void init_set_content() {
    set_pos = 0;
    set_content_arr[0].max = 99;
    set_content_arr[0].min = 23;
    set_content_arr[0].val = &set_timeinfo.year;
    strcpy(set_content_arr[0].prefix, "Year:20");

    set_content_arr[1].max = 12;
    set_content_arr[1].min = 1;
    set_content_arr[1].val = &set_timeinfo.month;
    strcpy(set_content_arr[1].prefix, "Month:");

    set_content_arr[2].max = 31;
    set_content_arr[2].min = 1;
    set_content_arr[2].val = &set_timeinfo.day;
    strcpy(set_content_arr[2].prefix, "Day:");

    set_content_arr[3].max = 23;
    set_content_arr[3].min = 0;
    set_content_arr[3].val = &set_timeinfo.hour;
    strcpy(set_content_arr[3].prefix, "Hour:");

    set_content_arr[4].max = 59;
    set_content_arr[4].min = 0;
    set_content_arr[4].val = &set_timeinfo.min;
    strcpy(set_content_arr[4].prefix, "Minute:");

    set_content_arr[5].max = 59;
    set_content_arr[5].min = 0;
    set_content_arr[5].val = &set_timeinfo.sec;
    strcpy(set_content_arr[5].prefix, "Second:");
}

void set_cotent_update() {
    gu3900_display_clear();

    gu3900_set_font_size(Font_16x32);
    gu3900_set_font_bold(true);
    memset(str_buf, 0, sizeof(str_buf));
    sprintf(str_buf, "%s%02d", set_content_arr[set_pos].prefix,
            *set_content_arr[set_pos].val);
    uint8_t x = 255 / 2 - (strlen(str_buf) * 16 / 2);
    gu3900_cursor_set(x, 48);
    gu3900_print(str_buf);
}