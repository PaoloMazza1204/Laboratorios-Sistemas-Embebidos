/* ************************************************************************** */
/** menu

  @File Name
    menu.c

  @Summary
    Contiene funciones para el manejo del menú.
 */
/* ************************************************************************** */
#include "menu.h"
#include "date.h"
#include "led_RGB.h"

#define CANCEL_CONFIG_ADC "Cancelar"
#define CONFIRM_CONFIG_ADC "Confirmar"

uint8_t buffer[64];
uint8_t numBytes;
MENU_MODE menu_mode = START;

static void write(SemaphoreHandle_t semaphore_USB, uint8_t* format, uint8_t* string1, uint8_t* string2);
static void write_log(SemaphoreHandle_t semaphore_USB, uint8_t* format, uint16_t id, time_t* date,
        uint8_t* latitude, uint8_t* longitude, uint8_t* drive_pattern);
static void datef(time_t* date, uint8_t* date_format);
static void printFloat(uint8_t* str, float value);

/**
 * Interfaz de usuario.
 * @param semaphore_USB
 * Semáforo que se habilita cuando el USB está listo.
 */
void user_interface(SemaphoreHandle_t semaphore_USB) {
    bool greeting_sent = false;
    for (;;) {
        // Inicio del menú.
        if (menu_mode == START && !greeting_sent) {
            greeting_sent = true;
            write(semaphore_USB, "\n\n1-Ajustar umbrales\n2-Log de datos\n3-Configurar colores", NULL, NULL);
        }
        numBytes = getsUSBUSART(buffer, sizeof (buffer)); // Leemos la entrada.
        if ((numBytes == 1) && (buffer[0] == '1')) {
            // Elige ajustar umbrales.
            if (menu_mode == START) {
                menu_mode = WAITING_THRESHOLD;
                write(semaphore_USB, "\n\nSeleccione el umbral\n1-Brusco\n2-Choque", NULL, NULL);
            }// Elige ajustar el umbral BRUSCO.
            else if (menu_mode == WAITING_THRESHOLD) {
                menu_mode = THRESHOLD_ABRUPT_CONFIG;
                write(semaphore_USB, "\nIngrese %s o %s", CANCEL_CONFIG_ADC, CONFIRM_CONFIG_ADC);
                return;
            }// Elige configurar el periodo del log.
            else if (menu_mode == WAITING_LOG) {
                menu_mode = LOG_TIME_CONFIG;
                write(semaphore_USB, "\nIngrese periodo deseado [1-255]", NULL, NULL);
                return;
            }
        } else if ((numBytes == 1) && (buffer[0] == '2')) {
            // Elige log de datos.
            if (menu_mode == START) {
                menu_mode = WAITING_LOG;
                write(semaphore_USB, "\n\n1-Configurar periodo log\n2-Descargar log\n", NULL, NULL);
            }// Elige descargar log.
            else if (menu_mode == WAITING_LOG) {
                menu_mode = DOWNLOAD_LOG;
                return;
            }// Elige configurar el umbral de CHOQUE.
            else if (menu_mode == WAITING_THRESHOLD) {
                menu_mode = THRESHOLD_CRASH_CONFIG;
                write(semaphore_USB, "\nIngrese %s o %s", CANCEL_CONFIG_ADC, CONFIRM_CONFIG_ADC);
                return;
            }
        } else if ((numBytes == 1) && (buffer[0] == '3')) {
            // Elige configurar colores.
            if (menu_mode == START) {
                menu_mode = WAITING_COLOR;
                write(semaphore_USB, "\n\nIngrese: R,G,B-MODO, donde MODO = {OK,BRUSCO,CHOQUE,UMBRAL}", NULL, NULL);
            }
        }// Ingresa color a cambiar.
        else if ((menu_mode == WAITING_COLOR) && (numBytes > 0)) {
            change_color();
            return;
        } else if (strncasecmp("cancelar", buffer, strlen("cancelar")) == 0) {
            greeting_sent = false;
            menu_mode = START;
        }
    }
}

