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


uint8_t TestNvsLog(void);
uint16_t LogAddMode(uint8_t mode);
uint8_t GetModeLog(uint8_t * log_data);
uint8_t NvsLogInit(void);
uint8_t NvsLogReset(void);


#endif /* INCLUDE_DRIVERS_FRAM_DRIVER_H_ */
