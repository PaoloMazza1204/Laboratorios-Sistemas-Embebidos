/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _LOG_H    /* Guard against multiple inclusion */
#define _LOG_H

#include <stdio.h>
#include "car_state.h"
#include "../mcc_generated_files/usb/usb_device_cdc.h"
#include "../freeRTOS/include/FreeRTOS.h"
#include "../freeRTOS/include/semphr.h"
#include <string.h>
#include <time.h>
#include "../framework/GPS/GPS.h"

typedef struct {
    uint16_t id;
    time_t date;
    float latitude;
    float longitude;
    uint8_t drive_pattern; // se puede usar como uint8_t
} log_register_t;

void add_register_to_log(struct tm date, GPSPosition_t position_GPS, DRIVE_PATTERN pattern);
log_register_t* get_log();
uint8_t get_position();
uint16_t get_id();

#endif /* _LOG_H */

/* *****************************************************************************
 End of File
 */
