/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#include <stdint.h>
#include <stdbool.h>

#define DEFAULT_POWER       true
#define DEFAULT_HUE         180
#define DEFAULT_SATURATION  100
#define DEFAULT_BRIGHTNESS  25

extern esp_rmaker_device_t *left_light_device;
extern esp_rmaker_device_t *right_light_device;

void app_driver_init(void);
esp_err_t app_light_set_left(uint32_t hue, uint32_t saturation, uint32_t brightness);
esp_err_t app_light_set_power_left(bool power);
esp_err_t app_light_set_brightness_left(uint16_t brightness);
esp_err_t app_light_set_hue_left(uint16_t hue);
esp_err_t app_light_set_saturation_left(uint16_t saturation);

esp_err_t app_light_set_right(uint32_t hue, uint32_t saturation, uint32_t brightness);
esp_err_t app_light_set_power_right(bool power);
esp_err_t app_light_set_brightness_right(uint16_t brightness);
esp_err_t app_light_set_hue_right(uint16_t hue);
esp_err_t app_light_set_saturation_right(uint16_t saturation);
