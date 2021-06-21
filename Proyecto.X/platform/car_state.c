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
#include "WS2812.h"
#include <stdbool.h>

CAR_STATE car_state = OK;
Accel_t accel;

void get_state_color(ws2812_t* color, float* threshold_abrupt, float* threshold_crash){
    // revisar acelerometro y actualizar segun umbral
    if(ACCEL_GetAccel(&accel)){
        if(accel.Accel_X < *threshold_abrupt && accel.Accel_Y < *threshold_abrupt && 
                accel.Accel_Z < *threshold_abrupt){
            *color = GREEN;
        }
        else if(accel.Accel_X < *threshold_crash && accel.Accel_Y < *threshold_crash && 
                accel.Accel_Z < *threshold_crash){
            *color = YELLOW;
        }
        else{
           *color = RED;
        }
    }
}

/* *****************************************************************************
 End of File
 */
