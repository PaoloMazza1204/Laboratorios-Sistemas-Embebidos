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

#ifndef _BUTTON_H    /* Guard against multiple inclusion */
#define _BUTTON_H

#include "../../freeRTOS/include/FreeRTOS.h"
#include "../../freeRTOS/include/semphr.h"
#include "portmacro.h"

void initialize_button_semaphore();
void give_button_semaphore();
void take_button_semaphore();

#endif /* _BUTTON_H */

/* *****************************************************************************
 End of File
 */
