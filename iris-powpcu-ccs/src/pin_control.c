/*
 * pin_control.c
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgaria1
 */
#include "pin_control.h"
#include "mission_timing.h"
#include <msp430.h>

void pinMode (char p, char m)
{
    if (m == OUTPUT)
        P1DIR |= (1 << p);
    else if (m == INPUT)
        P1DIR &= (0xFF ^ (1 << p));
}

void digitalWrite (char p, char v)
{
    if (v == HIGH)
        P1OUT |= (1 << p);
    else if (v == LOW)
        P1OUT &= (0xFF ^ (1 << p));
}

void delay(unsigned int t)
{
    int i;
    for(i=t; i>0; i--);
}

void delaydelay(unsigned int tt)
{
    int ii, i;
    for(ii=tt; ii>0; ii--)
        for(i=1000; i>0; i--);
}

void setSSR(char ssr, char state)
{
    pinMode(TMROUT, OUTPUT);
    pinMode(MPB, OUTPUT);
    pinMode(APB, OUTPUT);

    if (state == HIGH)
        digitalWrite (TMROUT, HIGH);
    else
        digitalWrite (TMROUT, LOW);

    if (ssr == MPB)
    {
        digitalWrite(MPB, LOW);
        delay(1000);
        digitalWrite(MPB, HIGH);
    }else if (ssr == APB)
    {
        digitalWrite(APB, LOW);
        delay(1000);
        digitalWrite(APB, HIGH);
    }
}

void initPins() // sets initial states and values of the pins
{
    // pin      I/O?    Value?
    //_________________________
    // MPB      OUTPUT  1
    // TMROUT   OUTPUT  0
    // APB      OUTPUT  1
    // RBF      INPUT   1
    // ATMR1    INPUT   0*
    // ATMR2    INPUT   0*
    // TMRRST   OUTPUT  1**
    // WDI      OUTPUT  1
    //__________________________
    // * value might change later depending on the circuit
    // ** change to "0" if reset is active high

    pinMode(MPB, OUTPUT);
    pinMode(TMROUT, OUTPUT);
    pinMode(APB, OUTPUT);
    pinMode(RBF, INPUT);
    pinMode(ATMR1, INPUT);
    pinMode(ATMR2, INPUT);
    pinMode(TMRRST, OUTPUT);
    pinMode(WDI, OUTPUT);

    digitalWrite(MPB, HIGH);
    digitalWrite(TMROUT, LOW);
    digitalWrite(APB, HIGH);
    digitalWrite(RBF, HIGH);
    digitalWrite(ATMR1, LOW);
    digitalWrite(ATMR2, LOW);
    digitalWrite(TMRRST, HIGH);
    digitalWrite(WDI, HIGH);

}

char digitalRead(char p)
{

    if ((P1IN & (1<<p)) == 0)
        return LOW;
    else
        return HIGH;
}

void resetATMRS()
{
    digitalWrite(TMRRST, LOW);
    delay(1000);
    digitalWrite(TMRRST, HIGH);
    delay(1000);
}

