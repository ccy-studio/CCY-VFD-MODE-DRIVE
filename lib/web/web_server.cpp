/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-05 14:24:45
 * @LastEditTime: 2023-09-05 18:05:19
 */
#include "web_server.h"
#include <ArduinoJson.h>
#include <store.h>
#include "web_index_html.h"

WiFiManager wifiManager;
WebServer server(80);

void web_init();
void handle_404();
void handle_root();
void http_reqeust_save_setteing();
void http_reqeust_get_setteing();

void wifi_start(ConfigModeCallback modeCallback,
                ConfigTimeOutCallback timeoutCallback) {
    if (WiFi.isConnected()) {
        return;
    }

    wifiManager.setAPCallback(modeCallback);
    wifiManager.setCountry("CN");
    wifiManager.setBreakAfterConfig(true);
    wifiManager.setTimeout(60);
    String ssid = "VFD-" + String(ESP.getEfuseMac() >> 32);
    if (!wifiManager.autoConnect(ssid.c_str(), NULL)) {
        Serial.println("Failed to connect and hit timeout.");
        while (1) {
            timeoutCallback();
        }
    }
    // 配网成功，打印连接信息
    Serial.println("Connected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    web_init();
}

void web_init() {
    server.onNotFound(handle_404);
    server.on("/", handle_root);
    server.on("/save-setting", http_reqeust_save_setteing);
    server.on("/get-setting", http_reqeust_get_setteing);
    server.begin();
}

void wifi_loop() {
    if (WiFi.isConnected()) {
        server.handleClient();
    }
}

void handle_404() {
    String html = "<h1>404</h1>";
    server.send(404, "text/html; charset=utf-8", html);
}

void handle_root() {
    String str = FPSTR(HTML_INDEX);
    server.send(200, "text/html; charset=utf-8", str);
}

void wifi_stop() {
    server.close();
    wifiManager.disconnect();
}

void wifi_erase() {
    wifiManager.erase();
}

void http_reqeust_save_setteing() {
    String body = server.arg("plain");  // 获取POST请求的内容

#ifdef DEBUG
    Serial.println("Received POST data: " + body);
#endif
    // 解析JSON数据
    DynamicJsonDocument jsonDoc(1024);  // 调整合适的JSON文档大小
    DeserializationError error = deserializeJson(jsonDoc, body);

    if (error) {
        Serial.println("JSON parsing error!");
        server.send(400, "text/plain", "Bad Request");
        return;
    }

    // 处理JSON数据
    JsonObject root = jsonDoc.as<JsonObject>();
    strcpy(setting_obj.custom_long_text, root["customLongText"]);
    setting_obj.custom_long_text_frame = root["customLongTextFrame"].as<u8>();

    setting_obj.auto_power = root["autoPower"].as<u8>();
    strcpy(setting_obj.auto_power_open_time, root["autoPowerOpenTime"]);
    strcpy(setting_obj.auto_power_close_time, root["autoPowerCloseTime"]);
    JsonArray arrTmp = root["autoPowerEnableDays"].as<JsonArray>();
    memset(setting_obj.auto_power_enable_days, 0,
           sizeof(setting_obj.auto_power_enable_days));
    for (u8 i = 0; i < arrTmp.size(); i++) {
        setting_obj.auto_power_enable_days[i] = arrTmp[i].as<u8>();
    }

    setting_obj.alarm_clock = root["alarmClock"].as<u8>();
    strcpy(setting_obj.alarm_clock_time, root["alarmClockTime"]);
    arrTmp = root["alarmClockEnableDays"].as<JsonArray>();
    memset(setting_obj.alarm_clock_enable_days, 0,
           sizeof(setting_obj.alarm_clock_enable_days));
    for (u8 i = 0; i < arrTmp.size(); i++) {
        setting_obj.alarm_clock_enable_days[i] = arrTmp[i].as<u8>();
    }

    setting_obj.countdown = root["countdown"].as<u8>();
    strcpy(setting_obj.countdown_time, root["countdownTime"]);

#ifdef DEBUG
    // 测试打印输出结果
    store_print_debug(setting_obj);
#endif

    // 写数据
    store_save_setting(&setting_obj);

    server.send(200, "application/json", "success");
}

void http_reqeust_get_setteing() {
    // 创建DynamicJsonDocument对象
    DynamicJsonDocument jsonDoc(1024);

    // 将结构体数据转换为JSON
    jsonDoc["customLongText"] = setting_obj.custom_long_text;
    jsonDoc["customLongTextFrame"] = setting_obj.custom_long_text_frame;
    jsonDoc["autoPower"] = setting_obj.auto_power ? true : false;
    jsonDoc["autoPowerOpenTime"] = setting_obj.auto_power_open_time;
    jsonDoc["autoPowerCloseTime"] = setting_obj.auto_power_close_time;

    JsonArray autoPowerEnableDaysArray =
        jsonDoc.createNestedArray("autoPowerEnableDays");
    for (int i = 0; i < 7; i++) {
        autoPowerEnableDaysArray.add(setting_obj.auto_power_enable_days[i]);
    }

    jsonDoc["alarmClock"] = setting_obj.alarm_clock ? true : false;
    jsonDoc["alarmClockTime"] = setting_obj.alarm_clock_time;

    JsonArray alarmClockEnableDaysArray =
        jsonDoc.createNestedArray("alarmClockEnableDays");
    for (int i = 0; i < 7; i++) {
        alarmClockEnableDaysArray.add(setting_obj.alarm_clock_enable_days[i]);
    }

    jsonDoc["countdown"] = setting_obj.countdown ? true : false;
    jsonDoc["countdownTime"] = setting_obj.countdown_time;

    // 将JSON数据发送到客户端
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
}