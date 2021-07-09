/* ************************************************************************** */
/** Button

  @File Name
    Button.c

  @Summary
    Contiene un sem�foro para el manejo del bot�n.
 */
/* ************************************************************************** */

#include "Button.h"

SemaphoreHandle_t semaphore_button;

/**
 * Crea el sem�foro para el bot�n.
 */
void initialize_button_semaphore(){
    semaphore_button = xSemaphoreCreateBinary();
}

/**
 * Libera el sem�foro del bot�n, se utiliza en la interrupci�n.
 */
void give_button_semaphore(){
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(semaphore_button, &xHigherPriorityTaskWoken);
}

/**
 * Toma el sem�foro del bot�n.
 */
void take_button_semaphore(){
    xSemaphoreTake(semaphore_button, portMAX_DELAY);
}

/* *****************************************************************************
 End of File
 */
