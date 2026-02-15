#ifndef __LED_H__
#define __LED_H__

#include "pico/stdlib.h"

void led_init();
void led_set_outpu(bool outOf);
void led_blink_error(int amount);
void led_blink_selectMC(int amount);
void led_output_newMC();

#endif