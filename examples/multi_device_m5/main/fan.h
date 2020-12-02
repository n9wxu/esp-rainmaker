#ifndef FAN_H
#define FAN_H

#include <esp_rmaker_core.h>

void fan_init(esp_rmaker_node_t *node);
void fan_set_speed(int speed);

#endif