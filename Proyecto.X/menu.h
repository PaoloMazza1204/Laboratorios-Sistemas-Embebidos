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
#include "freeRTOS/include/FreeRTOS.h"
#include "freeRTOS/include/semphr.h"

void initialize_USB_semaphore();
void give_USB_semaphore();
void display_options();

#endif /* _MENU_H */

/* *****************************************************************************
 End of File
 */
