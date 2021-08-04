/*
 * adcbankAB_driver.h
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */

#ifndef INCLUDE_DRIVERS_ADCBANKAB_DRIVER_H_
#define INCLUDE_DRIVERS_ADCBANKAB_DRIVER_H_

#include "drivers/protocol/spi.h"

// Ports

// Pins


void Init_ADC_A();
void Init_ADC_B();
unsigned int read_ADC_A(unsigned char channel);
unsigned int read_ADC_B(unsigned char channel);


#endif /* INCLUDE_DRIVERS_ADCBANKAB_DRIVER_H_ */
