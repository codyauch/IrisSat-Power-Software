/*
 * mission_timing.h
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgaria1
 */


#ifndef MISSION_TIMING_H_
#define MISSION_TIMING_H_

#include "main.h"

#define TTR_SEC 120

volatile unsigned int ms, s, m, h, aux_ms1, one_vote_TTR_sec, timer_vote, digital_TMR;

void MainPostEjectionHold(void);
void timerA2_init();
void startTMR();
uint8_t checkTimers();

#endif /* MISSION_TIMING_H_ */
