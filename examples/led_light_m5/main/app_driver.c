/*  LED Lightbulb demo implementation using RGB LED

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sdkconfig.h>

#include <iot_button.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h> 
#include <esp_rmaker_standard_params.h> 

#include <app_reset.h>
#include <sk6812.h>
#include <axp192.h>
#include "app_priv.h"

/* This is the GPIO on which the power will be set */
#define OUTPUT_GPIO    15
static pixel_settings_t px;

#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    10

static uint16_t g_hue_left = DEFAULT_HUE;
static uint16_t g_saturation_left = DEFAULT_SATURATION;
static uint16_t g_value_left = DEFAULT_BRIGHTNESS;
static bool g_power_left = DEFAULT_POWER;

static uint16_t g_hue_right = DEFAULT_HUE;
static uint16_t g_saturation_right = DEFAULT_SATURATION;
static uint16_t g_value_right = DEFAULT_BRIGHTNESS;
static bool g_power_right = DEFAULT_POWER;

/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
static void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}


esp_err_t app_light_set_led_left(uint32_t hue, uint32_t saturation, uint32_t brightness)
{
    /* Whenever this function is called, light power will be ON */
    if (!g_power_left) {
        g_power_left = true;
        esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(left_light_device, ESP_RMAKER_PARAM_POWER),
                esp_rmaker_bool(g_power_left));
    }
    px.brightness = brightness;
    uint32_t r, g, b, color;
    hsv2rgb(hue, saturation, brightness, &r, &g, &b);
    for (int i = 0; i < 5; i++) {
        r = r & 0xff; g = g & 0xff; b = b & 0xff;
        color = 0;
        color = (r << 24) | (g << 16) | (b << 8);
        np_set_pixel_color(&px, i, color);
    }
    np_show(&px, RMT_CHANNEL_0);
    return 0;
}

esp_err_t app_light_set_led_right(uint32_t hue, uint32_t saturation, uint32_t brightness)
{
    /* Whenever this function is called, light power will be ON */
    if (!g_power_right) {
        g_power_right = true;
        esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(right_light_device, ESP_RMAKER_PARAM_POWER),
                esp_rmaker_bool(g_power_right));
    }
    px.brightness = brightness;
    uint32_t r, g, b, color;
    hsv2rgb(hue, saturation, brightness, &r, &g, &b);
    for (int i = 5; i < 10; i++) {
        r = r & 0xff; g = g & 0xff; b = b & 0xff;
        color = 0;
        color = (r << 24) | (g << 16) | (b << 8);
        np_set_pixel_color(&px, i, color);
    }
    np_show(&px, RMT_CHANNEL_0);
    return 0;
}

esp_err_t app_light_set_power_left(bool power)
{
    g_power_left = power;
    if (power) {
        app_light_set_led_left(g_hue_left, g_saturation_left, g_value_left);
    } else {
        if(g_power_right == false)
        {
            np_clear(&px);
            np_show(&px, RMT_CHANNEL_0);
        }
        else
        {
            app_light_set_led_left(g_hue_left, g_saturation_left, 0);
        }
    }
    return ESP_OK;
}

esp_err_t app_light_set_power_right(bool power)
{
    g_power_right = power;
    if (power) {
        app_light_set_led_right(g_hue_right, g_saturation_right, g_value_right);
    } else {
        if(g_power_left == false)
        {
            np_clear(&px);
            np_show(&px, RMT_CHANNEL_0);
        }
        else
        {
            app_light_set_led_right(g_hue_right, g_saturation_right, 0);
        }
        
    }
    return ESP_OK;
}

esp_err_t app_light_set_brightness_left(uint16_t brightness)
{
    g_value_left = brightness;
    return app_light_set_led_left(g_hue_left, g_saturation_left, g_value_left);
}
esp_err_t app_light_set_brightness_right(uint16_t brightness)
{
    g_value_right = brightness;
    return app_light_set_led_right(g_hue_right, g_saturation_right, g_value_right);
}

