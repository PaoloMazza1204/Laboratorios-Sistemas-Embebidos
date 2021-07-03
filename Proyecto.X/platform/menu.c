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
#include "date.h"

#define EXIT_CONFIG_ADC "Exit"

uint8_t buffer[64];
uint8_t numBytes;
MENU_MODE mode_menu = START;

static void write(SemaphoreHandle_t semaphore_USB);

uint8_t user_interface(SemaphoreHandle_t semaphore_USB) {
    numBytes = sprintf(buffer, "\n1-Ajustar umbrales\n2-Log de datos");
    putUSBUSART(buffer, numBytes);
    uint8_t input;
    for (;;) {
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        // Si estamos esperando un modo e ingresa un caracter.
        if ((numBytes == 1) && (buffer[0] == '1')) {
            xSemaphoreTake(semaphore_USB, portMAX_DELAY);
            numBytes = sprintf(buffer, "\nSeleccione el umbral\n1-Brusco\n2-Choque");
            putUSBUSART(buffer, numBytes);
            input = threshold_select();
            xSemaphoreTake(semaphore_USB, portMAX_DELAY);
            numBytes = sprintf(buffer, "\nIngrese la palabra %s para salir", EXIT_CONFIG_ADC);
            putUSBUSART(buffer, numBytes);
            return input;
        }// Estamos esperando input para una funcionalidad específica.
        else if (numBytes == 1 && (buffer[0] == '2')) {
            xSemaphoreTake(semaphore_USB, portMAX_DELAY);
            numBytes = sprintf(buffer, "\n1-Configurar periodo log\n2-Descargar log");
            putUSBUSART(buffer, numBytes);
            if (threshold_select() == 1) {
                xSemaphoreTake(semaphore_USB, portMAX_DELAY);
                numBytes = sprintf(buffer, "\nIngrese periodo deseado [1-255]");
                putUSBUSART(buffer, numBytes);
                return 3;
            }
            return 4;
        }
    }
}

uint8_t get_log_time(SemaphoreHandle_t semaphore_USB) {
    for (;;) {
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        if (numBytes > 0) {
            uint8_t input = atoi(buffer);
            if (input > 0 && input < 256) {
                return input;
            }
            xSemaphoreTake(semaphore_USB, portMAX_DELAY);
            numBytes = sprintf(buffer, "\nNúmero fuera de rango");
            putUSBUSART(buffer, numBytes);
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
    return threshold;
}

bool exit_config_ADC() {
    getsUSBUSART(buffer, sizeof (buffer));
    return strncmp(buffer, EXIT_CONFIG_ADC, strlen(EXIT_CONFIG_ADC)) == 0;
}

void download_log(SemaphoreHandle_t semaphore_USB) {
    uint8_t * patterns[] = {"OK", "BRUSCO", "CHOQUE"};
    log_register_t *log = get_log();
    uint32_t id = get_id();
    uint8_t position = get_position();
    xSemaphoreTake(semaphore_USB, portMAX_DELAY);
    numBytes = sprintf(buffer, "\n\n*ID - PATRÓN DE MANEJO*\n");
    putUSBUSART(buffer, numBytes);
    uint8_t i;
    if (id > 250) {
        for (i = position; i < 250; i++) {
            xSemaphoreTake(semaphore_USB, portMAX_DELAY);
            numBytes = sprintf(buffer, "\n%d - %s", log[i].id, patterns[log[i].drive_pattern]);
            putUSBUSART(buffer, numBytes);
        }
    }
    for (i = 0; i < position; i++) {
        xSemaphoreTake(semaphore_USB, portMAX_DELAY);
        numBytes = sprintf(buffer, "\n%d - %s", log[i].id, patterns[log[i].drive_pattern]);
        putUSBUSART(buffer, numBytes);
    }
}


/* *****************************************************************************
 End of File
 */