/**
 * Compara el modo que se le pasa por parámetros con el modo actual del menú,
 * en caso de ser iguales retorna true.
 * @param mode
 * Modo a comparar.
 * @return 
 */
bool compare_to_menu_mode(MENU_MODE mode) {
    return mode == menu_mode;
}

/**
 * Valida el tiempo ingresado para la configuración del log.
 * @param semaphore_USB
 * Semáforo que se habilita cuando el USB está listo.
 * @return 
 */
uint8_t get_log_time(SemaphoreHandle_t semaphore_USB) {
    for (;;) {
        numBytes = getsUSBUSART(buffer, sizeof (buffer));
        if (numBytes > 0) {
            uint8_t input = atoi(buffer);
            if (input > 0 && input < 256) {
                reset_menu_mode();
                return input;
            }
            write(semaphore_USB, "\nNúmero fuera de rango", NULL, NULL);
        }
    }
}

/**
 * Devuelve true si el usuario ingresa la palabra definida para cancelar la
 * operación. Por defecto es: "Cancelar".
 * @return 
 */
bool cancel_config_ADC() {
    getsUSBUSART(buffer, sizeof (buffer));
    return strncasecmp(buffer, CANCEL_CONFIG_ADC, strlen(CANCEL_CONFIG_ADC)) == 0;
}

/**
 * Devuelve true si el usuario ingresa la palabra definida para confirmar la
 * operación. Por defecto es: "Confirmar".
 * @return 
 */
bool confirm_config_ADC() {
    getsUSBUSART(buffer, sizeof (buffer));
    return strncasecmp(buffer, CONFIRM_CONFIG_ADC, strlen(CONFIRM_CONFIG_ADC)) == 0;
}

/**
 * Muestra a traves del puerto COM todos los registros del log guardados.
 * @param semaphore_USB
 * Semáforo que se habilita cuando el USB está listo.
 */
void download_log(SemaphoreHandle_t semaphore_USB) {
    uint8_t * patterns[] = {"OK", "BRUSCO", "CHOQUE"};
    log_register_t* log = get_log();
    uint16_t id = get_id();
    uint8_t position = get_position();
    write(semaphore_USB, "\n\n*ID,FECHA Y HORA,LATITUD,LONGITUD,PATRÓN DE MANEJO*\n", NULL, NULL);
    uint8_t i;
    uint8_t buffer_latitude[11];
    uint8_t buffer_longitude[11];
    // Si llenamos el buffer y comenzamos a reemplazar datos.
    if (id > 250) {
        // Mostramos desde el dato más antiguo (próximo dato a reemplazar).
        for (i = position; i < 250; i++) {
            printFloat(buffer_latitude, log[i].latitude);
            printFloat(buffer_longitude, log[i].longitude);
            write_log(semaphore_USB, "\n%05d,%s,%s,%s,%s", log[i].id, &(log[i].date), buffer_latitude,
                    buffer_longitude, patterns[log[i].drive_pattern]);
        }
    }
    for (i = 0; i < position; i++) {
        printFloat(buffer_latitude, log[i].latitude);
        printFloat(buffer_longitude, log[i].longitude);
        write_log(semaphore_USB, "\n%05d,%s,%s,%s,%s", log[i].id, &(log[i].date), buffer_latitude,
                buffer_longitude, patterns[log[i].drive_pattern]);
    }
    reset_menu_mode();
}

/**
 * Cambia el color de los ledes RGB de un modo.
 */
void change_color() {
    ws2812_t color;
    uint8_t position;
    uint8_t * input[4];
    // Separamos lo ingresado por el usuario en r,g,b,modo.
    input[0] = strtok(buffer, ",");
    for (position = 1; position < 4; position++) {
        input[position] = strtok(NULL, ",-");
    }
    color.r = atoi(input[0]) < 256 ? atoi(input[0]) : 255;
    color.g = atoi(input[1]) < 256 ? atoi(input[1]) : 255;
    color.b = atoi(input[2]) < 256 ? atoi(input[2]) : 255;
    map_mode(input[3], color);
    reset_menu_mode();
}

