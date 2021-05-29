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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */

#include "date.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */
struct tm date;


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */

/** 
  @Function
    int ExampleLocalFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Description
    Full description, explaining the purpose and usage of the function.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Precondition
    List and describe any required preconditions. If there are no preconditions,
    enter "None."

  @Parameters
    @param param1 Describe the first parameter to the function.
    
    @param param2 Describe the second parameter to the function.

  @Returns
    List (if feasible) and describe the return values of the function.
    <ul>
      <li>1   Indicates an error occurred
      <li>0   Indicates an error did not occur
    </ul>

  @Remarks
    Describe any special behavior not described above.
    <p>
    Any additional remarks.

  @Example
    @code
    if(ExampleFunctionName(1, 2) == 0)
    {
        return 3;
    }
 */
uint8_t convert_to_int(uint8_t c1, uint8_t c2) {
    uint8_t d1 = c1 - '0';
    uint8_t d2 = c2 - '0';
    return (d1 * 10) +d2;
}

bool is_leap(uint8_t year) {
    return (year % 4 == 0);
}

bool valid_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return ( (hours >= 0 && hours < 24) && (minutes >= 0 && minutes < 60) && (seconds >= 0 && seconds < 60));
}

bool valid_date(uint8_t day, uint8_t month, uint8_t year) {
    if (month > 0 && month < 13) {
        if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
            return (day > 0 && day < 32);
        } else if (month == 2) {
            if (is_leap(year)) {
                return (day > 0 && day < 30);
            }
            return (day > 0 && day < 29);
        }
        return (day > 0 && day < 31);
    }
    return false;
}

int map_year(uint8_t year){
    return ((year < 70 && year >= 0) ? year+100 : year);
}

bool set_date(uint8_t* buffer) {
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
    if (valid_time(hours, minutes, seconds) && (valid_date(day, month, year))) {
        date.tm_year = map_year(year);
        date.tm_mon = month;
        date.tm_mday = day;
        date.tm_hour = hours;
        date.tm_min = minutes;
        date.tm_sec = seconds;
        RTCC_TimeSet(&date);
        return true;
    }
    return false;
}

struct tm get_date(){
    return date;
}


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

/** 
  @Function
    int ExampleInterfaceFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Remarks
    Refer to the example_file.h interface header for function usage details.
 */
int ExampleInterfaceFunction(int param1, int param2) {
    return 0;
}


/* *****************************************************************************
 End of File
 */
