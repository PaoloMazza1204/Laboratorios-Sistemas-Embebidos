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

DRIVE_PATTERN get_state_color(float* threshold_abrupt, float* threshold_crash) {
    // revisar acelerometro y actualizar segun umbral
    float accel_value;
    ACCEL_Mod(&accel_value);
    if (accel_value < *threshold_abrupt) {
        return OK;
    }
    if (accel_value < *threshold_crash) {
        return ABRUPT;
    }
    return CRASH;
    //    if(ACCEL_GetAccel(&accel)){
    //        if(fabs(accel.Accel_X) < *threshold_abrupt && fabs(accel.Accel_Y) < *threshold_abrupt && 
    //                fabs(accel.Accel_Z - 1.0) < *threshold_abrupt){
    //            return OK;
    //        }
    //        else if(fabs(accel.Accel_X) < *threshold_crash && fabs(accel.Accel_Y) < *threshold_crash && 
    //                fabs(accel.Accel_Z - 1.0) < *threshold_crash){
    //            return ABRUPT;
    //        }
    //        else{
    //            return CRASH;
    //        }
    //    }
}

uint8_t adc_to_LEDs() {
    uint16_t result = ANALOG_getResult();
    return (result / 128) + 1;
}

/* *****************************************************************************
 End of File
 */
