#ifndef DISPLAY_H
#define DISPLAY_H

void display_init();
void display_fan_init();
void display_house_init();
void display_temperature_init(void);

void display_lights_off(void);
void display_lights_on(int h, int s, int v);

void display_fan_speed(int s);
void display_fan_off(void);
void display_fan_on(void);

void display_temperature(float c);

#endif
