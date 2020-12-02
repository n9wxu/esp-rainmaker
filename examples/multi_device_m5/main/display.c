#include "display.h"

#include <esp_log.h>

#include <esp_rmaker_core.h>

#include "core2forAWS.h"

#include "lvgl/lvgl.h"

extern SemaphoreHandle_t xGuiSemaphore;

LV_IMG_DECLARE(img_light);

// LV_IMG_DECLARE(img_fan_off);
// LV_IMG_DECLARE(img_fan0);
// LV_IMG_DECLARE(img_fan1);
// LV_IMG_DECLARE(img_fan2);
// LV_IMG_DECLARE(img_fan3);
// LV_IMG_DECLARE(img_fan4);
// LV_IMG_DECLARE(img_fan5);
// LV_IMG_DECLARE(img_fan6);
// LV_IMG_DECLARE(img_fan7);

// LV_IMG_DECLARE(img_switch);

// LV_IMG_DECLARE(img_thermometer);

static lv_obj_t *light_object;
static lv_obj_t *fan_object;
static lv_obj_t *switch_object;
static lv_obj_t *temperature_object;

static esp_timer_handle_t spin_timer;

static int g_fan_speed = 0;
static int g_object_position = 0;
static bool left = true;

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
    Core2ForAWS_LCD_Init();
    Core2ForAWS_LCD_SetBrightness(100);

   xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    light_object = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(light_object, &img_light);
    lv_obj_set_pos(light_object, 0,20);

    lv_obj_t * test_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(test_label, 10, 5);
    lv_label_set_align(test_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(test_label, "Test");

    xSemaphoreGive(xGuiSemaphore);

    esp_timer_create_args_t spin_timer_conf = {
        .callback = spin_update,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "spin_update_tm"
    };
    if (esp_timer_create(&spin_timer_conf, &spin_timer) == ESP_OK) {
        esp_timer_start_periodic(spin_timer, 200U);
    }

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
    g_fan_speed = s;
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
