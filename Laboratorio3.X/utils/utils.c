/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include "utils.h"
#include "../mcc_generated_files/tmr2.h"
#include <stdbool.h>
#include <stdint.h>
#include "../mcc_generated_files/rtcc.h"

/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
bool UT_delayms(ut_tmrDelay_t* p_timer, uint32_t p_ms)
{
    if (p_timer -> state == UT_TMR_DELAY_INIT)
    {
        p_timer -> startValue = TMR2_SoftwareCounterGet();
        p_timer -> state = UT_TMR_DELAY_WAIT;
        return false;
    }
    /* La comparación cambiaría en caso de overflow, pero obviamos este caso
     * pues ocurriría recién a los 24 días con 20 horas y 31 minutos con 
     * 23 segundos y casi 648 milisegundos aproximadamente. Esto teniendo
     * en cuenta que TMR2_SoftwareCounterGet() devuelve un signed int.*/
    if ((TMR2_SoftwareCounterGet() - p_timer -> startValue) >= p_ms)
    {
        p_timer -> state = UT_TMR_DELAY_INIT;
        return true;
    }
    return false;
}
