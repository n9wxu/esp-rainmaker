#include "switch.h"

#include <string.h>

#include <esp_log.h>

#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_types.h> 
#include <esp_rmaker_standard_params.h> 

#include "display.h"

#include "user_parameters.h"

#define TAG "Switch"

static esp_rmaker_device_t *switch_device;
bool g_switch_is_on = false;

static void push_btn_cb(void *arg)
{
    g_switch_is_on = !g_switch_is_on;
    esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(switch_device, ESP_RMAKER_PARAM_POWER),
                esp_rmaker_bool(g_switch_is_on));
}

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t switch_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
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
        g_switch_is_on = val.val.b;
        if(val.val.b)
        {
            display_switch_on(); // todo switch this to the switch icon
        }
        else
        {
            display_switch_off();
        }
    } else {
        /* Silently ignoring invalid params */
        return ESP_OK;
    }
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}

void switch_init(esp_rmaker_node_t *node)
{
    //Core2ForAWS_Button_Init();

        /* Create a Switch device and add the relevant parameters to it */
    switch_device = esp_rmaker_switch_device_create("Switch", NULL, DEFAULT_SWITCH_POWER);
    esp_rmaker_device_add_cb(switch_device, switch_cb, NULL);
    esp_rmaker_node_add_device(node, switch_device);

}

