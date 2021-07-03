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

#define CANCEL_CONFIG_ADC "Cancelar"
#define CONFIRM_CONFIG_ADC "Confirmar"

uint8_t buffer[64];
uint8_t numBytes;
MENU_MODE menu_mode = START;

static void write(SemaphoreHandle_t semaphore_USB);

void user_interface(SemaphoreHandle_t semaphore_USB) {
    bool greeting_sent = false;
    for (;;) {
        if (menu_mode == START && !greeting_sent) {
            greeting_sent = true;
            numBytes = sprintf(buffer, "\n1-Ajustar umbrales\n2-Log de datos");
            putUSBUSART(buffer, numBytes);
        }
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        if ((numBytes == 1) && (buffer[0] == '1')) {
            if (menu_mode == START) {
                menu_mode = WAITING_THRESHOLD;
                xSemaphoreTake(semaphore_USB, portMAX_DELAY);
                numBytes = sprintf(buffer, "\nSeleccione el umbral\n1-Brusco\n2-Choque");
                putUSBUSART(buffer, numBytes);
            } else if (menu_mode == WAITING_THRESHOLD) {
                menu_mode = THRESHOLD_ABRUPT_CONFIG;
                xSemaphoreTake(semaphore_USB, portMAX_DELAY);
                numBytes = sprintf(buffer, "\nIngrese %s o %s", CANCEL_CONFIG_ADC, CONFIRM_CONFIG_ADC);
                putUSBUSART(buffer, numBytes);
                return;
            } else if (menu_mode == WAITING_LOG) {
                menu_mode = LOG_TIME_CONFIG;
                xSemaphoreTake(semaphore_USB, portMAX_DELAY);
                numBytes = sprintf(buffer, "\nIngrese periodo deseado [1-255]");
                putUSBUSART(buffer, numBytes);
                return;
            }
        } else if ((numBytes == 1) && (buffer[0] == '2')) {
            if (menu_mode == START) {
                menu_mode = WAITING_LOG;
                xSemaphoreTake(semaphore_USB, portMAX_DELAY);
                numBytes = sprintf(buffer, "\n1-Configurar periodo log\n2-Descargar log");
                putUSBUSART(buffer, numBytes);
            } else if (menu_mode == WAITING_LOG) {
                menu_mode = DOWNLOAD_LOG;
                return;
            } else if (menu_mode == WAITING_THRESHOLD) {
                menu_mode = THRESHOLD_CRASH_CONFIG;
                xSemaphoreTake(semaphore_USB, portMAX_DELAY);
                numBytes = sprintf(buffer, "\nIngrese %s o %s", CANCEL_CONFIG_ADC, CONFIRM_CONFIG_ADC);
                putUSBUSART(buffer, numBytes);
                return;
            }
        }
        else if(strncasecmp("cancelar",buffer,strlen("cancelar")) == 0){
            greeting_sent = false;
            menu_mode = START;
        }
    }
}

bool compare_to_menu_mode(MENU_MODE mode) {
    return mode == menu_mode;
}

uint8_t get_log_time(SemaphoreHandle_t semaphore_USB) {
    for (;;) {
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        if (numBytes > 0) {
            uint8_t input = atoi(buffer);
            if (input > 0 && input < 256) {
                reset_menu_mode();
                return input;
            }
            xSemaphoreTake(semaphore_USB, portMAX_DELAY);
            numBytes = sprintf(buffer, "\nNúmero fuera de rango");
            putUSBUSART(buffer, numBytes);
        }
    }
}

bool cancel_config_ADC() {
    getsUSBUSART(buffer, sizeof (buffer));
    return strncasecmp(buffer, CANCEL_CONFIG_ADC, strlen(CANCEL_CONFIG_ADC)) == 0;
}

bool confirm_config_ADC() {
    getsUSBUSART(buffer, sizeof (buffer));
    return strncasecmp(buffer, CONFIRM_CONFIG_ADC, strlen(CONFIRM_CONFIG_ADC)) == 0;
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
    reset_menu_mode();
}

void reset_menu_mode() {
    menu_mode = START;
}


/* *****************************************************************************
 End of File
 */
