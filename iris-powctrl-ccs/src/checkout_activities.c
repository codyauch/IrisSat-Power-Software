/*
 * checkout_activities.c
 *
 *  Created on: May 11, 2022
 *      Author: Jayden McKoy
 */

#include "main.h"
#include "checkout_activities.h"
#include "power_modes.h"
#include "peripheral_driver.h"

void CheckoutActivities(void)
{
    uint8_t i;
    // Measure battery SoC
    float soc = getBatterySoc();
    // Check if battery heater is on
    float mincoCurrent = readLoadCurrent(0);
    // Verify all internal/external power rail voltages/currents???
    float load_currents[NUM_LOAD_CURRENTS] = {0};
    for(i=0; i < NUM_LOAD_CURRENTS; i++)
        load_currents[i] = readLoadCurrent(i);
    // Read PCU state registers
    uint8_t mode = loadOperatingMode();
    // Measure solar array performance
    float sa_currents[NUM_SOLAR_ARRAYS] = {0};
    for(i=0; i < NUM_SOLAR_ARRAYS; i++)
        sa_currents[i] = readSolarCurrent(i);
}




