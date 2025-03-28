#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <string.h>          // 提供strncpy/memset/memcpy等函数
#include "esp_wifi.h"        // 提供wifi_config_t和WiFi API
#include "esp_event.h"       // 提供事件循环宏（如WIFI_EVENT）
#include "freertos/FreeRTOS.h"       // FreeRTOS 核心头文件
#include "freertos/task.h"           // 任务管理 API（如 xTaskCreate）
#include "freertos/event_groups.h"   // 事件组 API（如 xEventGroupGetBits）


extern EventGroupHandle_t s_wifi_event_group; 
static wifi_config_t s_wifi_config = {0};
static char s_ssid[32] = {0};
static char s_password[64] = {0};

// 定义WiFi事件标志位
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void wifi_manager_init();
void wifi_manager_connect(const char *ssid, const char *password);
void wifi_manager_disconnect();

#endif