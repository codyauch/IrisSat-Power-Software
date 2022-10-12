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
        delay(100);
        digitalWrite(MPB, HIGH);
    }else if (ssr == APB)
    {
        digitalWrite(APB, LOW);
        delay(100);
        digitalWrite(APB, HIGH);
    }
}



char digitalRead(char p)
{

    if ((P1IN & (1<<p)) == 0)
        return LOW;
    else
        return HIGH;
}

void resetATMRS(void)
{
    digitalWrite(TMRRST, LOW); // ****** for FM this needs to be HIGH
    delay(1000);
    digitalWrite(TMRRST, HIGH);// ****** for FM this needs to be LOW
    delay(1000);
}

