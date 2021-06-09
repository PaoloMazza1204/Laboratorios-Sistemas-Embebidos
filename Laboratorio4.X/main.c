/******************************************************************************
 * ANÁLISIS ÁRBOL DEL PROYECTO (PARTE 1 B)                                    *
 * En Header Files tenemos el mismo arbol de proyecto que en el lab anterior, *
 * ademas, se incluye el archivo FreeRTOSConfig.h y la carpeta freeRTOS donde *
 * tenemos varios .h pertenecientes al sistema operativo:
 * - croutine.h posee funcones         
 * - FreeRTOS.h
 * - list.h
 * - portable.h
 * - projdefs.h
 * - queue.h
 * - semphr.h
 * - task.h
 * - timers.h                   *
 * En Source Files tambien tenemos el mismo arbol de proyecto que en el lab 3,*
 * ademas, tenemos la carpeta de freeRTOS con los .c correspondientes a la    *
 * carpeta freeRTOS de Header Files.                                          *
 ******************************************************************************/

/**
  Section: Included Files
 */

/* Kernel includes. */
#include "freeRTOS/include/FreeRTOS.h"
#include "freeRTOS/include/task.h"

#include <stdio.h>
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "platform/date.h"
#include "freeRTOS/include/semphr.h"

// tasks
void blinkLED(void *p_param);
void userInterface(void* p_param);
void checkUSB(void* p_param);

typedef enum {
    WAITING, // Esperando mensaje inicial.
    INIT, // Pronto para recibir un modo.
    DATE, // Esperando ingreso de fecha.
    LED // Esperando ingreso de led a prender y su color.
} MODE;

SemaphoreHandle_t semaphore;

uint8_t displayOptions(uint8_t* buffer, MODE* mode);
void functionalities(uint8_t* buffer, MODE* mode, uint8_t* numBytes);

/*
                         Main application
 */
