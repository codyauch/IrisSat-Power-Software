/*
 * mission_timing.c
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgaria1
 */

#include <msp430.h>
#include "mission_timing.h"
#include "pin_control.h"



void timerA2_init()
{
    // Enable capture/compare interrupts
    TA0CCTL0 |= CCIE;

    // Set capture/compare time (clock cycles)
    TA0CCR0 = 10000;

    // Set clock source and mode
    TA0CTL |= TASSEL1 | MC0;

    __bis_SR_register(GIE);
}

void startTMR()
{
    s=0;
    m=0;
    h=0;
    resetATMRS();

}

void checkTimers()
{
    if (s>=3)
        digital_TMR = 1;
    else
        digital_TMR = 0;

    timer_vote = digitalRead(ATMR1) + digitalRead(ATMR2) + digital_TMR;

    if(timer_vote == 1) // I changed 2 and 1 to test something
    {
        // Begin a 10-second timer (Timer A interrupt routine)
        // If 10 seconds have elapsed and there has still been only 1 vote, reset the timers
        if(one_vote_TTR_sec >= TTR_SEC)
        {
           startTMR();
        }
        setSSR(MPB, HIGH);//just for test
    }
    else if(timer_vote >= 2)
    {
        setSSR(MPB, HIGH);
        //setAPB(ON);
        // Do something after..?
    }

}
