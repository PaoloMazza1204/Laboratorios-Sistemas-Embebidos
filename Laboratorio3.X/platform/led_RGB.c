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

#include "WS2812.h"
#include "led_RGB.h"
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
app_register_t app_register;
ws2812_t leds_RGB[8]; //= {BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};
ws2812_t colors_RGB[5]; //= {WHITE,RED,BLUE,GREEN,BLACK};


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
uint8_t map_led_RGB(uint8_t led) {
    switch (led) {
        case(6):
            return 1;
        case(4):
            return 2;
        case(1):
            return 3;
        case(2):
            return 4;
        case(3):
            return 5;
        case(5):
            return 6;
        case(8):
            return 7;
        case(7):
            return 8;
    }
    return 0;
}

bool set_ledRGB(uint8_t* buffer) {
    uint8_t* initial = buffer;
    uint8_t led = *(buffer++) - '0';
    buffer++;
    uint8_t color = *(buffer++) - '0';
    buffer = initial;

    struct tm date = get_date();
    if ((led > 0 && led < 9) && (color >= 0 && color < 5) && (RTCC_TimeGet(&date))) {
        leds_RGB[map_led_RGB(led) - 1] = colors_RGB[color];
        WS2812_send(leds_RGB, 8);
        app_register.led = led;
        app_register.color = color;
        app_register.time = mktime(&date);
        return true;
    }
    return false;
}

void initialize_leds_RGB() {
    uint8_t i;
    for (i = 0; i < 8; i++) {

        leds_RGB[i] = BLACK;
    }
    colors_RGB[0] = WHITE;
    colors_RGB[1] = RED;
    colors_RGB[2] = BLUE;
    colors_RGB[3] = GREEN;
    colors_RGB[4] = BLACK;
}

void initialize_app_register() {
    app_register.color = 4;
    app_register.led = 0;
    app_register.time = 0;
}

uint8_t get_last_led() {
    return app_register.led;
}

uint8_t get_last_color() {
    return app_register.color;
}

uint32_t get_last_time() {
    return app_register.time;
}

void set_last_led(uint8_t led) {
    if (led < 9 && led > 0) {
        app_register.led = led;
    }
}

void set_last_color(uint8_t color) {
    if (color < 5) {
        app_register.color = color;
    }
}

void set_last_time(uint32_t time) {
    app_register.time = time;
}

/* *****************************************************************************
 End of File
 */
