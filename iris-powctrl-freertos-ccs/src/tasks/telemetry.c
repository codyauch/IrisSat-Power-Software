/*
 * telemetry.c
 *
 *  Created on: Dec 2, 2021
 *      Author: mckoyj
 */


#include "tasks/telemetry.h"

uint8_t tx_rx_adcs(uint8_t command)
{
//    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN1);
    EUSCI_A_SPI_transmitData(SPI_ADCS, command);
//    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN1);
    return EUSCI_A_SPI_receiveData(SPI_ADCS);
}

void transmitADCS(uint8_t command)
{
    EUSCI_A_SPI_transmitData(SPI_ADCS, command);
}

uint8_t receiveADCS(void)
{
    return EUSCI_A_SPI_receiveData(SPI_ADCS);
}
