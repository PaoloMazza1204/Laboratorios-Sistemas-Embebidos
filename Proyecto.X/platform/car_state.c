/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "car_state.h"

Accel_t accel;

/**
 * Devuelve un patron de manejo segun el valor del acelerometro.
 * @param threshold_abrupt
 * Umbral para entrar en modo ABRUPTO.
 * @param threshold_crash
 * Umbral para entrar en modo CHOQUE.
 * @return 
 */
DRIVE_PATTERN get_state_color(float* threshold_abrupt, float* threshold_crash) {
    float accel_value;
    ACCEL_Mod(&accel_value);
    if (accel_value < *threshold_abrupt) {
        return OK;
    }
    if (accel_value < *threshold_crash) {
        return ABRUPT;
    }
    return CRASH;
}

/**
 * Devuelve la cantidad de leds a prender segun el valor analogico del ADC.
 * @return 
 */
uint8_t adc_to_LEDs() {
    uint16_t result = ANALOG_getResult();
    return (result / 128) + 1;
}

/* *****************************************************************************
 End of File
 */
