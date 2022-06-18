/*
 * main.c
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgari1
 */
#include "main.h"
#include "pin_control.h"
#include "mission_timing.h"
#include "nvs.h"


void MainInitGpio(void); // sets initial states and values of the pins
void NormalModeInitGpio(void);

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

	MainInitGpio();

//	WriteOpMode(MODE_POST_EJECTION_HOLD);
//	WriteOpMode(MODE_NORMAL_OPERATIONS);

	uint8_t opmode = GetOpMode();

	if(opmode == MODE_POST_EJECTION_HOLD)
	{
	    MainPostEjectionHold();
	}
	else
	{
	    NormalModeInitGpio();
	}

	while(1)
	{
	    __no_operation();
	}

}

void NormalModeInitGpio(void)
{

}

void MainInitGpio(void) // sets initial states and values of the pins
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

}
