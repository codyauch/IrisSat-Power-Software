/*
 * fram_driver.c
 *
 *  Created on: May 18, 2022
 *      Author: Jayden McKoy
 */

#include "power_modes.h"

#pragma PERSISTENT(STORED_MODE)
uint8_t STORED_MODE = 0;

uint8_t GetStoredMode(void)
{
    SYSCFG0 = FRWPPW | FRWPOA0 | DFWP | PFWP;// Configure 1024 bytes for FRAM write
    STORED_MODE++;                       // Record the port event in FRAM
    SYSCFG0 = FRWPPW | DFWP | PFWP;     // Program FRAM write protected (not writable)

    // Explicitly clear RTC control registers to disable it
    // just incase if the RTC was previously enabled
    RTCCTL = 0;

    PMMCTL0_H = PMMPW_H;                    // Open PMM Registers for write
    PMMCTL0_L |= PMMREGOFF;                 // and set PMMREGOFF
    PMMCTL0_H = 0;                          // Lock PMM Registers

    return STORED_MODE;
}
