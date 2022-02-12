/*
 * adcbankAB_driver.c
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */

#include "adcbankAB_driver.h"
#include "eusci_b_spi.h"


#define WAIT_FOR_TRANSMIT_B0               while ((HWREG16(SPI_HW_ADDR + OFS_UCBxIFG) & UCTXIFG))
#define WAIT_FOR_IDLE_B0                   while ((UCB0STATW & UCBUSY))
#define WAIT_FOR_IDLE_B1                   while ((UCB1STATW & UCBUSY))


void Init_ADC_A()
{
    // Dummy conversion
    // starts a dummy conversion with AD7928
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    WAIT_FOR_IDLE_B0;
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);       // ADCA_SE = P1.0
    WAIT_FOR_IDLE_B0;
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0xFF);
    WAIT_FOR_IDLE_B0;
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0xFF);
    WAIT_FOR_IDLE_B0;
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);       // ADCA_SE = P1.0

    _delay_cycles(500);

    WAIT_FOR_IDLE_B0;
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);       // ADCA_SE = P1.0
    WAIT_FOR_IDLE_B0;
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0xFF);
    WAIT_FOR_IDLE_B0;
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0xFF);
    WAIT_FOR_IDLE_B0;
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);       // ADCA_SE = P1.0
}

void Init_ADC_B()
{
    // starts a dummy conversion with AD7928
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);

    WAIT_FOR_IDLE_B1;
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);       // ADCB_SE = P5.4
    WAIT_FOR_IDLE_B1;
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0xFF);
    WAIT_FOR_IDLE_B1;
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0xFF);
    WAIT_FOR_IDLE_B1;
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);       // ADCB_SE = P5.4

    _delay_cycles(500);

    WAIT_FOR_IDLE_B1;
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);       // ADCB_SE = P5.4
    WAIT_FOR_IDLE_B1;
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0xFF);
    WAIT_FOR_IDLE_B1;
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0xFF);
    WAIT_FOR_IDLE_B1;
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);       // ADCB_SE = P5.4
}

unsigned int read_ADC_A(unsigned char channel)
{
    const unsigned int aa = (0b1000001100110011 | (channel<<10));
    const unsigned char aH = ((aa & 0xFF00) >> 8);
    const unsigned char aL = (aa & 0x00FF);
    unsigned int myval;

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);    // ADCA_SE = P1.0
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, aH);
    WAIT_FOR_IDLE_B0;
    myval = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE) << 8;
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, aL);
    WAIT_FOR_IDLE_B0;
    myval |= EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);    // ADCA_SE = P1.0

    __delay_cycles(100);

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);    // ADCA_SE = P1.0
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, aH);
    WAIT_FOR_IDLE_B0;
    myval = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE) << 8;
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, aL);
    WAIT_FOR_IDLE_B0;
    myval |= EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);    // ADCA_SE = P1.0

    myval &= 0x0FFF;

    return myval;
}

unsigned int read_ADC_B(unsigned char channel)
{
    const unsigned int aa = (0b1000001100110011 | (channel<<10));
    const unsigned char aH = ((aa & 0xFF00) >> 8);// | 0x80;
    const unsigned char aL = (aa & 0x00FF);
    unsigned int myval;

    // ADC_B control reg =
    // WRITE | SEQ | ADD3 | ADD2 | ADD1 | ADD0 | PM1 | PM0 | SHADOW | WEAK/TRI | RANGE | CODING
    // refer to the datasheet for meaning of each bit

    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, aH);
    WAIT_FOR_IDLE_B1;
    myval = EUSCI_B_SPI_receiveData(EUSCI_B1_BASE)<<8;
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, aL);
    WAIT_FOR_IDLE_B1;
    myval |= EUSCI_B_SPI_receiveData(EUSCI_B1_BASE);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);

    __delay_cycles(100);

    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, aH);
    WAIT_FOR_IDLE_B1;
    myval = EUSCI_B_SPI_receiveData(EUSCI_B1_BASE)<<8;
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, aL);
    WAIT_FOR_IDLE_B1;
    myval |= EUSCI_B_SPI_receiveData(EUSCI_B1_BASE);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);

    myval &= 0x0FFF;

    return myval;
}

