/*
 * peripheral_driver.h
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */

#ifndef INCLUDE_DRIVERS_PERIPHERAL_DRIVER_H_
#define INCLUDE_DRIVERS_PERIPHERAL_DRIVER_H_



#include <msp430.h>
#include <driverlib.h>

/******** ---Defines--- ********/
// Subsystem enable ports
#define GPIO_PORT_HTR_EN            GPIO_PORT_P3
#define GPIO_PORT_ADCS_EN           GPIO_PORT_P2
#define GPIO_PORT_COMS_EN           GPIO_PORT_P5
#define GPIO_PORT_CDH_EN            GPIO_PORT_P3
#define GPIO_PORT_PLD_EN            GPIO_PORT_P3
#define GPIO_PORT_A_DPL_EN          GPIO_PORT_P3
#define GPIO_PORT_S_DPL_EN          GPIO_PORT_P3
// Subsystem enable pins
#define GPIO_PIN_HTR_EN             BIT3
#define GPIO_PIN_ADCS_EN            BIT2
#define GPIO_PIN_CDH_EN             BIT6
#define GPIO_PIN_COMS_EN            BIT5
#define GPIO_PIN_PLD_EN             BIT2
#define GPIO_PIN_A_DPL_EN           BIT0
#define GPIO_PIN_S_DPL_EN           BIT5
// Solar array enable ports
#define GPIO_PORT_EN_A1             GPIO_PORT_P3
#define GPIO_PORT_EN_A2             GPIO_PORT_P4
#define GPIO_PORT_EN_A3             GPIO_PORT_P4
#define GPIO_PORT_EN_A4             GPIO_PORT_P2
#define GPIO_PORT_EN_A5             GPIO_PORT_P3
#define GPIO_PORT_EN_A6             GPIO_PORT_P2
#define GPIO_PORT_EN_A7             GPIO_PORT_P4
// Solar array enable pins
#define GPIO_PIN_EN_A1              BIT7
#define GPIO_PIN_EN_A2              BIT0
#define GPIO_PIN_EN_A3              BIT1
#define GPIO_PIN_EN_A4              BIT3
#define GPIO_PIN_EN_A5              BIT4
#define GPIO_PIN_EN_A6              BIT7
#define GPIO_PIN_EN_A7              BIT2
// Number of solar arrays
#define NUM_SOLAR_ARRAYS 7
// Enums
typedef enum LoadSwitchNumbers
{
    LS_HTR,
    LS_ADCS,
    LS_COMS,
    LS_CDH,
    LS_PLD,
    LS_DPL_A,
    LS_DPL_S,
    NUM_LOAD_SWITCHES
};
// Structs
typedef struct {
    uint8_t port;
    uint8_t pin;
} GPIO_t;



// Functions
float readTemperature(uint8_t therm); /// input ADC channel from 0 to 6
float readSolarCurrent(uint8_t solar);
float readLoadCurrent(uint8_t load);
float readMsbVoltage(void);
void setSolarArraySwitch(uint8_t solarArrayNumber, uint8_t gpioPinInput);
void setLoadSwitch(uint8_t loadSwitchNumber, uint8_t gpioPinInput);
void TestPowerSupply(void);
void TestSolarCells(void);
void Init_Ports(void);

#endif /* INCLUDE_DRIVERS_PERIPHERAL_DRIVER_H_ */
