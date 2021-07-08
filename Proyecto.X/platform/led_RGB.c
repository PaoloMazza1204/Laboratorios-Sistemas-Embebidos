/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "WS2812.h"
#include "led_RGB.h"
#include "date.h"

//app_register_t app_register;
// Vector con el color actual de los leds.
ws2812_t leds_RGB[8]; 
// Vector con los posibles colores de un led.
ws2812_t colors_RGB[6]; 

const ws2812_t *mode_colors[4] = {&DEFAULT_COLOR_OK, &DEFAULT_COLOR_ABRUPT, &DEFAULT_COLOR_CRASH, &DEFAULT_COLOR_THRESHOLD};
ws2812_t modified_mode_colors[4];

/**
 * 
 * @param position
 * @return 
 */
ws2812_t get_mode_color(uint8_t position){
    return *mode_colors[position];
}

void set_mode_color(uint8_t position, ws2812_t color){
    modified_mode_colors[position] = color;
    mode_colors[position] = &modified_mode_colors[position];
}

/* Mapeo de los leds RGB para la placa. */
uint8_t map_led_RGB(uint8_t led) {
    switch (led) {
        case(6):
            return 1;
        case(4):
            return 2;
        case(1):
            return 3;
        case(2):
            return 4;
        case(3):
            return 5;
        case(5):
            return 6;
        case(8):
            return 7;
        case(7):
            return 8;
    }
    return 0;
}

///* Si el contenido del buffer es válido establece el app_register. */
//bool set_ledRGB(uint8_t* buffer) {
//    uint8_t* initial = buffer;
//    uint8_t led = *(buffer++) - '0';
//    buffer++;
//    uint8_t color = *(buffer++) - '0';
//    buffer = initial;
//
//    struct tm date = get_date();
//    if ((led > 0 && led < 9) && (color >= 0 && color < 5) && (RTCC_TimeGet(&date))) {
//        leds_RGB[map_led_RGB(led) - 1] = colors_RGB[color];
//        WS2812_send(leds_RGB, 8);
//        app_register.led = led;
//        app_register.color = color;
//        app_register.time = mktime(&date);
//        return true;
//    }
//    return false;
//}

/* Inicializa los vectores. */
void initialize_leds_RGB() {
    uint8_t i;
    // Todos apagados.
    for (i = 0; i < 8; i++) {
        leds_RGB[i] = BLACK;
    }
    // Posibles colores.
//    colors_RGB[0] = WHITE;
//    colors_RGB[1] = RED;
//    colors_RGB[2] = BLUE;
//    colors_RGB[3] = GREEN;
//    colors_RGB[4] = BLACK;
//    colors_RGB[5] = YELLOW;
}

///* Inicializa app_register. */
//void initialize_app_register() {
//    app_register.color = 4;
//    app_register.led = 0;
//    app_register.time = 0;
//}
//
///* Devuelve el último led modificado. */
//uint8_t get_last_led() {
//    return app_register.led;
//}
//
///* Devuelve el color del último led modificado. */
//uint8_t get_last_color() {
//    return app_register.color;
//}
//
///* Devuelve el momento del último led modificado. */
//uint32_t get_last_time() {
//    return app_register.time;
//}
//
///* Si led es válido establece el led del app_register. */
//void set_last_led(uint8_t led) {
//    if (led < 9 && led > 0) {
//        app_register.led = led;
//    }
//}
//
///* Si el color es válido establece el color del app_register. */
//void set_last_color(uint8_t color) {
//    if (color < 5) {
//        app_register.color = color;
//    }
//}
//
///* Establece el tiempo del app_register. */
//void set_last_time(uint32_t time) {
//    app_register.time = time;
//}

void update_LEDs_array(ws2812_t color, uint8_t leds) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        if (i < leds) {
            leds_RGB[i] = color;
        }
        else{
            leds_RGB[i] = BLACK;
        }
    }
    WS2812_send(leds_RGB, 8);
}

bool compare_colors(ws2812_t color1, ws2812_t color2){
    return(color1.r == color2.r && color1.g == color2.g && color1.b == color2.b);
}
