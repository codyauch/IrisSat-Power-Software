/*
 * adcs_driver.c
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */
#include "main.h"
#include "adcs_driver.h"
#include "spi.h"
#include "telemetry.h"


bool pingAdcs(void)
{
    uint8_t response = adcsTxRx(ADCS_CMD_PING);
    return response == ADCS_ACK;
}

bool setTorqueRodPwm(uint8_t torqueRodNumber, uint8_t dutyCycle)
{
    uint8_t cmd_id = -1;
    switch(torqueRodNumber)
    {
    case TORQUE_ROD_1:
        cmd_id = ADCS_CMD_SET_PWM_TORQUE_ROD_1;
        break;
    case TORQUE_ROD_2:
        cmd_id = ADCS_CMD_SET_PWM_TORQUE_ROD_2;
        break;
    case TORQUE_ROD_3:
        cmd_id = ADCS_CMD_SET_PWM_TORQUE_ROD_3;
        break;
    default:
        return;
    }
    // Send command code
    uint8_t ack = adcsTxRx(cmd_id);
    if(ack != ADCS_ACK)
        return false;
    // Send duty cycle
    uint8_t cmd_ack = adcsTxRx(dutyCycle);
    if(cmd_ack == cmd_id)
        return true;
    else
        return false;
}

void getGyroMeasurements(uint8_t gyroNumber, uint8_t * gyroMeasurements)
{
    // Get command ID
    uint8_t cmd_id = -1;
    switch(gyroNumber)
    {
    case GYRO_1:
        cmd_id = ADCS_CMD_GET_MEASUREMENT_GYRO_1;
        break;
    case GYRO_2:
        cmd_id = ADCS_CMD_GET_MEASUREMENT_GYRO_2;
        break;
    default:
        return;
    }
    // Check size of output array
    int size = sizeof(gyroMeasurements);
    //if(sizeof(gyroMeasurements) != GYRO_DATA_NUM_BYTES) return;
    // Send command
    uint8_t ack = 0;
    ack = adcsTxRx(cmd_id);
    // Get measurements
    int i;
    for(i=0; i < GYRO_DATA_NUM_BYTES; i++){
        gyroMeasurements[i] = adcsTxRx(0x00);
    }
}


void getMagnetometerMeasurements(uint8_t magnetometerNumber, uint8_t * magnetometerMeasurements)
{
    // Get command ID
    uint8_t cmd_id = -1;
    switch(magnetometerNumber)
    {
    case MAGNETOMETER_1:
        cmd_id = ADCS_CMD_GET_MEASUREMENT_MAGNETOMETER_1;
        break;
    case MAGNETOMETER_2:
        cmd_id = ADCS_CMD_GET_MEASUREMENT_MAGNETOMETER_2;
        break;
    default:
        return;
    }
    // Check size of output array
    //if(sizeof(magnetometerMeasurements) != MAGNETOMETER_DATA_NUM_BYTES) return;
    // Send command
    uint8_t ack = 0;
    ack = adcsTxRx(cmd_id);
    // Get measurements
    int i;
    for(i=0; i < MAGNETOMETER_DATA_NUM_BYTES; i++){
        magnetometerMeasurements[i] = adcsTxRx(0x00);
    }
}


void testAdcsSpi(void)
{
    // Ping
//    bool adcs_available;
//    adcs_available = pingAdcs();

    // Set torque rod PWM
    setTorqueRodPwm(TORQUE_ROD_1,0x33);
    __delay_cycles(10000);
    setTorqueRodPwm(TORQUE_ROD_2,0x44);
    __delay_cycles(10000);
    setTorqueRodPwm(TORQUE_ROD_3,0x55);
    __delay_cycles(50000);

     //Get Gyro 1 measurements
//    uint8_t gyroMeasurements[6];
//    getGyroMeasurements(GYRO_1, gyroMeasurements);
//    __delay_cycles(50000);

    // Get Magnetometer 1 measurements
//    uint8_t magnetometerMeasurements[6];
//    getMagnetometerMeasurements(MAGNETOMETER_1, magnetometerMeasurements);
}


uint8_t adcsTxRx(uint8_t tx_data)
{
    uint8_t rx_data;
    EUSCI_A_SPI_transmitData(ADCS_SPI_ADDR, tx_data);
    __delay_cycles(100);
    EUSCI_A_SPI_transmitData(ADCS_SPI_ADDR, 0x00);
    __delay_cycles(100);
    rx_data = HWREG8(ADCS_SPI_ADDR + OFS_UCAxRXBUF);
    return rx_data;
}
