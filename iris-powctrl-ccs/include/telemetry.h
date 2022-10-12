/*
 * telemetry.h
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#ifndef INCLUDE_TELEMETRY_H_
#define INCLUDE_TELEMETRY_H_

#include "driverlib.h"

#define CDH_RXID 0x824
#define CDH_TXID 0x721

/* TELEMETRY STRUCTS */
typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t weekday;
    uint8_t week;

} Calendar_t;
typedef struct
{
    Calendar_t timestamp;
    uint8_t telem_id;       //Make sure there is less than 255 commands/telemetry ids for any subsystem. Or change to uint16_t.
    uint8_t length;
    uint8_t* data;
} telemetryPacket_t;

/* TELEMETRY IDS AND COMMANDS */
typedef enum
{
POWER_READ_TEMP_ID = 50,
POWER_READ_SOLAR_CURRENT_ID,
POWER_READ_LOAD_CURRENT_ID,
POWER_READ_MSB_VOLTAGE_ID,
POWER_GET_BATTERY_SOC_ID,
POWER_GET_ECLIPSE_ID,
POWER_GET_BOOT_COUNT_ID,
POWER_SET_LOAD_OFF_ID,
POWER_SET_LOAD_ON_ID,
POWER_SET_SOLAR_OFF_ID,
POWER_SET_SOLAR_ON_ID,
POWER_RESET_LOAD_SWITCH_ID,
NUM_POWER_TELEMETRY
} powerTelemetry_t;
typedef enum
{
POWER_READ_TEMP_CMD = 0,
POWER_READ_SOLAR_CURRENT_CMD,
POWER_READ_LOAD_CURRENT_CMD,
POWER_READ_MSB_VOLTAGE_CMD,
POWER_GET_BATTERY_SOC_CMD,
POWER_GET_ECLIPSE_CMD,
POWER_GET_BOOT_COUNT,
POWER_SET_LOAD_OFF_CMD,
POWER_SET_LOAD_ON_CMD,
POWER_SET_SOLAR_OFF_CMD,
POWER_SET_SOLAR_ON_CMD,
POWER_SET_POW_MODE_CMD,
POWER_RESET_LOAD_SWITCH_CMD,
AIT_POWER_SET_BATTERY_SOC_CMD,
AIT_POWER_SET_ECLIPSE,
NUM_POWER_COMMANDS
} powerCommands_t;

typedef enum
{
    ADCS_CMD_PING = 1,
    ADCS_CMD_SET_PWM_TORQUE_ROD_1,
    ADCS_CMD_SET_PWM_TORQUE_ROD_2,
    ADCS_CMD_SET_PWM_TORQUE_ROD_3,
    ADCS_CMD_SET_ON_TORQUE_ROD_1,
    ADCS_CMD_SET_ON_TORQUE_ROD_2,
    ADCS_CMD_SET_ON_TORQUE_ROD_3,
    ADCS_CMD_SET_OFF_TORQUE_ROD_1,
    ADCS_CMD_SET_OFF_TORQUE_ROD_2,
    ADCS_CMD_SET_OFF_TORQUE_ROD_3,
    ADCS_CMD_SET_POLARITY_TORQUE_ROD_1,
    ADCS_CMD_SET_POLARITY_TORQUE_ROD_2,
    ADCS_CMD_SET_POLARITY_TORQUE_ROD_3,
    ADCS_CMD_SET_PWM_COUNTER_TORQUE_ROD,
    ADCS_CMD_GET_MEASUREMENT_SUN_SENSOR,
    ADCS_CMD_GET_MEASUREMENT_GYRO_1,
    ADCS_CMD_GET_MEASUREMENT_GYRO_2,
    ADCS_GYRO_ACK,
    ADCS_CMD_GET_MEASUREMENT_MAGNETOMETER_1,
    ADCS_CMD_GET_MEASUREMENT_MAGNETOMETER_2,
    ADCS_MAGNETO_ACK,
    ADCS_SYNC_SPI,
    NUM_ADCS_COMMANDS,
    ADCS_ACK=55,
    ADCS_SPI_CMD_ERROR = 75,
} AdcsCommands_t;



/* Functions */
void initTelemetry(void);
void unpackTelemetry(uint8_t * data, telemetryPacket_t* output);
void sendTelemetryRaw(uint8_t tlm_id, uint8_t * data);
void sendTelemetry(telemetryPacket_t * packet);


#endif /* INCLUDE_TELEMETRY_H_ */
