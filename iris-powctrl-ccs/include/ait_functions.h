/*
 * ait_functions.h
 *
 *  Created on: May 11, 2022
 *      Author: Jayden McKoy
 */

#ifndef INCLUDE_AIT_FUNCTIONS_H_
#define INCLUDE_AIT_FUNCTIONS_H_


void AitAdcsControl(void);
void AitEclipseControl(void);
bool AitGetEclipse(void);
void AitSetEclipse(bool in_eclipse);
void AitMonitorSoc(void);
void AitSetBatterySoc(float percent_soc);


#endif /* INCLUDE_AIT_FUNCTIONS_H_ */
