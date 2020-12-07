#ifndef LIGHT_H
#define LIGHT_H

#include <stdbool.h>
#include <stdint.h>

#include <esp_rmaker_core.h>

void light_init(esp_rmaker_node_t *node);

void light_set_on(bool on);
void light_set_hue(int hue);
void light_set_saturation(int hue);
void light_set_brightness(int brightness);

bool light_is_on(void);
int light_get_hue(void);
int light_get_saturation(void);
int light_get_brightness(void);

#endif
