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

#include "Button.h"

SemaphoreHandle_t semaphore_button;

void initialize_button_semaphore(){
    semaphore_button = xSemaphoreCreateBinary();
}

void give_button_semaphore(){
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(semaphore_button, &xHigherPriorityTaskWoken);
}

void take_button_semaphore(){
    xSemaphoreTake(semaphore_button, portMAX_DELAY);
}

/* *****************************************************************************
 End of File
 */
