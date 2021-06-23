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
#include "menu.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include <stdio.h>

uint8_t buffer[64];
uint8_t numBytes;
SemaphoreHandle_t semaphore_USB;

void initialize_USB_semaphore() {
    semaphore_USB = xSemaphoreCreateBinary();
}

void give_USB_semaphore() {
    xSemaphoreGive(semaphore_USB);
}

void display_options() {

    // este semaforo se habilita cuando el USB esta configurado. 
    xSemaphoreTake(semaphore_USB, portMAX_DELAY);

    // check modo
    numBytes = sprintf(buffer, "1-Ajustar umbrales\n2-Log de datos\n");
    //mode = INIT;
    putUSBUSART(buffer, numBytes);
}

/* *****************************************************************************
 End of File
 */