esp_err_t app_light_set_hue_left(uint16_t hue)
{
    g_hue_left = hue;
    return app_light_set_led_left(g_hue_left, g_saturation_left, g_value_left);
}
esp_err_t app_light_set_hue_right(uint16_t hue)
{
    g_hue_right = hue;
    return app_light_set_led_right(g_hue_right, g_saturation_right, g_value_right);
}

esp_err_t app_light_set_saturation_left(uint16_t saturation)
{
    g_saturation_left = saturation;
    return app_light_set_led_left(g_hue_left, g_saturation_left, g_value_left);
}

esp_err_t app_light_set_saturation_right(uint16_t saturation)
{
    g_saturation_right = saturation;
    return app_light_set_led_right(g_hue_right, g_saturation_right, g_value_right);
}

esp_err_t app_light_init(void)
{
    px.pixel_count = 10;
    px.brightness = g_value_left;
    sprintf(px.color_order, "GRBW");
    px.nbits = 24;
    px.timings.t0h = (350);
    px.timings.t0l = (800);
    px.timings.t1h = (600);
    px.timings.t1l = (700);
    px.timings.reset = 80000;
    px.pixels = (uint8_t *)malloc((px.nbits / 8) * px.pixel_count);
    neopixel_init(GPIO_NUM_2, RMT_CHANNEL_0);
    if (g_power_left) {
        app_light_set_led_left(g_hue_left, g_saturation_left, g_value_left);
        app_light_set_led_right(g_hue_right, g_saturation_right, g_value_right);
    } else {
        np_clear(&px);
        np_show(&px, RMT_CHANNEL_0);
    }
    return ESP_OK;
}

static void push_btn_cb(void *arg)
{
    app_light_set_power_left(!g_power_left);
    esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_type(left_light_device, ESP_RMAKER_PARAM_POWER),
            esp_rmaker_bool(g_power_left));
}
void Core2ForAWS_PMU_SetPowerIn(uint8_t mode) {
    if (mode) {
        Axp192_SetGPIO0Mode(0);
        Axp192_EnableExten(0);
    } else {
        Axp192_EnableExten(1);
        Axp192_SetGPIO0Mode(1);
    }
}

void Core2ForAWS_PMU_Init(uint16_t ldo2_volt, uint16_t ldo3_volt, uint16_t dc2_volt, uint16_t dc3_volt) {
    uint8_t value = 0x00;
    value |= (ldo2_volt > 0) << AXP192_LDO2_EN_BIT;
    value |= (ldo3_volt > 0) << AXP192_LDO3_EN_BIT;
    value |= (dc2_volt > 0) << AXP192_DC2_EN_BIT;
    value |= (dc3_volt > 0) << AXP192_DC3_EN_BIT;
    value |= 0x01 << AXP192_DC1_EN_BIT;

    Axp192_Init();

    // value |= 0x01 << AXP192_EXT_EN_BIT;
    Axp192_SetLDO23Volt(ldo2_volt, ldo3_volt);
    // Axp192_SetDCDC1Volt(3300);
    Axp192_SetDCDC2Volt(dc2_volt);
    Axp192_SetDCDC3Volt(dc3_volt);
    Axp192_SetVoffVolt(3000);
    Axp192_SetChargeCurrent(CHARGE_Current_100mA);
    Axp192_SetChargeVoltage(CHARGE_VOLT_4200mV);
    Axp192_EnableCharge(1);
    Axp192_SetPressStartupTime(STARTUP_128mS);
    Axp192_SetPressPoweroffTime(POWEROFF_4S);
    Axp192_EnableLDODCExt(value);
    Axp192_SetGPIO4Mode(1);
    Axp192_SetGPIO2Mode(1);
    Axp192_SetGPIO2Level(0);

    Axp192_SetGPIO0Volt(3300);
    Axp192_SetAdc1Enable(0xfe);
    Axp192_SetGPIO1Mode(1);
    Core2ForAWS_PMU_SetPowerIn(0);
}

void app_driver_init()
{
    Core2ForAWS_PMU_Init(3300, 0, 0, 2700);   
    app_light_init();
#if 0
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        /* Register a callback for a button tap (short press) event */
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, push_btn_cb, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
        app_reset_button_register(btn_handle, WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
    }
#endif
}
