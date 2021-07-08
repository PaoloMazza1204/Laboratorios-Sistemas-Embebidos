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

/**
 * Crea el semaforo para el boton.
 */
void initialize_button_semaphore(){
    semaphore_button = xSemaphoreCreateBinary();
}

/**
 * Libera el semaforo del boton, se utiliza en la interrupcion.
 */
void give_button_semaphore(){
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(semaphore_button, &xHigherPriorityTaskWoken);
}

/**
 * Toma el semaforo del boton.
 */
void take_button_semaphore(){
    xSemaphoreTake(semaphore_button, portMAX_DELAY);
}

/* *****************************************************************************
 End of File
 */
