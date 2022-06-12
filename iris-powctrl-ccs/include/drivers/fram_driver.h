/*
 * fram_driver.h
 *
 *  Created on: May 18, 2022
 *      Author: Jayden McKoy
 */

#ifndef INCLUDE_DRIVERS_FRAM_DRIVER_H_
#define INCLUDE_DRIVERS_FRAM_DRIVER_H_


/***** Things to store in NVS: *****/
// - Periodic Telemetry:
//  - Operation mode
//  - SoC
//  - Thermistor status, readings
// - Checkout activities
//  -
// - Comissioning activities:
//  - SoC
//  -

uint16_t LogBootCount(uint8_t bootcount);
uint16_t LogOpMode(uint8_t opmode);
uint16_t LogSoc(float soc);
uint16_t RetrieveBootCount(uint8_t * prevBootCount);
uint16_t GetModeLog(uint8_t * prevOpMode);
uint16_t RetrieveSoc(float * prevSoc);
uint8_t NvsInit(void);


#endif /* INCLUDE_DRIVERS_FRAM_DRIVER_H_ */
