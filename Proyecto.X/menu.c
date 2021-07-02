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

#define EXIT_CONFIG_ADC "Exit"

uint8_t buffer[64];
uint8_t numBytes;

uint8_t user_interface() {
    // check modo
    numBytes = sprintf(buffer, "1-Ajustar umbrales\n2-Log de datos\n");
    //mode = INIT;
    putUSBUSART(buffer, numBytes);
    //CDCTxService();
    for (;;) {
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        // Si estamos esperando un modo e ingresa un caracter.
        if ((numBytes == 1) && (buffer[0] == '1')) {
            //mode = INIT;
            if (USBUSARTIsTxTrfReady()) {
                //numBytes = sprintf(buffer, "Ingrese la palabra %s para salir\n", EXIT_CONFIG_ADC);
                numBytes = sprintf(buffer, "Seleccione el umbral\n1-Brusco\n2-Choque");
                putUSBUSART(buffer, numBytes);
                return threshold_select();
            }
        }// Estamos esperando input para una funcionalidad específica.
        else if (numBytes == 1 && (buffer[0] == '2')) {
            return 3;
        }
    }
}

uint8_t threshold_select() {
    uint8_t threshold;
    for (;;) {
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        if ((numBytes == 1) && (buffer[0] == '1')) {
            threshold = 1;
            break;
        } else if ((numBytes == 1) && (buffer[0] == '2')) {
            threshold = 2;
            break;
        }
    }
    while(!USBUSARTIsTxTrfReady());
    numBytes = sprintf(buffer, "Ingrese la palabra %s para salir\n", EXIT_CONFIG_ADC);
    putUSBUSART(buffer, numBytes);
    return threshold;
}

bool exit_config_ADC() {
    getsUSBUSART(buffer, sizeof (buffer));
    return strncmp(buffer, EXIT_CONFIG_ADC, strlen(EXIT_CONFIG_ADC)) == 0;
}
/* *****************************************************************************
 End of File
 */
