/*
 * main.h
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#ifndef INCLUDE_MAIN_H_
#define INCLUDE_MAIN_H_

#include <msp430.h>
#include "driverlib.h"


#define AIT_MODE
#define THERMAL_CONTROL


void detumbleDriver(void);
//volatile uint32_t ulRunTimeCounterOverflows;
//void vConfigureTimerForRunTimeStats( void )
//{
//    /* Force an assert. */
////    configASSERT( ( volatile void * ) NULL );
//}
//void vPeriodicEventGroupsProcessing( void );
//void xNotifyTaskFromISR( void );



#endif /* INCLUDE_MAIN_H_ */
