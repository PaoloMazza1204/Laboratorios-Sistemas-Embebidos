/* ************************************************************************** */
/** car_state

  @File Name
    car_state.c

  @Summary
    Módulo para obtener el patrón de manejo del auto.
 */
/* ************************************************************************** */
#include "car_state.h"

/**
 * Devuelve un patrón de manejo según el valor del acelerómetro.
 * @param threshold_abrupt
 * Umbral para entrar en modo ABRUPTO.
 * @param threshold_crash
 * Umbral para entrar en modo CHOQUE.
 * @return 
 */
DRIVE_PATTERN get_drive_pattern(float* threshold_abrupt, float* threshold_crash) {
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

/* *****************************************************************************
 End of File
 */
