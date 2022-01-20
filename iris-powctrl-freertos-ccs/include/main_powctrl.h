/*
 * main_powctrl.h
 *
 *  Created on: Dec 8, 2021
 *      Author: mckoyj
 */

#ifndef INCLUDE_MAIN_POWCTRL_H_
#define INCLUDE_MAIN_POWCTRL_H_


int powctrl_main(void);
void readAllSensors(void *pvParameters);
void Init_GPIO(void);
void TestSolarCells(void);
void TestPowerSupply(void);
void Init_interrupts_main(void);
float read_temperature(unsigned char therm); /// input ADC channel from 0 to 6
float read_solar_current(unsigned char solar);
float read_load_current(unsigned char load);
float read_MSB_voltage();



#endif /* INCLUDE_MAIN_POWCTRL_H_ */
