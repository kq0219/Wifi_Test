#include "wifi_manager.h"

static const char *TAG = "WiFiManager";
EventGroupHandle_t s_wifi_event_group;
/* WiFi事件处理函数 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, 
                              int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        esp_wifi_connect(); // 自动重连
        ESP_LOGI(TAG, "Retry to connect to AP...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* 初始化WiFi底层配置 */
void wifi_manager_init() {
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 创建事件组和默认网络接口
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 初始化WiFi驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // 注册事件处理器
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
}

/* 设置WiFi凭证并触发连接 */
void wifi_manager_connect(const char *ssid, const char *password) {
    strncpy(s_ssid, ssid, sizeof(s_ssid) - 1);
    strncpy(s_password, password, sizeof(s_password) - 1);

    // 配置WiFi参数
    memset(&s_wifi_config, 0, sizeof(wifi_config_t));
    memcpy(s_wifi_config.sta.ssid, s_ssid, strlen(s_ssid));
    memcpy(s_wifi_config.sta.password, s_password, strlen(s_password));
    s_wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &s_wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // 等待连接结果（超时30秒）
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(30000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to AP SSID: %s", s_ssid);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Failed to connect to SSID: %s", s_ssid);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/* 断开WiFi连接 */
void wifi_manager_disconnect() {
    ESP_ERROR_CHECK(esp_wifi_stop());
    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
}