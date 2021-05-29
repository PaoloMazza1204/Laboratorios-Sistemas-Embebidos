/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC32MM0256GPM064
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
 */

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

/**
  Section: Included Files
 */
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "mcc_generated_files/rtcc.h"
#include "mcc_generated_files/pin_manager.h"
#include "utils/utils.h"
#include "platform/WS2812.h"
#include <string.h>
#include <stdio.h>
#include "platform/led_RGB.h"
#include "platform/date.h"

ut_tmrDelay_t p_timer;

void ledA_on_off() {
    if (LEDA_GetValue()) {
        if (UT_delayms(&p_timer, 400)) {
            LEDA_Toggle();
        }
    } else {
        if (UT_delayms(&p_timer, 800)) {
            LEDA_Toggle();
        }
    }
}

typedef enum {
    INIT,
    DATE,
    LED
} MODE;

MODE mode;

/*
                         Main application
 */
int main(void) {
    initialize_app_register();
    initialize_leds_RGB();
    p_timer.state = UT_TMR_DELAY_INIT;
    mode = INIT;
    // initialize the device
    SYSTEM_Initialize();
    struct tm date = get_date();
    RTCC_TimeGet(&date);
    uint8_t numBytes;
    uint8_t buffer[64];


    while (1) {
        ledA_on_off();
        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
                (USBIsDeviceSuspended() == true)) {
            continue;
        } else {
            if (USBUSARTIsTxTrfReady()) {
                numBytes = getsUSBUSART(buffer, sizeof (buffer)); //until the buffer is free.
                if ((numBytes == 1) && (mode == INIT)) {
                    if (buffer[0] == '1') {
                        numBytes = sprintf(buffer, "Ingresar fecha y hora: AA/MM/DD HH:MM:SS\n");
                        mode = DATE;
                    } else if (buffer[0] == '2') {
                        numBytes = sprintf(buffer, "Ingresar led y color: LED(1-8)-Color(0-4)\n");
                        mode = LED;
                    } else if (buffer[0] == '3') {
                        if (get_last_led() == 0) {
                            numBytes = sprintf(buffer, "No se han modificado leds\n");
                        } else {
                            uint32_t last_time = get_last_time();
                            struct tm* actual = gmtime((time_t*) & last_time); // es un puntero a la variable
                            uint8_t str_hour[20];
                            uint16_t year = actual->tm_year + 1900;
                            sprintf(str_hour, "%d/%d/%d %d:%d:%d", year, actual->tm_mon,
                                    actual->tm_mday, actual->tm_hour, actual->tm_min, actual->tm_sec);

                            numBytes = sprintf(buffer, "\nLed: %d\nColor: %d\nHora: %s\n",
                                    get_last_led(), get_last_color(), str_hour);
                        }
                    } else {
                        numBytes = 0;
                    }
                } else {
                    if ((numBytes == 17) && (mode == DATE)) {
                        if (set_date(buffer)) {
                            numBytes = sprintf(buffer, "Fecha actualizada\n");
                            mode = INIT;
                        } else {
                            numBytes = sprintf(buffer, "Fecha incorrecta\n");
                        }
                    } else if ((numBytes == 3) && (mode == LED)) {
                        if (set_ledRGB(buffer)) {
                            numBytes = sprintf(buffer, "Led prendido\n");
                            mode = INIT;
                        } else {
                            numBytes = sprintf(buffer, "Datos incorrectos\n");
                        }
                    } else if ((numBytes == 1) && (buffer[0] == 'B' || buffer[0] == 'b')) {
                        numBytes = sprintf(buffer, "Elija nueva opcion\n");
                        mode = INIT;
                    } else {
                        numBytes = 0;
                    }
                }
                putUSBUSART(buffer, numBytes);
                // mandar mensaje error
            }
        }
        //Keep trying to send data to the PC as required
        CDCTxService();


    }
    return 1;
}
/**
 End of File
 */

