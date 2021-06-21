/**
  Section: Included Files
 */

/* Kernel includes. */
#include "freeRTOS/include/FreeRTOS.h"
#include "freeRTOS/include/task.h"

#include <stdio.h>
#include <stdint.h>
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "platform/date.h"
#include "freeRTOS/include/semphr.h"
#include "framework/Analog/Analog.h"
#include "platform/WS2812.h"
#include "framework/Accelerometer/Accelerometer.h"

// tasks
void update_LEDs(void *p_param);

//SemaphoreHandle_t semaphore;
//SemaphoreHandle_t mutex;

/*
                         Main application
 */


//void ANALOG_RESULT(void *p_param) {
//    uint8_t buffer[64];
//    uint8_t numBytes;
//    while (1) {
//        uint16_t result = ANALOG_getResult();
//        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
//                (USBIsDeviceSuspended() == true)) {
//            continue;
//        }
//        if (USBUSARTIsTxTrfReady()) {
//            numBytes = sprintf(buffer, "Resultado %d", result);
//            putUSBUSART(buffer, numBytes);
//        }
//        CDCTxService();
//        vTaskDelay(pdMS_TO_TICKS(1000));
//    }
//}

/****VER EN EL ACCELERÓMETRO LIS3DH LOS EJES, EL Z ES NUESTRO Y?****/
// Precondición: Seba dejame quieta la placa y ponele un nivel arriba.
void accelerometer_test(void *p_param) {
    uint8_t buffer[64];
    uint8_t numBytes;
    Accel_t accel;
    uint8_t i = 1;
    while (1) {
        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
                (USBIsDeviceSuspended() == true)) {
            continue;
        }
        if (USBUSARTIsTxTrfReady()) {
            if (ACCEL_GetAccel(&accel)) {
                
                numBytes = sprintf(buffer, "\nMedida %d:\nX:%f\nY:%f\nZ:%f\n", i++, accel.Accel_X,
                        accel.Accel_Y, accel.Accel_Z);
                putUSBUSART(buffer, numBytes);
            }
        }
        CDCTxService();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    while(!ACCEL_init());
    //    semaphore = xSemaphoreCreateBinary();
    //    mutex = xSemaphoreCreateMutex();

    /* Create the tasks defined within this file. */
    //xTaskCreate(ANALOG_convert, "ANALOG", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(update_LEDs, "leds", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(accelerometer_test, "acceltst", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Finally start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

void update_LEDs(void *p_param) {
    ws2812_t color;
    float threshold_abrupt;
    float threshold_crash;
    while (1) {
        update_car_state(&color, &threshold_abrupt, &threshold_crash);
        if (color.r == 255) {
            uint8_t i;
            for (i = 0; i < 3; i++) {
                update_LEDs_array(color);
                vTaskDelay(pdMS_TO_TICKS(166));
                update_LEDs_array(BLACK);
                vTaskDelay(pdMS_TO_TICKS(166));
            }
        }
        else{
            update_LEDs_array(GREEN);
        }
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

