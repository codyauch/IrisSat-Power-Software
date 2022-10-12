/*
 * mission_timing.c
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgaria1
 */

#include <msp430.h>
#include "mission_timing.h"
#include "pin_control.h"
#include "nvs.h"


char is_counting = 0;


void MainPostEjectionHold(void)
{

    digitalWrite(MPB, HIGH);
    digitalWrite(TMROUT, LOW);
    digitalWrite(APB, HIGH);
    digitalWrite(RBF, HIGH);
    digitalWrite(ATMR1, LOW);
    digitalWrite(ATMR2, LOW);
    digitalWrite(TMRRST, HIGH); //********* for FM this needs to be LOW
    digitalWrite(WDI, HIGH);

    setSSR(MPB, LOW);
    setSSR(APB, HIGH);

    timerA2_init();

    while(1)
    {

        if (digitalRead(RBF) == HIGH) //RBF removed //****** for FM the RBF logic is reversed (this is HIGH for FM)
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
            //WriteOpMode(MODE_NORMAL_OPERATIONS);
            setSSR(MPB, LOW);
            is_counting = 0;
            //return;
        }

    }
}


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
    if (s>10 | m>0 | h>0) //(m>30 | h>0) this should be this for FM
        digital_TMR = 1;
    else
        digital_TMR = 0;

    timer_vote = digitalRead(ATMR1) + digitalRead(ATMR2) + digital_TMR;

    if(timer_vote < 1) // I changed 2 and 1 to test something
    {
        // Begin a 10-second timer (Timer A interrupt routine)
        // If 10 seconds have elapsed and there has still been only 1 vote, reset the timers
       // if(one_vote_TTR_sec >= TTR_SEC)
       // {
        //   startTMR();
        //}
        setSSR(MPB, LOW);//just for test
    }
    else if(timer_vote >= 1)
    {
        setSSR(MPB, HIGH);
        //setAPB(ON);
        // Do something after..?
        //while (1);
        WriteOpMode(MODE_NORMAL_OPERATIONS);
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

    if (aux_ms1 >= 50)      // this is for resetting the WDT
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

            m++;
            s=0;

            if(m>=60)
            {

                h++;
                m=0;
            }
        }
    }
}
