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
#include "../mcc_generated_files/usb/usb_device_cdc.h"
#include "../freeRTOS/include/FreeRTOS.h"
#include "../freeRTOS/include/semphr.h"
#include "log.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    START,
    WAITING_THRESHOLD,
    WAITING_LOG,
    WAITING_COLOR,
    THRESHOLD_ABRUPT_CONFIG,
    THRESHOLD_CRASH_CONFIG,
    LOG_TIME_CONFIG,
    COLOR_CONFIG,
    DOWNLOAD_LOG
} MENU_MODE;

void user_interface(SemaphoreHandle_t semaphore_USB);
bool cancel_config_ADC();
bool confirm_config_ADC();
void download_log(SemaphoreHandle_t semaphore_USB);
uint8_t get_log_time(SemaphoreHandle_t semaphore_USB);
bool compare_to_menu_mode(MENU_MODE mode);
void change_color();
void map_mode(uint8_t* mode, ws2812_t color);
void reset_menu_mode();

#endif /* _MENU_H */

/* *****************************************************************************
 End of File
 */
