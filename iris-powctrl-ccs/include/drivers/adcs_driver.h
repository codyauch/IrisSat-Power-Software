/*
 * adcs_driver.h
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */


#ifndef INCLUDE_DRIVERS_ADCS_DRIVER_H_
#define INCLUDE_DRIVERS_ADCS_DRIVER_H_

#define ADCS_SPI_ADDR EUSCI_A1_BASE
#define TORQUE_ROD_1 1
#define TORQUE_ROD_2 2
#define TORQUE_ROD_3 3
#define GYRO_1 1
#define GYRO_2 2
#define MAGNETOMETER_1 1
#define MAGNETOMETER_2 2
#define GYRO_DATA_NUM_BYTES 6
#define MAGNETOMETER_DATA_NUM_BYTES 6

bool pingAdcs(void);
bool setTorqueRodPwm(uint8_t torqueRodNumber, uint8_t dutyCycle);
void getGyroMeasurements(uint8_t gyroNumber, uint8_t * gyroMeasurements);
void getMagnetometerMeasurements(uint8_t magnetometerNumber, uint8_t * magnetometerMeasurements);
void testAdcsSpi(void);

uint8_t adcsTxRx(uint8_t tx_data);


#endif /* INCLUDE_DRIVERS_ADCS_DRIVER_H_ */
