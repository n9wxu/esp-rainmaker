#include "fan.h"

#include <string.h>

#include <esp_log.h>

#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_types.h> 
#include <esp_rmaker_standard_params.h> 

#include "display.h"

#include "user_parameters.h"

#define TAG "FAN"

static esp_rmaker_device_t *fan_device;
static int g_fan_speed = DEFAULT_FAN_SPEED;

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t fan_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
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
        if(val.val.b)
        {
            fan_set_speed(g_fan_speed);
        }
        else
        {
            fan_set_speed(0);
        }
    } else if (strcmp(param_name, ESP_RMAKER_DEF_SPEED_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        g_fan_speed = val.val.i;
        fan_set_speed(g_fan_speed);
    } else {
        /* Silently ignoring invalid params */
        return ESP_OK;
    }
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}

void fan_init(esp_rmaker_node_t *node)
{
    /* Create a Fan device and add the relevant parameters to it */
    fan_device = esp_rmaker_fan_device_create("Fan", NULL, DEFAULT_FAN_POWER);
    esp_rmaker_device_add_cb(fan_device, fan_cb, NULL);
    esp_rmaker_device_add_param(fan_device, esp_rmaker_speed_param_create(ESP_RMAKER_DEF_SPEED_NAME, DEFAULT_FAN_SPEED));
    esp_rmaker_node_add_device(node, fan_device);
}

void fan_set_speed(int speed)
{
    if(speed == 0)
    {
        display_fan_off();
    }
    else
    {
        display_fan_spinning(speed);
    }
}

