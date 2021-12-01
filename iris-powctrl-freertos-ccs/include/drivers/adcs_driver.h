/*
 * adcs_driver.h
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */

#include "drivers/protocol/spi.h"

#ifndef INCLUDE_DRIVERS_ADCS_DRIVER_H_
#define INCLUDE_DRIVERS_ADCS_DRIVER_H_

#define POWER_DOWN  0x08
#define POWER_ON    0x09
#define TELEMETRY   0x10
#define READOUT     0x11
#define MAG1_ON     0x2A
#define MAG2_ON     0x2B
#define MAG3_ON     0x2C
#define MAG1_OFF    0x3A
#define MAG2_OFF    0x3B
#define MAG3_OFF    0x3C
#define RESET_ADCS  0xAA


void set_magnetorquer(unsigned char num, unsigned char val);
void read_sunsensor();


#endif /* INCLUDE_DRIVERS_ADCS_DRIVER_H_ */
