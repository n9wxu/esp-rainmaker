#include "display.h"

#include <esp_log.h>

#include <esp_rmaker_core.h>

#include "core2forAWS.h"

#include "lvgl/lvgl.h"

extern SemaphoreHandle_t xGuiSemaphore;

LV_IMG_DECLARE(img_House);

LV_IMG_DECLARE(img_fan_off);
LV_IMG_DECLARE(img_fan0);
LV_IMG_DECLARE(img_fan1);
LV_IMG_DECLARE(img_fan2);
LV_IMG_DECLARE(img_fan3);
LV_IMG_DECLARE(img_fan4);
LV_IMG_DECLARE(img_fan5);
LV_IMG_DECLARE(img_fan6);
LV_IMG_DECLARE(img_fan7);

LV_IMG_DECLARE(img_switch);

LV_IMG_DECLARE(img_thermometer);

static lv_obj_t *light_object;
static lv_obj_t *fan_object;
static lv_obj_t *switch_object;
static lv_obj_t *temperature_object;

void display_init()
{
    Core2ForAWS_LCD_Init();

   xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    light_object = 


void lv_ex_img_1(void)
{
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &img_cogwheel_argb);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t * img2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img2, LV_SYMBOL_OK "Accept");
    lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}



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
