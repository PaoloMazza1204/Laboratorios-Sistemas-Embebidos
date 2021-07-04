#ifndef _LED_RGB_H
#define _LED_RGB_H

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <stdint.h>
#include <stdbool.h>

#include "WS2812.h"

// *****************************************************************************
// Section: Data Types
// *****************************************************************************

#define DEFAULT_COLOR_OK GREEN
#define DEFAULT_COLOR_ABRUPT YELLOW
#define DEFAULT_COLOR_CRASH RED
#define DEFAULT_COLOR_THRESHOLD BLUE

#define COLOR_OK_POSITION 0
#define COLOR_ABRUPT_POSITION 1
#define COLOR_CRASH_POSITION 2
#define COLOR_THRESHOLD_POSITION 3

typedef struct {
    uint8_t led;
    uint8_t color;
    uint32_t time;
} app_register_t;

// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
uint8_t map_led_RGB(uint8_t led);

bool set_ledRGB(uint8_t* buffer);

void initialize_leds_RGB();

void initialize_app_register();

uint8_t get_last_led();

uint8_t get_last_color();

uint32_t get_last_time();

void set_last_led(uint8_t led);

void set_last_color(uint8_t color);

void set_last_time(uint32_t time);

bool compare_colors(ws2812_t color1, ws2812_t color2);

ws2812_t get_mode_color(uint8_t position);

void set_mode_color(uint8_t position, ws2812_t color);

#endif /* _LED_RGB_H */
