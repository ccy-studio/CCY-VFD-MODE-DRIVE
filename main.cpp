/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-31 20:52:37
 * @LastEditTime: 2023-10-11 21:54:39
 */
#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <constant.h>
#include <gui.h>
#include <rx8025.h>
#include <te200k.h>

void handle_key_task(void* params);
void button_callback(void* params);
u8 set_time(String* date, rx8025_timeinfo* timeinfo);
u8 set_aht20_val(String* str);

u32 key_last_time;

Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
rx8025_timeinfo timeinfo;
String date_str;
u8 last_date_len;
u8 last_second_line_len;
String second_line_str;

typedef struct {
    u8 gpio;
    u8 state;  // 0 press, 1 release, 2 long press
    u32 press_time;
} button_t;

button_t btn_arr[3] = {{KEY1_PIN, 1}, {KEY2_PIN, 1}, {KEY3_PIN, 1}};
QueueHandle_t queue;

rx8025_timeinfo set_timeinfo;

#define CONTENT_CLOCK 0
#define CONTENT_SET_CLOCK 1
volatile u8 current_content = CONTENT_CLOCK;
u8 cursor_index = 0;
#define CURROR_ARR_LEN 6

typedef struct {
    u8 cursor;
    u8* val;
    u8 min;
    u8 max;
} clock_set_t;

clock_set_t clock_set_objs[CURROR_ARR_LEN];

u8 brightness = 4;

void init_set_clock() {
    clock_set_objs[0] = {
        .cursor = 3, .val = &set_timeinfo.year, .min = 20, .max = 99};
    clock_set_objs[1] = {
        .cursor = 6, .val = &set_timeinfo.month, .min = 1, .max = 12};
    clock_set_objs[2] = {
        .cursor = 9, .val = &set_timeinfo.day, .min = 1, .max = 31};
    clock_set_objs[3] = {
        .cursor = 12, .val = &set_timeinfo.hour, .min = 0, .max = 24};
    clock_set_objs[4] = {
        .cursor = 15, .val = &set_timeinfo.min, .min = 0, .max = 59};
    clock_set_objs[5] = {
        .cursor = 18, .val = &set_timeinfo.sec, .min = 0, .max = 59};
}

void setup() {
    Serial.begin(115200);
    init_key_gpio();
    init_buz_gpio();
    init_vfd_en_gpio();
    init_ir_gpio();

    queue = xQueueCreate(3, sizeof(button_t));

    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!aht.begin(&Wire, 0L, 0x38)) {
        Serial.println("Could not find AHT? Check wiring");
        while (1)
            delay(10);
    }
    Serial.println("AHT10 or AHT20 found");
    init_8025t();
    // rx8025_reset();
    // rx8025_set_all(23, 9, 10, 7, 14, 36, 0);

    gui_init();
    gui_set_brightness(brightness);
    gui_print("Hello My VFD~~~");
    delay(1000);
    for (size_t i = 1; i <= 20; i++) {
        gui_print(i, 2, ">");
        V_DELAY_MS(100);
    }
    gui_clear_all();
    xTaskCreate(handle_key_task, "keyScan", 1024, NULL, 2, NULL);
    xTaskCreate(button_callback, "BtnCall", 2048, NULL, 2, NULL);
}

void loop() {
    delay(500);
    if (current_content == CONTENT_CLOCK) {
        rx8025_read_all(&timeinfo);
        if (set_time(&date_str, &timeinfo)) {
            gui_clear_line(1);
        }
        gui_print(1, 1, date_str.c_str());

        if (set_aht20_val(&second_line_str)) {
            gui_clear_line(2);
        }
        gui_print_cn(1, 2, second_line_str.c_str());
    } else if (current_content == CONTENT_SET_CLOCK) {
        if (set_time(&date_str, &set_timeinfo)) {
            gui_clear_line(1);
        }
        gui_print(1, 1, date_str.c_str());
        te200k_cursor_set(clock_set_objs[cursor_index].cursor, 1);
    }
}

void button_callback(void* params) {
    portBASE_TYPE xStatus;
    button_t btn;
    while (1) {
        V_DELAY_MS(1);
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
                    gui_set_brightness(brightness);
                } else if (btn.state == 2) {
                }
            } else if (btn.gpio == KEY2_PIN) {
                if (btn.state == 0) {
                    brightness++;
                    if (brightness == 5) {
                        brightness = 4;
                    }
                    gui_set_brightness(brightness);
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
                    gui_set_cursor_display(1);
                    gui_clear_all();
                    cursor_index = 0;
                    init_set_clock();
                    current_content = CONTENT_SET_CLOCK;
                }
            }
        } else if (current_content == CONTENT_SET_CLOCK) {
            if (btn.gpio == KEY1_PIN) {
                if (btn.state == 0) {
                    clock_set_t obj = clock_set_objs[cursor_index];
                    if (*(obj.val) - 1 < obj.min) {
                        *(obj.val) = obj.min;
                    } else {
                        *(obj.val) -= 1;
                    }
                }
            } else if (btn.gpio == KEY2_PIN) {
                if (btn.state == 0) {
                    clock_set_t obj = clock_set_objs[cursor_index];
                    if (btn.state == 0) {
                        if (*(obj.val) + 1 > obj.max) {
                            *(obj.val) = obj.max;
                        } else {
                            *(obj.val) += 1;
                        }
                    }
                }
            } else if (btn.gpio == KEY3_PIN) {
                if (btn.state == 0) {
                    if (cursor_index + 1 >= CURROR_ARR_LEN) {
                        cursor_index = 0;
                    } else {
                        cursor_index++;
                    }
                } else if (btn.state == 2) {
                    // 保存时间
                    rx8025_set_all(set_timeinfo.year, set_timeinfo.month,
                                   set_timeinfo.day, 0, set_timeinfo.hour,
                                   set_timeinfo.min, set_timeinfo.sec);
                    gui_set_cursor_display(0);
                    gui_clear_all();
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
    u8 level = digitalRead(btn->gpio);
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
        V_DELAY_MS(50);
    }
}


u8 set_time(String* date, rx8025_timeinfo* timeinfo) {
    date->clear();
    *date += "20";
    *date += timeinfo->year;
    *date += "-";
    if (timeinfo->month < 10) {
        *date += "0";
    }
    *date += timeinfo->month;
    *date += "-";
    if (timeinfo->day < 10) {
        *date += "0";
    }
    *date += timeinfo->day;

    *date += " ";
    if (timeinfo->hour < 10) {
        *date += "0";
    }
    *date += timeinfo->hour;
    *date += ":";

    if (timeinfo->min < 10) {
        *date += "0";
    }
    *date += timeinfo->min;
    *date += ":";

    if (timeinfo->sec < 10) {
        *date += "0";
    }
    *date += timeinfo->sec;
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