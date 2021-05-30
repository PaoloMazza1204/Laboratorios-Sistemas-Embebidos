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
#include <stdio.h> // Usaremos sprintf().
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "mcc_generated_files/rtcc.h"
#include "mcc_generated_files/pin_manager.h"
#include "utils/utils.h"
#include "platform/WS2812.h"
#include "platform/led_RGB.h"
#include "platform/date.h"

ut_tmrDelay_t p_timer;

/* Encendido y apagado del LEDA. */
void ledA_on_off()
{
    if (LEDA_GetValue())
    {
        if (UT_delayms(&p_timer, 400))
        {
            LEDA_Toggle();
        }
    }
    else
    {
        if (UT_delayms(&p_timer, 800))
        {
            LEDA_Toggle();
        }
    }
}

// Distintos modos del programa.
typedef enum
{
    INIT, // Pronto para recibir un modo.
    DATE, // Esperando ingreso de fecha.
    LED   // Esperando ingreso de led a prender y su color.
} MODE;

MODE mode;

/*
                         Main application
 */
int main(void)
{
    // Inicialización de variables.
    initialize_app_register();
    initialize_leds_RGB();
    p_timer.state = UT_TMR_DELAY_INIT;
    mode = INIT;
    SYSTEM_Initialize();
    struct tm date = get_date();
    RTCC_TimeGet(&date);
    uint8_t numBytes;
    uint8_t buffer[64];

    while (1)
    {
        ledA_on_off(); // LEDA.
        // Manejo de interfaz de usuario.
        if ((USBGetDeviceState() < CONFIGURED_STATE) ||
                (USBIsDeviceSuspended() == true))
        {
            continue;
        }
        else
        {
            if (USBUSARTIsTxTrfReady())
            {
                numBytes = getsUSBUSART(buffer, sizeof (buffer));
                
                // Si estamos esperando un modo e ingresa un caracter.
                if ((numBytes == 1) && (mode == INIT))
                {
                    // Si es el modo 1 (Funcionalidad 1).
                    if (buffer[0] == '1')
                    {
                        // Indicamos cómo esperamos el ingreso de la fecha.
                        numBytes = sprintf(buffer, "Ingresar fecha y hora: AA/MM/DD HH:MM:SS\n");
                        mode = DATE;
                    }
                    // Si es el modo 2 (Funcionalidad 2).
                    else if (buffer[0] == '2')
                    {
                        // Indicamos cómo esperamos el ingreso del led y color.
                        numBytes = sprintf(buffer, "Ingresar led y color: LED(1-8)-Color(0-4)\n");
                        mode = LED;
                    }
                    // Si es el modo 3 (Funcionalidad 3).
                    else if (buffer[0] == '3')
                    {
                        // Si no se han modificado leds.
                        if (get_last_led() == 0)
                        {
                            // Indicamos que no se han modificado leds.
                            numBytes = sprintf(buffer, "No se han modificado leds\n");
                        }
                        // Si se han modificado leds.
                        else
                        {
                            uint32_t last_time = get_last_time();
                            struct tm* actual = gmtime((time_t*) & last_time);
                            uint8_t str_hour[20]; // String que contendrá el mensaje.
                            uint16_t year = actual->tm_year + 1900;
                            /* Fix posible para el tema del mes:
                             * uint8_t month = actual->tm_mon+1;
                             * REEMPLAZAR actual->tm_mon POR month en el sprintf
                             * (Segundo parámetro luego del formato de string).
                             *  */
                            // Formato del momento de la última modificación del led.
                            sprintf(str_hour, "%d/%d/%d %d:%d:%d", year, actual->tm_mon,
                                    actual->tm_mday, actual->tm_hour, actual->tm_min, actual->tm_sec);
                            // Indicamos última modificación.
                            numBytes = sprintf(buffer, "\nLed: %d\nColor: %d\nHora: %s\n",
                                    get_last_led(), get_last_color(), str_hour);
                        }
                    }
                    // Si la entrada es algo que no esperamos.
                    else
                    {
                        // No mandamos nada.
                        numBytes = 0;
                    }
                }
                // Estamos esperando input para una funcionalidad específica.
                else
                {
                    // Si el modo es el ingreso de una fecha y estamos en el respectivo modo.
                    if ((numBytes == 17) && (mode == DATE))
                    {
                        // Si pasa todos los controles la establecemos.
                        if (set_date(buffer))
                        {
                            // Indicamos su correctitud.
                            numBytes = sprintf(buffer, "Fecha actualizada\n");
                            mode = INIT;
                        }
                        // Si no pasa todos los controles.
                        else
                        {
                            // Indicamos su incorrectitud.
                            numBytes = sprintf(buffer, "Fecha incorrecta\n");
                        }
                    }
                    // Si el modo es la modificación de un led y estamos en el respectivo modo.
                    else if ((numBytes == 3) && (mode == LED))
                    {
                        // Si pasa los controles encendemos el led del color indicado.
                        if (set_ledRGB(buffer))
                        {
                            // Indicamos que encendimos el led.
                            numBytes = sprintf(buffer, "Led prendido\n");
                            mode = INIT;
                        }
                        // Si no pasa todos los controles.
                        else
                        {
                            // Indicamos que no se cumple con los controles.
                            numBytes = sprintf(buffer, "Datos incorrectos\n");
                        }
                    }
                    // Si el usuario desea regresar para hacer uso de otra funcionalidad.
                    else if ((numBytes == 1) && (buffer[0] == 'B' || buffer[0] == 'b'))
                    {
                        // Indicamos que puede ingresar otra opción.
                        numBytes = sprintf(buffer, "Elija nueva opcion\n");
                        mode = INIT;
                    }
                    // Si no ingresa algo esperado.
                    else
                    {
                        // No mandamos nada.
                        numBytes = 0;
                    }
                }
                // Mandamos el mensaje necesario al buffer, si corresponde.
                putUSBUSART(buffer, numBytes);
            }
        }
        // Enviamos el mensaje.
        CDCTxService();
    }
    return 1;
}
