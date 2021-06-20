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

CAR_STATE car_state = OK;

void update_car_state(ws2812_t* color){
    // revisar acelerometro y actualizar segun umbral
    *color = YELLOW;
}

/* *****************************************************************************
 End of File
 */
