/*
 * main.c
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgari1
 */

#include <msp430.h>				
#include "pin_control.h"
#include "mission_timing.h"


char is_counting = 0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

	pinMode (7, HIGH);

	volatile unsigned int i;		// volatile to prevent optimization

	initPins();
	setSSR(APB, HIGH);

    setSSR(MPB, LOW);

    setSSR(APB, LOW);

	timerA2_init();

	while(1)
	{

	    if (digitalRead(RBF) == LOW)
	    {
	        if(is_counting == 0)
	        {
	            startTMR();
	            is_counting = 1;
	        }
	        else
	            checkTimers();
	    }
	    else
	    {
	        setSSR(MPB, LOW);
	        is_counting = 0;
	    }

	}
}


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERA0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    ms+=10;
    aux_ms1+=10;

    if (aux_ms1 >= 50)
    {
        P1OUT ^= 0x80;
        aux_ms1 = 0;
    }

    if (ms >= 977)
    {

        ms=0;
        if (is_counting == 1)
            s++;

        if (s>=60)
        {
            s=0;
            m++;
            if(m>=60)
            {
                m=0;
                h++;
            }
        }
    }
}
