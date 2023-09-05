#ifndef __WEB_S_H
#define __WEB_S_H

#include <constant.h>
#include "WiFi.h"
#include <DNSServer.h>
#include "WiFiManager.h"
#include "WebServer.h"

typedef void (*ConfigModeCallback)(WiFiManager* myWiFiManager);
typedef void (*ConfigTimeOutCallback)();

void wifi_start(ConfigModeCallback modeCallback,ConfigTimeOutCallback timeoutCallback);
void wifi_stop();
void wifi_erase();
void wifi_loop();

#endif