int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    semaphore = xSemaphoreCreateBinary();

    /* Create the tasks defined within this file. */
    xTaskCreate(blinkLED, "task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(userInterface, "task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(checkUSB, "task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Finally start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

void blinkLED(void *p_param) {
    for (;;) {
        LEDA_SetLow();
        vTaskDelay(pdMS_TO_TICKS(800));
        LEDA_SetHigh();
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

void checkUSB(void *p_param) {
    // CREAR SEMAFORO QUE ARRANQUE EN 1 
    for (;;) {
        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
                (USBIsDeviceSuspended() == true)) {
            continue;
        } else if (USBUSARTIsTxTrfReady()) {
            xSemaphoreGive(semaphore);
        }
        CDCTxService();
    }
}

void userInterface(void *p_param) {
    uint8_t numBytes;
    uint8_t buffer[64];
    MODE mode = WAITING;
    initialize_app_register();
    initialize_leds_RGB();
    struct tm date = get_date();
    RTCC_TimeGet(&date);

    for (;;) {
        //        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
        //                (USBIsDeviceSuspended() == true)) {
        //            continue;
        //        } else {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        numBytes = getsUSBUSART(buffer, sizeof (buffer));

        if ((numBytes > 0) && (mode == WAITING)) {
            numBytes = sprintf(buffer, "\n1-Ingresar fecha\n2-Prender led\n3-Consultar modificación\n");
            mode = INIT;
        }// Si estamos esperando un modo e ingresa un caracter.
        else if ((numBytes == 1) && (mode == INIT)) {
            numBytes = displayOptions(buffer, &mode);
        }// Estamos esperando input para una funcionalidad específica.
        else {
            functionalities(buffer, &mode, &numBytes);
        }
        // Mandamos el mensaje necesario al buffer, si corresponde.
        putUSBUSART(buffer, numBytes);
// HACER UN RELEASE DEL SEMAFORO NUEVO PARA QUE PUEDA SER USADO DE NUEVO POR checkUSB
        //        }
        //        CDCTxService();
    }
}

void functionalities(uint8_t* buffer, MODE* mode, uint8_t* numBytes) {
    // Si el modo es el ingreso de una fecha y estamos en el respectivo modo.
    if ((*numBytes == 17) && (*mode == DATE)) {
        // Si pasa todos los controles la establecemos.
        if (set_date(buffer)) {
            // Indicamos su correctitud.
            *numBytes = sprintf(buffer, "Fecha actualizada\n");
            *mode = INIT;
        }// Si no pasa todos los controles.
        else {
            // Indicamos su incorrectitud.
            *numBytes = sprintf(buffer, "Fecha incorrecta\n");
        }
    }// Si el modo es la modificación de un led y estamos en el respectivo modo.
    else if ((*numBytes == 3) && (*mode == LED)) {
        // Si pasa los controles encendemos el led del color indicado.
        if (set_ledRGB(buffer)) {
            // Indicamos que encendimos el led.
            *numBytes = sprintf(buffer, "Led prendido\n");
            *mode = INIT;
        }// Si no pasa todos los controles.
        else {
            // Indicamos que no se cumple con los controles.
            *numBytes = sprintf(buffer, "Datos incorrectos\n");
        }
    }// Si el usuario desea regresar para hacer uso de otra funcionalidad.
    else if ((*numBytes == 1) && (buffer[0] == 'B' || buffer[0] == 'b')) {
        // Indicamos que puede ingresar otra opción.
        *numBytes = sprintf(buffer, "Elija nueva opción\n");
        *mode = INIT;
    }// Si no ingresa algo esperado.
    else {
        // No mandamos nada.
        *numBytes = 0;
    }
}

uint8_t displayOptions(uint8_t* buffer, MODE* mode) {

    switch (buffer[0]) {
        case '1':
            // Indicamos cómo esperamos el ingreso de la fecha.
            *mode = DATE;
            return sprintf(buffer, "Ingresar fecha y hora: AA/MM/DD HH:MM:SS\n");
        case '2':
            // Indicamos cómo esperamos el ingreso del led y color.
            *mode = LED;
            return sprintf(buffer, "Ingresar led y color: LED(1-8)-Color(0-4)\n");
        case '3':
            // Si no se han modificado leds.
            if (get_last_led() == 0) {
                // Indicamos que no se han modificado leds.
                return sprintf(buffer, "No se han modificado leds\n");
            }// Si se han modificado leds.
            else {
                uint32_t last_time = get_last_time();
                struct tm* actual = gmtime((time_t*) & last_time);
                uint8_t str_hour[20]; // String que contendrá el mensaje.
                uint16_t year = actual->tm_year + 1900;
                uint8_t month = actual->tm_mon + 1;
                // Formato del momento de la última modificación del led.
                sprintf(str_hour, "%d/%d/%d %d:%d:%d", year, month,
                        actual->tm_mday, actual->tm_hour, actual->tm_min, actual->tm_sec);
                // Indicamos última modificación.
                return sprintf(buffer, "\nLed: %d\nColor: %d\nHora: %s\n",
                        get_last_led(), get_last_color(), str_hour);
            }
        default:
            // Si la entrada es algo que no esperamos.
            // No mandamos nada.
            return 0;
    }
}

void vApplicationMallocFailedHook(void) {
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for (;;);
}

/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    (void) pcTaskName;
    (void) pxTask;

    /* Run time task stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is 
    called if a task stack overflow is detected.  Note the system/interrupt
    stack is not checked. */
    taskDISABLE_INTERRUPTS();
    for (;;);
}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void) {
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()). */
}

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void vAssertCalled(const char * pcFile, unsigned long ulLine) {
    volatile unsigned long ul = 0;

    (void) pcFile;
    (void) ulLine;

    __asm volatile( "di");
    {
        /* Set ul to a non-zero value using the debugger to step out of this
        function. */
        while (ul == 0) {
            portNOP();
        }
    }
    __asm volatile( "ei");
}

/**
 End of File
 */

