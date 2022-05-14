/*
 * power_modes.h
 *
 *  Created on: Feb 12, 2022
 *      Author: mckoyj
 */

#ifndef INCLUDE_DRIVERS_POWER_MODES_H_
#define INCLUDE_DRIVERS_POWER_MODES_H_


#include <msp430.h>
#include "driverlib.h"

typedef enum {
    DETUMBLE_MODE,
    CRITICAL_HOLD_MODE,
    SURVIVAL_MODE,
    LOW_POWER_MODE,
    IDLE_MODE,
    NORMAL_MODE,
    SUN_POINTING_MODE,
    SCIENCE_MODE,
    NUM_POWER_MODES
} PowerMode_t;

void monitorSoc(void);
void InitEstimateSoc(float soc_t0);
float EstimateSoc(void);
float getBatterySoc(void);
uint8_t loadOperatingMode(void);
bool GetMode(void);
void setMode(uint8_t m);
bool UpdatePrevMode(void);
bool GetModeChange(void);
void setPowMode();


#endif /* INCLUDE_DRIVERS_POWER_MODES_H_ */
