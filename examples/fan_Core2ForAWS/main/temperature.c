#include "temperature.h"

#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_devices.h>

#include "user_parameters.h"

#include "display.h"

#include "mpu6886.h"

static esp_rmaker_device_t *temp_sensor_device;
static esp_timer_handle_t sensor_timer;

static float g_temperature = DEFAULT_TEMPERATURE;

static void temperature_sensor_update(void *priv)
{
    // TODO: Configure this to read the temperature sensor
    MPU6886_GetTempData(&g_temperature);

    display_temperature(g_temperature);

    esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(temp_sensor_device, ESP_RMAKER_PARAM_TEMPERATURE),
                esp_rmaker_float(g_temperature));
}

void temperature_init(esp_rmaker_node_t *node)
{
    MPU6886_Init();
    MPU6886_GetTempData(&g_temperature);
    /* Create a Temperature Sensor device and add the relevant parameters to it */
    temp_sensor_device = esp_rmaker_temp_sensor_device_create("Temperature Sensor", NULL, DEFAULT_TEMPERATURE);
    esp_rmaker_node_add_device(node, temp_sensor_device);

    /* Start the update task to send the temperature every so often */
    g_temperature = DEFAULT_TEMPERATURE;
    esp_timer_create_args_t sensor_timer_conf = {
        .callback = temperature_sensor_update,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "temperature_sensor_update_tm"
    };
    if (esp_timer_create(&sensor_timer_conf, &sensor_timer) == ESP_OK) {
        esp_timer_start_periodic(sensor_timer, REPORTING_PERIOD * 1000000U);
    }

    display_temperature_init();
    display_temperature(g_temperature);
}

