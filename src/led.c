#include "led.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define WS2812_PIN 16
#define WS2812_FREQ 800000

static uint sm;
static uint offset;

void ws2812_put_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
    pio_sm_put_blocking(pio1, sm, grb << 8);
    sleep_us(60);
}

void led_init()
{
    offset = pio_add_program(pio1, &ws2812_program);
    sm = pio_claim_unused_sm(pio1, true);
    ws2812_program_init(pio1, sm, offset, WS2812_PIN, WS2812_FREQ, false);
    ws2812_put_rgb(0, 0, 0);
}

void led_set_outpu(bool outOf) {
    if(outOf) {
		ws2812_put_rgb(255, 96, 0);	// Orange
		sleep_ms(25);
	} else {
		ws2812_put_rgb(0, 0, 0);
	}
}

void led_blink_error(int amount) {
	ws2812_put_rgb(0, 0, 0);
    sleep_ms(1000);
    ws2812_put_rgb(255, 0, 0);
	sleep_ms(2000);
	for(int i = 0; i < amount; ++i) {
		ws2812_put_rgb(0, 0, 0);
		sleep_ms(500);
		ws2812_put_rgb(255, 0, 0);
		sleep_ms(500);
	}
    ws2812_put_rgb(0, 0, 0);
}

void led_blink_selectMC(int amount) {
	ws2812_put_rgb(0, 0, 255); //Blue
	sleep_ms(1000);
	for(int i = 0; i < amount; ++i) {
		ws2812_put_rgb(0, 0, 0);
		sleep_ms(500);
		ws2812_put_rgb(0, 0, 255); //Blue
		sleep_ms(500);
	}
    sleep_ms(500);
    ws2812_put_rgb(0, 0, 0);
}

void led_output_newMC() {
    ws2812_put_rgb(0, 0, 0);
    sleep_ms(200);
    for(int i = 0; i < 3; i++) {
    	ws2812_put_rgb(0, 255, 0);	// Green
    	sleep_ms(200);
    	ws2812_put_rgb(0, 0, 0);
    	sleep_ms(200);
	}	
}