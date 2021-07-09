/* ************************************************************************** */
/** Button

  @File Name
    Button.c

  @Summary
    Contiene un semáforo para el manejo del botón.
 */
/* ************************************************************************** */

#include "Button.h"

SemaphoreHandle_t semaphore_button;

/**
 * Crea el semáforo para el botón.
 */
void initialize_button_semaphore(){
    semaphore_button = xSemaphoreCreateBinary();
}

/**
 * Libera el semáforo del botón, se utiliza en la interrupción.
 */
void give_button_semaphore(){
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(semaphore_button, &xHigherPriorityTaskWoken);
}

/**
 * Toma el semáforo del botón.
 */
void take_button_semaphore(){
    xSemaphoreTake(semaphore_button, portMAX_DELAY);
}

/* *****************************************************************************
 End of File
 */
