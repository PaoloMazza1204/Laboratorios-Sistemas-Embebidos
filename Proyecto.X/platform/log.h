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

typedef struct {
    uint8_t id;
    DRIVE_PATTERN drive_pattern;
} log_register_t;

void add_register_to_log(DRIVE_PATTERN pattern);

#endif /* _LOG_H */

/* *****************************************************************************
 End of File
 */
