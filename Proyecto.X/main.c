/**
  Section: Included Files
 */
/* TO-DO:
 * Hacer los logs
 * Hacer el extra (colores)
 * 
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
#include "framework/Button/Button.h"
#include "menu.h"
#include "platform/led_RGB.h"

#define DEFAULT_THRESHOLD 1.5f
#define DEFAULT_LOG_TIME 10
#define DEFAULT_COLOR_OK GREEN
#define DEFAULT_COLOR_ABRUPT YELLOW
#define DEFAULT_COLOR_CRASH RED
#define DEFAULT_COLOR_THRESHOLD BLUE

// tasks
void update_LEDs(void *p_param);
void config_ACCEL(void *p_param);
void analog_result(void *p_param);
void button_check(void *p_param);
void check_USB(void *p_param);
void log_update(void *p_param);
void enable_log_update(void *p_param);

// semaforos
SemaphoreHandle_t semaphore_ACCEL;
SemaphoreHandle_t semaphore_config_adc;
SemaphoreHandle_t semaphore_USB;
SemaphoreHandle_t semaphore_log;
//SemaphoreHandle_t mutex;

float threshold_abrupt = DEFAULT_THRESHOLD;
float threshold_crash = DEFAULT_THRESHOLD;
uint8_t log_time = DEFAULT_LOG_TIME;
const ws2812_t *color_ok = &DEFAULT_COLOR_OK;
const ws2812_t *color_abrupt = &DEFAULT_COLOR_ABRUPT;
const ws2812_t *color_crash = &DEFAULT_COLOR_CRASH;
const ws2812_t *color_threshold = &DEFAULT_COLOR_THRESHOLD;

uint8_t threshold_to_change; // umbral a configurar

/*
                         Main application
 */


//void delay_CDCTxService(void *p_param){
//    while (1) {
//        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
//                (USBIsDeviceSuspended() == true)) {
//            continue;
//        }
//        CDCTxService();
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }
//}

int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    initialize_button_semaphore();
    semaphore_ACCEL = xSemaphoreCreateBinary();
    semaphore_config_adc = xSemaphoreCreateBinary();
    semaphore_USB = xSemaphoreCreateBinary();
    semaphore_log = xSemaphoreCreateBinary();
    //    mutex = xSemaphoreCreateMutex();
    while (!ACCEL_init());

    /* Create the tasks defined within this file. */
    //xTaskCreate(ANALOG_RESULT, "ANALOG", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(ANALOG_convert, "Convert", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(analog_result, "Result", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(button_check, "Button", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(check_USB, "USB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(update_LEDs, "leds", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(log_update, "log", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(enable_log_update, "enable", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(config_ACCEL, "ACCEL", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    //xTaskCreate(delay_CDCTxService, "delay", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    /* Finally start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

void config_ACCEL(void *p_param) {
    while (!ACCEL_init()) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    xSemaphoreGive(semaphore_ACCEL);
}

void update_LEDs(void *p_param) {
    ws2812_t color;
    ws2812_t last_color = *color_ok;
    uint8_t color_result;
    for (;;) {
        //xSemaphoreTake(semaphore_ACCEL, portMAX_DELAY);
        color_result = get_state_color(&threshold_abrupt, &threshold_crash); // modificar esto
        color = color_result == 0 ? *color_ok : color_result == 1 ? *color_abrupt : *color_crash;
        //xSemaphoreGive(semaphore_ACCEL);
        if (compare_colors(color, *color_ok)) {
            update_LEDs_array(*color_ok, 8);
        } else {
            if ((compare_colors(color, *color_abrupt) && compare_colors(last_color, *color_ok)) ||
                    (compare_colors(color, *color_crash) && (!compare_colors(last_color, *color_crash)))) {
                xSemaphoreGive(semaphore_log);
            }
            uint8_t i;
            for (i = 0; i < 3; i++) {
                update_LEDs_array(color, 8);
                vTaskDelay(pdMS_TO_TICKS(166));
                update_LEDs_array(BLACK, 8);
                vTaskDelay(pdMS_TO_TICKS(166));
            }
        }
        last_color = color;
    }
}

void analog_result(void *p_param) {
    TaskHandle_t handle_convert = NULL;
    for (;;) {
        xSemaphoreTake(semaphore_config_adc, portMAX_DELAY);
        xTaskCreate(ANALOG_convert, "Convert", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &handle_convert);
        uint8_t leds = 0;
        while (!exit_config_ADC()) {
            uint8_t leds_actual = adc_to_LEDs();
            if (leds_actual != leds) {
                if (((threshold_to_change == 1) && ((leds_actual - 1) + DEFAULT_THRESHOLD <= threshold_crash)) ||
                        ((threshold_to_change == 2) && ((leds_actual - 1) + DEFAULT_THRESHOLD >= threshold_abrupt))) { // brusco)
                    update_LEDs_array(BLUE, leds_actual);
                    leds = leds_actual;
                }
            }
        }
        if (threshold_to_change == 1) {
            threshold_abrupt = DEFAULT_THRESHOLD + ((leds - 1));
        } else {
            threshold_crash = DEFAULT_THRESHOLD + ((leds - 1));
        }
        vTaskDelete(handle_convert);
    }
}

void check_USB(void *p_param) {
    for (;;) {
        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
                (USBIsDeviceSuspended() == true)) {
            continue;
        }
        if (USBUSARTIsTxTrfReady()) {
            xSemaphoreGive(semaphore_USB);
        }
        CDCTxService();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void button_check(void *p_param) {
    for (;;) {
        // este semaforo se habilita cuando ocuure una interrupcion 
        // al apretar el boton.
        take_button_semaphore();
        // este semaforo se habilita cuando el USB esta configurado.
        xSemaphoreTake(semaphore_USB, portMAX_DELAY);
        uint8_t result = user_interface();
        if (result < 3) {
            threshold_to_change = result;
            xSemaphoreGive(semaphore_config_adc);
        } else if (result == 3) {
            //xSemaphoreGive(semaphore_LOG DATOS);
        }
    }
}

void log_update(void* p_param) {
    for (;;) {
        xSemaphoreTake(semaphore_log, portMAX_DELAY);
        LEDA_Toggle();
        // hacer cosas
    }
}

void enable_log_update(void* p_param) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(log_time * 1000));
        xSemaphoreGive(semaphore_log);
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

