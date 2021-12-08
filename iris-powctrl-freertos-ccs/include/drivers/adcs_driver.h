/*
 * adcs_driver.h
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */

#include "driverlib.h"
#include "drivers/protocol/spi.h"
#include "tasks/telemetry.h"

#ifndef INCLUDE_DRIVERS_ADCS_DRIVER_H_
#define INCLUDE_DRIVERS_ADCS_DRIVER_H_

void vTestADCS(void *prvParameters);
bool set_pwm_torquerodx(uint8_t num_tr, uint8_t pwm_value);
bool set_magnetorquerx(uint8_t num_tr, bool turnon);
void read_sunsensor();


#endif /* INCLUDE_DRIVERS_ADCS_DRIVER_H_ */
