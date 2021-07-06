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
#include <time.h>

#include "log.h"

log_register_t buffer_log[250];
uint8_t position = 0;
uint16_t id = 0;

void add_register_to_log(struct tm date, GPSPosition_t position_GPS, DRIVE_PATTERN pattern) {
    buffer_log[position].date = mktime(&date);
    buffer_log[position].latitude = position_GPS.latitude;
    buffer_log[position].longitude = position_GPS.longitude;
    buffer_log[position].drive_pattern = (uint8_t)pattern;
    buffer_log[position].id = ++id;
    position = (position + 1) % 250;
}

log_register_t* get_log() {
    return buffer_log;
}

uint8_t get_position() {
    return position;
}

uint16_t get_id() {
    return id;
}

/* *****************************************************************************
 End of File
 */
