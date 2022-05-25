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


uint16_t TestNvsLog(void);
uint16_t LogAddMode(uint8_t mode);
uint16_t LogAddSoc(float soc);
uint16_t GetModeLog(uint8_t * log_data);
uint16_t GetSocLog(float * log_data);
uint16_t NvsCommitMode(uint8_t mode);
uint16_t NvsCommitSoc(float soc);
uint8_t NvsInit(void);


#endif /* INCLUDE_DRIVERS_FRAM_DRIVER_H_ */
