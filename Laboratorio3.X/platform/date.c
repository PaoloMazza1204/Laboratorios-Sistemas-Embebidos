/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "date.h"

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
struct tm date;

/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */

/* Devuelve el número representado por dos dígitos
 * c1 (decenas) y c2 (unidades). */
uint8_t convert_to_int(uint8_t c1, uint8_t c2)
{
    uint8_t d1 = c1 - '0';
    uint8_t d2 = c2 - '0';
    return (d1*10) + d2;
}

/* Devuelve si el año parámetro es bisiesto. */
bool is_leap(uint8_t year)
{
    return (year % 4 == 0);
}

/* Devuelve si la hora, minutos y segundos son correctos. */
bool valid_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    return ( (hours >= 0 && hours < 24) && (minutes >= 0 && minutes < 60) && (seconds >= 0 && seconds < 60));
}

/* Devuelve si la fecha es válida. */
bool valid_date(uint8_t day, uint8_t month, uint8_t year)
{
    if (month > 0 && month < 13)
    {
        if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
        {
            return (day > 0 && day < 32);
        }
        else if (month == 2)
        {
            if (is_leap(year))
            {
                return (day > 0 && day < 30);
            }
            return (day > 0 && day < 29);
        }
        return (day > 0 && day < 31);
    }
    return false;
}

/* Mapea el año según lo definido en el documento.
 * Si el año está entre 0 y 70 entonces se suma 100. */
int map_year(uint8_t year)
{
    return ((year < 70 && year >= 0) ? year+100 : year);
}

/* Si el contenido del buffer es válido establece la fecha de date con él. */
bool set_date(uint8_t* buffer)
{
    uint8_t* initial = buffer;
    uint8_t year = convert_to_int(*(buffer++), *(buffer++));
    buffer++;
    uint8_t month = convert_to_int(*(buffer++), *(buffer++));
    buffer++;
    uint8_t day = convert_to_int(*(buffer++), *(buffer++));
    buffer++;
    uint8_t hours = convert_to_int(*(buffer++), *(buffer++));
    buffer++;
    uint8_t minutes = convert_to_int(*(buffer++), *(buffer++));
    buffer++;
    uint8_t seconds = convert_to_int(*(buffer++), *(buffer++));
    buffer = initial;
    if (valid_time(hours, minutes, seconds) && (valid_date(day, month, year)))
    {
        date.tm_year = map_year(year);
        date.tm_mon = month; // -1.
        date.tm_mday = day;
        date.tm_hour = hours;
        date.tm_min = minutes;
        date.tm_sec = seconds;
        RTCC_TimeSet(&date);
        return true;
    }
    return false;
}

/* Devuelve date. */
struct tm get_date()
{
    return date;
}
