/*
 * mission_timing.h
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgaria1
 */


#ifndef MISSION_TIMING_H_
#define MISSION_TIMING_H_

#define TTR_SEC 10

volatile unsigned int ms, s, m, h, aux_ms1, one_vote_TTR_sec, timer_vote, digital_TMR;

void timerA2_init();
void startTMR();
void checkTimers();

#endif /* MISSION_TIMING_H_ */
