#include "display.h"

#include <esp_log.h>

#include <esp_rmaker_core.h>

#include "core2forAWS.h"

#include "lvgl/lvgl.h"

#include "ft6336u.h"

#include "fan.h"

#define TAG "display"

extern SemaphoreHandle_t xGuiSemaphore;

LV_IMG_DECLARE(HouseOn);
LV_IMG_DECLARE(HouseOff);
LV_IMG_DECLARE(Thermometer);
LV_IMG_DECLARE(fanoff);
LV_IMG_DECLARE(one);
LV_IMG_DECLARE(three);
LV_IMG_DECLARE(five);
LV_IMG_DECLARE(seven);
LV_IMG_DECLARE(nine);
LV_IMG_DECLARE(eleven);

static const lv_img_dsc_t* fanImages[] = {&one,&three,&five,&seven,&nine,&eleven};

static lv_obj_t *light_object;
static lv_obj_t *fan_object;
static lv_obj_t *temperature_object;
static lv_obj_t *fan_strength_slider;
static lv_obj_t *fan_sw1;

static int g_fan_speed = 0;
static bool g_fan_power = false;
static bool screenConfigured = false;

static void spin_update(void *priv)
{
    int fan_index = 0;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if(pdTRUE == xSemaphoreTake(xGuiSemaphore, 0))
        {
            if(g_fan_speed && g_fan_power)
            {
                ESP_LOGI("spin","Fan Index %d",fan_index);
                lv_img_set_src(fan_object, fanImages[fan_index++]);
                if(fan_index >= (sizeof(fanImages)/sizeof(*fanImages))) fan_index = 0;
            }
            else
            {
                lv_img_set_src(fan_object, &fanoff);
            }
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

static void strength_slider_event_cb(lv_obj_t * slider, lv_event_t event)
{

    if(event == LV_EVENT_VALUE_CHANGED) {
        fan_set_speed(lv_slider_get_value(slider));
        fan_speed_report();
    }
}

static void sw1_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        fan_set_power(lv_switch_get_state(obj));
        fan_power_report();
    }
}

void display_init()
{
    FT6336U_Init();
    Core2ForAWS_LCD_Init();
    Core2ForAWS_LCD_SetBrightness(100);
}

void display_fan_init()
{

    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    ESP_LOGI(TAG,"configuring the display");

    fan_object = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(fan_object, &fanoff);
    lv_obj_align(fan_object, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    ESP_LOGI(TAG,"configured fan_object");

    fan_strength_slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(fan_strength_slider, 8);
    lv_obj_set_height(fan_strength_slider, 80);
    lv_obj_align(fan_strength_slider, fan_object, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_obj_set_event_cb(fan_strength_slider, strength_slider_event_cb);
    lv_slider_set_value(fan_strength_slider, 0, LV_ANIM_OFF);
    lv_slider_set_range(fan_strength_slider, 0, 5);

    ESP_LOGI(TAG,"configured fan_strength_slider");

    fan_sw1 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_size(fan_sw1, 60, 20);
    lv_obj_align(fan_sw1, fan_object, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_event_cb(fan_sw1, sw1_event_handler);
    lv_switch_off(fan_sw1, LV_ANIM_OFF);
    ESP_LOGI(TAG,"configured fan_sw1");

    xSemaphoreGive(xGuiSemaphore);

    xTaskCreatePinnedToCore(spin_update, "fan", 4096, NULL, 2, NULL, 1);

    ESP_LOGI(TAG,"display configured");
}

void display_lights_off(void)
{
    ESP_LOGI("display","lights off");
}

void display_lights_on(int h, int s, int v)
{
    ESP_LOGI("display","lights on");
}

void display_fan_speed(int s)
{
    lv_slider_set_value(fan_strength_slider,s,LV_ANIM_OFF);
    ESP_LOGI("display","fan spinning %d",s);
    g_fan_speed = s;
}

void display_fan_off()
{
    ESP_LOGI("display","fan off");
    lv_switch_off(fan_sw1, LV_ANIM_OFF);
    g_fan_power = false;
}

void display_fan_on()
{
    ESP_LOGI("display","switch on");
    lv_switch_on(fan_sw1, LV_ANIM_OFF);
    g_fan_power = true;
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
