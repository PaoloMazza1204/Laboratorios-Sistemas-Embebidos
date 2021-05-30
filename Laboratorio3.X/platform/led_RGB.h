#ifndef _LED_RGB_H
#define _LED_RGB_H

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <stdint.h>
#include <stdbool.h>

// *****************************************************************************
// Section: Data Types
// *****************************************************************************
typedef struct
{
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

#endif /* _LED_RGB_H */
