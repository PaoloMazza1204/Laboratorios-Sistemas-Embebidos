/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "WS2812.h"
#include "led_RGB.h"

// Vector con el color actual de los leds.
ws2812_t leds_RGB[8]; 

const ws2812_t *mode_colors[4] = {&DEFAULT_COLOR_OK, &DEFAULT_COLOR_ABRUPT, &DEFAULT_COLOR_CRASH, &DEFAULT_COLOR_THRESHOLD};
ws2812_t modified_mode_colors[4];

/**
 * Devuelve el color por defecto que se encuentra en una posicion dada.
 * @param position
 * Posicion en el array.
 * @return 
 */
ws2812_t get_mode_color(uint8_t position){
    return *mode_colors[position];
}

/**
 * Setea el color pasado como segundo parametro en la posicion pasada como
 * primer parametro.
 * @param position
 * Posicion en la que se insertará el color.
 * @param color
 * Color a setear.
 */
void set_mode_color(uint8_t position, ws2812_t color){
    modified_mode_colors[position] = color;
    mode_colors[position] = &modified_mode_colors[position];
}

/**
 * Inicializa los vectores.
 */
void initialize_leds_RGB() {
    uint8_t i;
    // Todos apagados.
    for (i = 0; i < 8; i++) {
        leds_RGB[i] = BLACK;
    }
}

/**
 * Actualiza una cantidad de leds RGB determinados con un color determinado.
 * @param color
 * Color a setear.
 * @param leds
 * Cantidad de leds RGB a modificar.
 */
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

/**
 * Compara dos colores y retorna True si son iguales.
 * @param color1
 * @param color2
 * @return 
 */
bool compare_colors(ws2812_t color1, ws2812_t color2){
    return(color1.r == color2.r && color1.g == color2.g && color1.b == color2.b);
}
