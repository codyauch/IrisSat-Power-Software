/*
 * ait_functions.c
 *
 *  Created on: May 11, 2022
 *      Author: Jayden McKoy
 */

#include "main.h"
#include "ait_functions.h"
#include "power_modes.h"

bool eclipse = true;
bool prev_eclipse = true;
float gg_soc = 0.50;


void AitAdcsControl(void)
{
    AitEclipseControl();
}

void AitEclipseControl(void)
{
    // Check that we've entered an eclipse (not just that we're in one)
    bool entered_eclipse = eclipse != prev_eclipse && eclipse;
    // Check that we're not already in a lower state
    bool in_higher_state = GetMode() > LOW_POWER_MODE;
    if(entered_eclipse && in_higher_state)
    {
        setMode(LOW_POWER_MODE);
        setPowMode();
        UpdatePrevMode();
    }
}

bool AitGetEclipse(void)
{
    return eclipse;
}

void AitSetEclipse(bool in_eclipse)
{
    eclipse = in_eclipse;
}

void AitMonitorSoc(void)
{
    uint8_t operating_mode = GetMode();

    if(gg_soc < 0.25) operating_mode = CRITICAL_HOLD_MODE;
    else if(gg_soc < 0.3) operating_mode = SURVIVAL_MODE;
    else if(gg_soc < 0.4) operating_mode = LOW_POWER_MODE;

    setMode(operating_mode);

    // Check if mode has changed
    if(GetModeChange())
    {
        setMode(operating_mode);
        setPowMode();
        UpdatePrevMode();
    }
}

void AitSetBatterySoc(float percent_soc)
{
    gg_soc = percent_soc;
}
