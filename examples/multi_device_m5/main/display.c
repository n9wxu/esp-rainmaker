#include "display.h"

#include <esp_log.h>

#include <esp_rmaker_core.h>

#include "core2forAWS.h"

#include "lvgl/lvgl.h"

extern SemaphoreHandle_t xGuiSemaphore;

void display_init()
{
    Core2ForAWS_LCD_Init();

   xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    lv_obj_t * test_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(test_label, 10, 5);
    lv_label_set_align(test_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(test_label, "Test");

    xSemaphoreGive(xGuiSemaphore);

}

void display_lights_off(void)
{
    ESP_LOGI("display","lights off");
}

void display_lights_on(int h, int s, int v)
{
    ESP_LOGI("display","lights on");

}

void display_fan_spinning(int s)
{
    ESP_LOGI("display","fan spinning %d",s);
}

void display_fan_off()
{
    ESP_LOGI("display","fan off");
}

void display_switch_on()
{
    ESP_LOGI("display","switch on");
}

void display_switch_off()
{
    ESP_LOGI("display","switch off");
}

void display_temperature(int c)
{
    ESP_LOGI("display","temperature %d",c);
}
