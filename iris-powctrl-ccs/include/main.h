/*
 * main.h
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#ifndef INCLUDE_MAIN_H_
#define INCLUDE_MAIN_H_

#include "driverlib.h"


void Init_GPIO(void);
void TestSolarCells(void);
void TestPowerSupply(void);
void Init_interrupts(void);
float read_temperature(uint8_t therm); /// input ADC channel from 0 to 6
float read_solar_current(uint8_t solar);
float read_load_current(uint8_t load);
float read_MSB_voltage();
void set_POW_mode(uint8_t mode);



#endif /* INCLUDE_MAIN_H_ */
