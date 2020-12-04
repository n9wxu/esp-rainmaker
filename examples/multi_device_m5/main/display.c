#include "display.h"

#include <esp_log.h>

#include <esp_rmaker_core.h>

#include "core2forAWS.h"

#include "lvgl/lvgl.h"

#include "ft6336u.h"

extern SemaphoreHandle_t xGuiSemaphore;

LV_IMG_DECLARE(HouseOn);
LV_IMG_DECLARE(HouseOff);
LV_IMG_DECLARE(Thermometer);
LV_IMG_DECLARE(ButtonOn);
LV_IMG_DECLARE(ButtonOff);
LV_IMG_DECLARE(fanoff);
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

static const lv_img_dsc_t* fanImages[] = {&one,&two,&three,&four,&five,&six,&seven,&eight,&nine,&ten,&eleven};

static lv_obj_t *light_object;
static lv_obj_t *fan_object;
static lv_obj_t *temperature_object;

static esp_timer_handle_t spin_timer;

static int g_fan_speed = 0;
static bool screenConfigured = false;

static void spin_update(void *priv)
{
    static int fan_index = 0;
    if(ESP_OK == xSemaphoreTake(xGuiSemaphore, 1000))
    {
        const lv_img_dsc_t* image;
        if(g_fan_speed)
        {
            image = fanImages[fan_index++];
            if(fan_index > 10) fan_index = 0;
        }
        else
        {
            image = &fanoff;
        }
        lv_img_set_src(fan_object, image);
        lv_event_send_refresh(light_object);
        xSemaphoreGive(xGuiSemaphore);
    }
}

void display_init()
{
    Core2ForAWS_LCD_Init();
    FT6336U_Init();
    Core2ForAWS_LCD_SetBrightness(100);

    screenConfigured = false;
}

static void display_setupscreen()
{
    if(!screenConfigured)
    {
        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

        light_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(light_object, &HouseOn);
        lv_obj_set_pos(light_object, 0,0);

        fan_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(fan_object, &one);
        lv_obj_set_pos(fan_object, 220,0);

        temperature_object = lv_img_create(lv_scr_act(), NULL);
        lv_img_set_src(temperature_object, &Thermometer);
        lv_obj_set_pos(temperature_object, 185,100);

        xSemaphoreGive(xGuiSemaphore);

        esp_timer_create_args_t spin_timer_conf = {
            .callback = spin_update,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "spin_update_tm"
        };
        if (esp_timer_create(&spin_timer_conf, &spin_timer) == ESP_OK) {
            esp_timer_start_periodic(spin_timer, 10U);
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
