/*
 * telemetry.h
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#ifndef INCLUDE_TELEMETRY_H_
#define INCLUDE_TELEMETRY_H_

#include "driverlib.h"

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
POWER_READ_TEMP_ID = 0,
POWER_READ_SOLAR_CURRENT_ID,
POWER_READ_LOAD_CURRENT_ID,
POWER_READ_MSB_VOLTAGE_ID,
} powerTelemetry_t;
typedef enum
{
POWER_READ_TEMP_CMD = 0,
POWER_READ_SOLAR_CURRENT_CMD,
POWER_READ_LOAD_CURRENT_CMD,
POWER_READ_MSB_VOLTAGE_CMD,
POWER_SET_POW_MODE_CMD,
} powerCommands_t;

/* Functions */
void initTelemetry(void);
void unpackTelemetry(uint8_t * data, telemetryPacket_t* output);
void sendTelemetryRaw(uint8_t * data);
void sendTelemetry(telemetryPacket_t * packet);


#endif /* INCLUDE_TELEMETRY_H_ */
