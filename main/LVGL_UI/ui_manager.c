#include "ui_manager.h"

static const char *TAG = "UIManager";
static lv_obj_t *ta_ssid, *ta_password, *btn_connect, *label_status;


/* 连接按钮点击回调 */
static void connect_button_cb(lv_event_t *e) {
    const char *ssid = lv_textarea_get_text(ta_ssid);
    const char *password = lv_textarea_get_text(ta_password);
    // const char *ssid ="realme GT6";
    // const char *password = "e6bpjxjs";
    if (strlen(ssid) == 0) {
        lv_label_set_text(label_status, "Error: SSID cannot be empty!");
        return;
    }

    // 禁用按钮防止重复点击
    lv_obj_add_state(btn_connect, LV_STATE_DISABLED);
    lv_label_set_text(label_status, "Connecting to WiFi...");
    lv_timer_handler(); // 强制刷新UI

    // 在独立任务中执行连接操作（避免阻塞UI）
    xTaskCreate(&connect_task, "wifi_connect_task", 4096, NULL, 5, NULL);
}

/* WiFi连接任务 */
static void connect_task(void *arg) {
    const char *ssid = lv_textarea_get_text(ta_ssid);
    const char *password = lv_textarea_get_text(ta_password);
    // const char *ssid ="realme GT6";
    // const char *password = "e6bpjxjs";
    wifi_manager_connect(ssid, password);

    // 根据连接结果更新UI
    if (xEventGroupGetBits(s_wifi_event_group) & WIFI_CONNECTED_BIT) {
        lv_label_set_text(label_status, "Connection failed!");
    } else {
        lv_label_set_text(label_status, "Connected!");
    }

    // 重新启用按钮
    lv_obj_clear_state(btn_connect, LV_STATE_DISABLED);
    vTaskDelete(NULL);
}

/* 创建WiFi连接界面 */
void ui_create_wifi_connect_screen() {
    // 创建容器
    
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 240, 320);
    lv_obj_center(cont);

    // SSID输入框
    ta_ssid = lv_textarea_create(cont);
    lv_obj_set_size(ta_ssid, 200, 40);
    lv_obj_align(ta_ssid, LV_ALIGN_TOP_MID, 0, 20);
    lv_textarea_set_placeholder_text(ta_ssid, "Enter SSID");
    lv_textarea_set_one_line(ta_ssid, true);

    // 密码输入框
    ta_password = lv_textarea_create(cont);
    lv_obj_set_size(ta_password, 200, 40);
    lv_obj_align_to(ta_password, ta_ssid, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_textarea_set_placeholder_text(ta_password, "Enter Password");
    lv_textarea_set_password_mode(ta_password, true);
    lv_textarea_set_one_line(ta_password, true);

    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_textarea(kb, ta_ssid);
    lv_obj_align_to(kb, ta_ssid, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_flags(kb, LV_OBJ_FLAG_HIDDEN);

    // 绑定焦点事件
    lv_obj_add_event_cb(ta_ssid, ta_focus_cb, LV_EVENT_FOCUSED, kb);
    lv_obj_add_event_cb(ta_password, ta_focus_cb, LV_EVENT_FOCUSED, kb);

    // 连接按钮
    btn_connect = lv_btn_create(cont);
    lv_obj_set_size(btn_connect, 100, 40);
    lv_obj_align_to(btn_connect, ta_password, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_add_event_cb(btn_connect, connect_button_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_label = lv_label_create(btn_connect);
    lv_label_set_text(btn_label, "Connect");
    lv_obj_center(btn_label);

    // 状态标签
    label_status = lv_label_create(cont);
    lv_obj_set_width(label_status, 220);
    lv_obj_align_to(label_status, btn_connect, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_label_set_text(label_status, "Ready");
    lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP);
}

// 焦点事件回调
static void ta_focus_cb(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = lv_event_get_user_data(e);
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN); // 显示键盘
}