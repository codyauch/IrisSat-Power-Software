/*
 * spi.h
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */

#ifndef INCLUDE_DRIVERS_PROTOCOL_SPI_H_
#define INCLUDE_DRIVERS_PROTOCOL_SPI_H_

#include <stdio.h>

#include <driverlib.h>
#include <msp430fr2475.h>

// SPI A0 ports
#define GPIO_PORT_SPIA0_CS                  GPIO_PORT_P4
#define GPIO_PORT_SPIA0_CLK_MISO_MOSI       GPIO_PORT_P5
// SPI A0 pins
#define GPIO_PIN_SPIA0_CS           BIT7
#define GPIO_PIN_SPIA0_CLK          BIT0
#define GPIO_PIN_SPIA0_MISO         BIT1
#define GPIO_PIN_SPIA0_MOSI         BIT2
// SPI A1 ports
#define GPIO_PORT_SPIA1_CLK_MISO_MOSI      GPIO_PORT_P2
#define GPIO_PORT_SPIA1_CS                 GPIO_PORT_P3
// SPI A1 pins
#define GPIO_PIN_SPIA1_CS           BIT1
#define GPIO_PIN_SPIA1_CLK          BIT4
#define GPIO_PIN_SPIA1_MISO         BIT5
#define GPIO_PIN_SPIA1_MOSI         BIT6
// SPI B0 ports
#define GPIO_PORT_SPIB0                     GPIO_PORT_P1
// SPI B0 pins
#define GPIO_PIN_SPIB0_CS           BIT0
#define GPIO_PIN_SPIB0_CLK          BIT1
#define GPIO_PIN_SPIB0_MISO         BIT3
#define GPIO_PIN_SPIB0_MOSI         BIT2
// SPI B1 ports
#define GPIO_PORT_SPIB1_MISO_MOSI           GPIO_PORT_P4
#define GPIO_PORT_SPIB1_CLK_CS              GPIO_PORT_P5
// SPI B1 pins
#define GPIO_PIN_SPIB1_CS           BIT4
#define GPIO_PIN_SPIB1_CLK          BIT3
#define GPIO_PIN_SPIB1_MISO         BIT3
#define GPIO_PIN_SPIB1_MOSI         BIT4

void Init_SPI();
void Init_SPIA0();
void Init_SPIA0();
void Init_SPIB0();
void Init_SPIB1();

uint32_t read_write_SPIA0(uint8_t data[]);
uint32_t read_write_SPIA1(uint8_t data[]);
uint16_t read_write_SPIB0(uint8_t data[]);
uint16_t read_write_SPIB1(uint8_t data[]);


#endif /* INCLUDE_DRIVERS_PROTOCOL_SPI_H_ */
