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
#include "log.h"

log_register_t buffer_log[250];
uint8_t position = 0;
uint32_t id = 0;

void add_register_to_log(DRIVE_PATTERN pattern) {
    buffer_log[position].drive_pattern = pattern;
    buffer_log[position].id = ++id;
    position = (position + 1) % 250;
}

log_register_t* get_log() {
    return buffer_log;
}

uint8_t get_position() {
    return position;
}

uint32_t get_id() {
    return id;
}

/* *****************************************************************************
 End of File
 */