/**
 * Asigna el color al modo correspondiente.
 * @param mode
 * Modo a modificar.
 * @param color
 * Color a setear.
 */
void map_mode(uint8_t* mode, ws2812_t color) {
    if (strncasecmp(mode, "OK", 2) == 0) {
        set_mode_color(COLOR_OK_POSITION, color);
    } else if (strncasecmp(mode, "BRUSCO", 6) == 0) {
        set_mode_color(COLOR_ABRUPT_POSITION, color);
    } else if (strncasecmp(mode, "CHOQUE", 6) == 0) {
        set_mode_color(COLOR_CRASH_POSITION, color);
    } else if (strncasecmp(mode, "UMBRAL", 6) == 0) {
        set_mode_color(COLOR_THRESHOLD_POSITION, color);
    }
}

/**
 * Coloca en el USB el mensaje a enviar con el formato correspondiente.
 * @param semaphore_USB
 * Semáforo que se habilita cuando el USB está listo.
 * @param format
 * String con el fotmato a enviar.
 * @param string1
 * Parámetro 1 para el formato.
 * @param string2
 * Parámetro 2 para el formato.
 */
static void write(SemaphoreHandle_t semaphore_USB, uint8_t* format, uint8_t* string1, uint8_t* string2) {
    xSemaphoreTake(semaphore_USB, portMAX_DELAY);
    if (string1 != NULL) {
        numBytes = sprintf(buffer, format, string1, string2);
    } else {
        numBytes = sprintf(buffer, format);
    }
    putUSBUSART(buffer, numBytes);
}

/**
 * Coloca en el USB el contenido de un registro.
 * @param semaphore_USB
 * Semáforo que se habilita cuando el USB está listo.
 * @param format
 * String con el formato a enviar.
 * @param id
 * Id del registro.
 * @param date
 * Tiempo en el que se tomó el registro.
 * @param latitude
 * Latitud con cual se cargó el registro.
 * @param longitude
 * Longitud con la cual se cargó el registro.
 * @param drive_pattern
 * Patrón de manejo del registro.
 */
static void write_log(SemaphoreHandle_t semaphore_USB, uint8_t* format, uint16_t id, time_t* date,
        uint8_t* latitude, uint8_t* longitude, uint8_t* drive_pattern) {
    uint8_t date_format[19];
    datef(date, date_format);
    xSemaphoreTake(semaphore_USB, portMAX_DELAY);
    numBytes = sprintf(buffer, format, id, date_format, latitude, longitude, drive_pattern);
    putUSBUSART(buffer, numBytes);
}

/**
 * Transforma el tiempo a un formato AAAA/MM/DD HH:MM:SS.
 * @param date
 * Tiempo en segundos.
 * @param date_format
 * Buffer donde se guardará con formato.
 */
static void datef(time_t* date, uint8_t* date_format) {
    struct tm* date_struct = gmtime(date);
    sprintf(date_format, "%04d/%02d/%02d %02d:%02d:%02d", date_struct->tm_year + 1900, date_struct->tm_mon + 1,
            date_struct->tm_mday, date_struct->tm_hour, date_struct->tm_min, date_struct->tm_sec);
}

/**
 * Coloca el modo del menú en START.
 */
void reset_menu_mode() {
    menu_mode = START;
}

/**
 * Coloca un float en el buffer.
 * @param str
 * Buffer donde se guarda el resultado.
 * @param real
 * Valor float.
 */
static void printFloat(uint8_t* str, float value) {
    int16_t num = value > 0 ? floor(value) : ceil(value);
    int32_t resto = fabs(((value - num) * 1000000));
    sprintf(str, "%d.%d", num, resto);
}


/* *****************************************************************************
 End of File
 */
