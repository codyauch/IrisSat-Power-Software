/*
 * peripheral_driver.c
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */

#include "drivers/peripheral_driver.h"

// Solar cells
#define NUM_SOLAR_CELLS     7
const uint8_t SOLAR_CELL_EN_PORTS[7] = {
GPIO_PORT_EN_A1,
GPIO_PORT_EN_A2,
GPIO_PORT_EN_A3,
GPIO_PORT_EN_A4,
GPIO_PORT_EN_A5,
GPIO_PORT_EN_A6,
GPIO_PORT_EN_A7
};
const uint8_t SOLAR_CELL_EN_Ax[7] = {
GPIO_PIN_EN_A1,
GPIO_PIN_EN_A2,
GPIO_PIN_EN_A3,
GPIO_PIN_EN_A4,
GPIO_PIN_EN_A5,
GPIO_PIN_EN_A6,
GPIO_PIN_EN_A7
};
// Subsystem loads
#define NUM_SUBSYTEM_EN     7
const uint8_t SUBSYTEM_EN_PORTS[7] = {
GPIO_PORT_ADCS_EN,
GPIO_PORT_CDH_EN,
GPIO_PORT_COM_EN,
GPIO_PORT_PLD_EN,
GPIO_PORT_A_DPL_EN,
GPIO_PORT_S_DPL_EN,
GPIO_PORT_HTR_EN
};
const uint8_t SUBSYSTEM_EN_PINS[7] = {
GPIO_PIN_ADCS_EN,
GPIO_PIN_CDH_EN,
GPIO_PIN_COM_EN,
GPIO_PIN_PLD_EN,
GPIO_PIN_A_DPL_EN,
GPIO_PIN_S_DPL_EN,
GPIO_PIN_HTR_EN
};

void Init_Ports(void)
{
    // Set all GPIO pins to output low to prevent floating input and reduce power consumption
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    // TCAN4550
    GPIO_setAsOutputPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_WAKE); // Set CAN_WAKE as output
    GPIO_setAsOutputPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_RST); // Set CAN_RST as output

    /*********************************************************
     *                  SPI A0 Interface Pins
     *********************************************************/
    //P1.4(SPI CLK on UCB0CLK) --> P5.0
    //GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_CLK);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_CLK, GPIO_PRIMARY_MODULE_FUNCTION);
    //P1.6(MOSI on UCB0SIMO) --> P5.2
    //GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MOSI);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MOSI, GPIO_PRIMARY_MODULE_FUNCTION );
    //P1.7(MISO on UCB0SOMI) --> P5.1
    //GPIO_setAsInputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MISO);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MISO, GPIO_PRIMARY_MODULE_FUNCTION);
    //set P2.5 as SPI CS, already set to output above --> P4.7
    //GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS, GPIO_PRIMARY_MODULE_FUNCTION);

//    GPIO_setOutputLowOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_CLK);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MOSI);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);

}



void setSolar(uint8_t n, uint8_t val) //n=[1~7], val=[HIGH/LOW]
{
//    unsigned int i;
//    for(i=0; i < NUM_SOLAR_CELLS; i++)
//    {
//        GPIO_setAsOutputPin(SOLAR_CELL_EN_PORTS[i], SOLAR_CELL_EN_Ax[i]);
//    }

    GPIO_setAsOutputPin(SOLAR_CELL_EN_PORTS[n-1], SOLAR_CELL_EN_Ax[n-1]);

    if(val == GPIO_INPUT_PIN_LOW)
    {
        GPIO_setOutputLowOnPin(SOLAR_CELL_EN_PORTS[n-1], SOLAR_CELL_EN_Ax[n-1]);
    }
    else if(val == GPIO_INPUT_PIN_HIGH)
    {
        GPIO_setOutputHighOnPin(SOLAR_CELL_EN_PORTS[n-1], SOLAR_CELL_EN_Ax[n-1]);
    }

    /*
    const char ports[7] = {3, 4, 4, 2, 3, 2, 4};
    const char EN_As[7] = {EN_A1, EN_A2, EN_A3, EN_A4, EN_A5, EN_A6, EN_A7};
    pinMode(ports[0], EN_A1, OUTPUT);
    pinMode(ports[1], EN_A2, OUTPUT);
    pinMode(ports[2], EN_A3, OUTPUT);
    pinMode(ports[3], EN_A4, OUTPUT);
    pinMode(ports[4], EN_A5, OUTPUT);
    pinMode(ports[5], EN_A6, OUTPUT);
    pinMode(ports[6], EN_A7, OUTPUT);

    digitalWrite(ports[n-1], EN_As[n-1], val);
    */
}

void setLoads(uint8_t n, uint8_t val)
{
//    unsigned int i;
//    for(i=0; i < NUM_SUBSYTEM_EN; i++)
//    {
//        GPIO_setAsOutputPin(SUBSYTEM_EN_PORTS[i], SUBSYSTEM_EN_PINS[i]);
//    }

    GPIO_setAsOutputPin(SUBSYTEM_EN_PORTS[n-1], SUBSYSTEM_EN_PINS[n-1]);

    if(val == GPIO_INPUT_PIN_LOW)
    {
        GPIO_setOutputLowOnPin(SUBSYTEM_EN_PORTS[n-1], SUBSYSTEM_EN_PINS[n-1]);
    }
    else if(val == GPIO_INPUT_PIN_HIGH)
    {
        GPIO_setOutputHighOnPin(SUBSYTEM_EN_PORTS[n-1], SUBSYSTEM_EN_PINS[n-1]);
    }
    /*
    const char ports[7] = {2, 3, 5, 3, 3, 3, 3};
    const char loads[7] = {ADCS_EN, CDH_EN, COM_EN, PLD_EN, A_DPL_EN, S_DPL_EN, HTR_EN};
    pinMode(ports[0], loads[0], OUTPUT);
    pinMode(ports[1], loads[1], OUTPUT);
    pinMode(ports[2], loads[2], OUTPUT);
    pinMode(ports[3], loads[3], OUTPUT);
    pinMode(ports[4], loads[4], OUTPUT);
    pinMode(ports[5], loads[5], OUTPUT);
    pinMode(ports[6], loads[6], OUTPUT);

    digitalWrite(ports[n-1], loads[n-1], val);
    */
}
