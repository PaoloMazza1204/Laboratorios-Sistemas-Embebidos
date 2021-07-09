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

#ifndef _CAR_STATE_H    /* Guard against multiple inclusion */
#define _CAR_STATE_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "WS2812.h"
#include "../framework/Analog/Analog.h"
#include "../framework/Accelerometer/Accelerometer.h"

typedef enum {
    OK,
    ABRUPT,
    CRASH
} DRIVE_PATTERN;

DRIVE_PATTERN get_drive_pattern(float* threshold_abrupt, float* threshold_crash);

#endif /* _CAR_STATE_H */

/* *****************************************************************************
 End of File
 */
