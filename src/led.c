#include "led.h"
#include "config.h"
#include "hardware/gpio.h"

void led_init()
{
    gpio_init(PICO_LED_PIN);
    gpio_set_dir(PICO_LED_PIN, GPIO_OUT);
    gpio_put(PICO_LED_PIN, false);
}

void led_set_outpu(bool outOf) {
	gpio_put(PICO_LED_PIN, outOf);
}

void led_blink_error(int amount) {
	gpio_put(PICO_LED_PIN, false);
    sleep_ms(1000);
    gpio_put(PICO_LED_PIN, true);
	sleep_ms(2000);
	for(int i = 0; i < amount; ++i) {
		gpio_put(PICO_LED_PIN, false);
		sleep_ms(500);
		gpio_put(PICO_LED_PIN, true);
		sleep_ms(500);
	}
    gpio_put(PICO_LED_PIN, false);
}

void led_blink_selectMC(int amount) {
	gpio_put(PICO_LED_PIN, true);
	sleep_ms(1000);
	for(int i = 0; i < amount; ++i) {
		gpio_put(PICO_LED_PIN, false);
		sleep_ms(500);
		gpio_put(PICO_LED_PIN, true);
		sleep_ms(500);
	}
    sleep_ms(500);
    gpio_put(PICO_LED_PIN, false);
}

void led_output_newMC() {
    gpio_put(PICO_LED_PIN, false);
    sleep_ms(100);
    for(int i = 0; i < 3; i++) {
    	gpio_put(PICO_LED_PIN, true);
    	sleep_ms(100);
    	gpio_put(PICO_LED_PIN, false);
    	sleep_ms(100);
	}	
}