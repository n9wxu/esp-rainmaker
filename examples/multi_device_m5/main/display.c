#include "display.h"

#include <esp_log.h>

#include <esp_rmaker_core.h>

#include "core2forAWS.h"

#include "lvgl/lvgl.h"

extern SemaphoreHandle_t xGuiSemaphore;

LV_IMG_DECLARE(HouseOn);
LV_IMG_DECLARE(HouseOff);
LV_IMG_DECLARE(Thermometer);
LV_IMG_DECLARE(ButtonOn);
LV_IMG_DECLARE(ButtonOff);
LV_IMG_DECLARE(one);
LV_IMG_DECLARE(two);
LV_IMG_DECLARE(three);
LV_IMG_DECLARE(four);
LV_IMG_DECLARE(five);
LV_IMG_DECLARE(six);
LV_IMG_DECLARE(seven);
LV_IMG_DECLARE(eight);
LV_IMG_DECLARE(nine);
LV_IMG_DECLARE(ten);
LV_IMG_DECLARE(eleven);


static lv_obj_t *light_object;
static lv_obj_t *fan_object;
static lv_obj_t *switch_object;
static lv_obj_t *temperature_object;

static esp_timer_handle_t spin_timer;

static int g_fan_speed = 0;
static int g_object_position = 0;
static bool left = true;

static bool screenConfigured = false;

static void spin_update(void *priv)
{
    if(ESP_OK == xSemaphoreTake(xGuiSemaphore, 1000))
    {
        if(g_fan_speed)
        {
            // slide the image

            if(g_object_position > 320 - 203)
            {
                left = false;
            }
            if(g_object_position < 0)
            {
                left = true;
            }

            if(left)
            {
                g_object_position += 2;
            }
            else
            {
                g_object_position -= 2;
            }
            
        }
        else
        {
            g_object_position = 0;
        }
        lv_obj_set_pos(light_object, g_object_position,20);
        xSemaphoreGive(xGuiSemaphore);
    }
}

void display_init()
{
    screenConfigured = false;
}

static void display_setupscreen()
{
    if(!screenConfigured)
    {
        Core2ForAWS_LCD_Init();
        Core2ForAWS_LCD_SetBrightness(100);

        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

        light_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(light_object, &HouseOn);
        lv_obj_set_pos(light_object, 0,20);

        fan_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(fan_object, &one);
        lv_obj_set_pos(fan_object, 220,0);

        switch_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(switch_object, &ButtonOn);
        lv_obj_set_pos(switch_object, 220,120);

        temperature_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(temperature_object, &Thermometer);
        lv_obj_set_pos(temperature_object, 220,160);

        xSemaphoreGive(xGuiSemaphore);

        esp_timer_create_args_t spin_timer_conf = {
            .callback = spin_update,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "spin_update_tm"
        };
        if (esp_timer_create(&spin_timer_conf, &spin_timer) == ESP_OK) {
            esp_timer_start_periodic(spin_timer, 200U);
        }

        screenConfigured = true;
    }
}

void display_lights_off(void)
{
    display_setupscreen();
    ESP_LOGI("display","lights off");
}

void display_lights_on(int h, int s, int v)
{
    display_setupscreen();
    ESP_LOGI("display","lights on");
}

void display_fan_spinning(int s)
{
    display_setupscreen();
    g_fan_speed = s;
    ESP_LOGI("display","fan spinning %d",s);
}

void display_fan_off()
{
    display_setupscreen();
    ESP_LOGI("display","fan off");
}

void display_switch_on()
{
    display_setupscreen();
    ESP_LOGI("display","switch on");
}

void display_switch_off()
{
    display_setupscreen();
    ESP_LOGI("display","switch off");
}

void display_temperature(int c)
{
    display_setupscreen();
    ESP_LOGI("display","temperature %d",c);
}
