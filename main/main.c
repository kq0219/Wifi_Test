#include "ST7789.h"
#include "PCF85063.h"
#include "QMI8658.h"
#include "SD_MMC.h"
#include "Wireless.h"
#include "LVGL_Example.h"
#include "BAT_Driver.h"
#include "PCM5101.h"
#include "ui_demo.h"
#include "wifi_manager.h"
#include "ui_manager.h"

void Driver_Loop(void *parameter)
{
    Wireless_Init();
    while(1)
    {
        QMI8658_Loop();
        PCF85063_Loop();
        BAT_Get_Volts();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}
void Driver_Init(void)
{
    Flash_Searching();
    BAT_Init();
    I2C_Init();
    PCF85063_Init();
    QMI8658_Init();
    xTaskCreatePinnedToCore(
        Driver_Loop, 
        "Other Driver task",
        4096, 
        NULL, 
        3, 
        NULL, 
        0);
}
void app_main(void)
{
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // 关键：在此处统一创建Driver_Init();

    SD_Init();
    LCD_Init();
    Audio_Init();
    // Play_Music("/sdcard","AAA.mp3");
    LVGL_Init();   // returns the screen object

/********************* Demo *********************/
    //Lvgl_Example1();
    //lv_demo_widgets();
    // lv_demo_keypad_encoder();
    // lv_demo_benchmark();
    // lv_demo_stress();
    // lv_demo_music();
    // example_event();

    ui_create_wifi_connect_screen();
    // xTaskCreate(lv_timer_task, "lvgl", 4096, NULL, 5, NULL);
    wifi_manager_init();
    while (1) {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(10));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
    }
}






