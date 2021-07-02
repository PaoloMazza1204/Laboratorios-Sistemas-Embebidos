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

void add_register_to_log(DRIVE_PATTERN pattern){
    buffer_log[position].drive_pattern = pattern;
    buffer_log[position].id = position;
    position = (position+1)%250; 
}

/* *****************************************************************************
 End of File
 */
