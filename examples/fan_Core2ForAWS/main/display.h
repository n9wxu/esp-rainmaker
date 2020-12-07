#ifndef DISPLAY_H
#define DISPLAY_H

void display_init();
void display_fan_init();

void display_lights_off(void);
void display_lights_on(int h, int s, int v);

void display_fan_speed(int s);
void display_fan_off(void);
void display_fan_on(void);

void display_switch_on(void);
void display_switch_off(void);

void display_temperature(int c);

#endif
