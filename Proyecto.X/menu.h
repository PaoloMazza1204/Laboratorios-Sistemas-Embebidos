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

#ifndef _MENU_H    /* Guard against multiple inclusion */
#define _MENU_H

#include <stdint.h>
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

uint8_t user_interface();
uint8_t threshold_select();
bool exit_config_ADC();

#endif /* _MENU_H */

/* *****************************************************************************
 End of File
 */
