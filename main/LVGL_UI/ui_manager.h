#ifndef __UI_MANAGER_H__
#define __UI_MANAGER_H__

#include "wifi_manager.h"
#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"       // FreeRTOS 核心头文件
#include "freertos/task.h"           // 任务管理 API（如 xTaskCreate）
#include "freertos/event_groups.h"   // 事件组 API（如 xEventGroupGetBits）

static void connect_task(void *arg);
static void connect_button_cb(lv_event_t *e);
void ui_create_wifi_connect_screen();

#endif