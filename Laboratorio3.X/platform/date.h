#ifndef _DATE_H
#define _DATE_H

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
uint8_t convert_to_int(uint8_t c1, uint8_t c2);

bool is_leap(uint8_t year);

bool valid_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

bool valid_date(uint8_t day, uint8_t month, uint8_t year);

bool set_date(uint8_t* buffer);

struct tm get_date();

#endif /* _DATE_H */
