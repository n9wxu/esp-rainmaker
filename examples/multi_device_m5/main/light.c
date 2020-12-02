
#include "light.h"
// #include <sdkconfig.h>
#include <string.h>

#include <esp_log.h>

#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_types.h> 
#include <esp_rmaker_standard_params.h> 

#include "user_parameters.h"

#include "display.h"

#include <sk6812.h>
//#include <axp192.h>

#define TAG "light"

const int light_pin = GPIO_NUM_2;

#define DEFAULT_HUE         180
#define DEFAULT_SATURATION  100
#define DEFAULT_BRIGHTNESS  25

#define LED_COUNT 10

static esp_rmaker_device_t *light_device;

static bool isOn = true;
static uint8_t hue = DEFAULT_HUE;
static uint8_t saturation = DEFAULT_SATURATION;
static uint8_t brightness = DEFAULT_BRIGHTNESS;

static pixel_settings_t px;
static uint8_t pixelBuffer[3 * LED_COUNT] = {0};

static void light_set_hsv(int h, int s, int v)
{
    px.brightness = v;
    for (int i = 0; i < LED_COUNT; i++) {
        np_set_pixel_color_hsb(&px,i,h,s,v);
    }
}

static esp_err_t light_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    const char *device_name = esp_rmaker_device_get_name(device);
    const char *param_name = esp_rmaker_param_get_name(param);
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %s for %s - %s",
                val.val.b? "true" : "false", device_name, param_name);
        light_set_on(val.val.b);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_BRIGHTNESS_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        light_set_brightness(val.val.i);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_HUE_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        light_set_hue(val.val.i);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_SATURATION_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        light_set_saturation(val.val.i);
    } else {
        /* Silently ignoring invalid params */
        return ESP_OK;
    }
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}

void light_init(esp_rmaker_node_t *node)
{
    px.pixel_count = LED_COUNT;
    px.brightness = brightness;
    sprintf(px.color_order, "GRBW");
    px.nbits = 24;
    px.timings.t0h = (350);
    px.timings.t0l = (800);
    px.timings.t1h = (600);
    px.timings.t1l = (700);
    px.timings.reset = 80000;
    px.pixels = pixelBuffer;
    neopixel_init(light_pin, RMT_CHANNEL_0);
    light_set_on(isOn);

        /* Create a Light device and add the relevant parameters to it */
    light_device = esp_rmaker_lightbulb_device_create("Light", NULL, DEFAULT_LIGHT_POWER);
    esp_rmaker_device_add_cb(light_device, light_cb, NULL);
    
    esp_rmaker_device_add_param(light_device, esp_rmaker_brightness_param_create(ESP_RMAKER_DEF_BRIGHTNESS_NAME, DEFAULT_LIGHT_BRIGHTNESS));
    esp_rmaker_device_add_param(light_device, esp_rmaker_hue_param_create(ESP_RMAKER_DEF_HUE_NAME, DEFAULT_LIGHT_HUE));
    esp_rmaker_device_add_param(light_device, esp_rmaker_saturation_param_create(ESP_RMAKER_DEF_SATURATION_NAME, DEFAULT_LIGHT_SATURATION));

    esp_rmaker_device_add_attribute(light_device, "Serial Number", "012345");
    esp_rmaker_device_add_attribute(light_device, "MAC", "xx:yy:zz:aa:bb:cc");

    esp_rmaker_node_add_device(node, light_device);

}

void light_set_on(bool on)
{
    isOn = on;
    if(on)
    {
        light_set_hsv(hue,saturation,brightness);
        display_lights_on(hue,saturation,brightness);
    }
    else
    {
        np_clear(&px);
        display_switch_off();
    }    
    np_show(&px,RMT_CHANNEL_0);
}

void light_set_hue(int h)
{
    hue = h;
    light_set_on(true);
}

void light_set_saturation(int s)
{
    saturation = s;
    light_set_on(true);
}

void light_set_brightness(int b)
{
    brightness = b;
    light_set_on(true);
}

bool light_is_on(void)
{
    return isOn;
}

int light_get_hue(void)
{
    return hue;
}

int light_get_saturation(void)
{
    return saturation;
}

int light_get_brightness(void)
{
    return brightness;
}

