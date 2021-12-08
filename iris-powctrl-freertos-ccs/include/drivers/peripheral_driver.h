/*
 * peripheral_driver.h
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */

#ifndef INCLUDE_DRIVERS_PERIPHERAL_DRIVER_H_
#define INCLUDE_DRIVERS_PERIPHERAL_DRIVER_H_


#include <driverlib.h>
#include <msp430.h>
#include "drivers/protocol/spi.h"
#include "drivers/protocol/can.h"


// Solar cell enable ports
#define GPIO_PORT_EN_A1             GPIO_PORT_P3
#define GPIO_PORT_EN_A2             GPIO_PORT_P4
#define GPIO_PORT_EN_A3             GPIO_PORT_P4
#define GPIO_PORT_EN_A4             GPIO_PORT_P2
#define GPIO_PORT_EN_A5             GPIO_PORT_P3
#define GPIO_PORT_EN_A6             GPIO_PORT_P2
#define GPIO_PORT_EN_A7             GPIO_PORT_P4
// Solar cell enable pins
#define GPIO_PIN_EN_A1              BIT7
#define GPIO_PIN_EN_A2              BIT0
#define GPIO_PIN_EN_A3              BIT1
#define GPIO_PIN_EN_A4              BIT3
#define GPIO_PIN_EN_A5              BIT4
#define GPIO_PIN_EN_A6              BIT7
#define GPIO_PIN_EN_A7              BIT2
// Subsystem enable ports
#define GPIO_PORT_ADCS_EN           GPIO_PORT_P2
#define GPIO_PORT_CDH_EN            GPIO_PORT_P3
#define GPIO_PORT_COM_EN            GPIO_PORT_P5
#define GPIO_PORT_PLD_EN            GPIO_PORT_P3
#define GPIO_PORT_A_DPL_EN          GPIO_PORT_P3
#define GPIO_PORT_S_DPL_EN          GPIO_PORT_P3
#define GPIO_PORT_HTR_EN            GPIO_PORT_P3
// Subsystem enable pins
#define GPIO_PIN_ADCS_EN            BIT2
#define GPIO_PIN_CDH_EN             BIT6
#define GPIO_PIN_COM_EN             BIT5
#define GPIO_PIN_PLD_EN             BIT2
#define GPIO_PIN_A_DPL_EN           BIT0
#define GPIO_PIN_S_DPL_EN           BIT5
#define GPIO_PIN_HTR_EN             BIT3


void Init_Ports(void);
void Init_interrupts(void);

void setSolar(uint8_t n, uint8_t val);
void setLoads(uint8_t n, uint8_t val);

#endif /* INCLUDE_DRIVERS_PERIPHERAL_DRIVER_H_ */
