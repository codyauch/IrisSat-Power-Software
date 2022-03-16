/*
 * power_modes.c
 *
 *  Created on: Feb 12, 2022
 *      Author: mckoyj
 */

#include "power_modes.h"
#include "peripheral_driver.h"

const float MAX_VOLTAGE = 7.3;

typedef enum{
    BATT_HTR_MODE_EN,
    ADCS_MODE_EN,
    COMS_MODE_EN,
    CDH_MODE_EN,
    PLD_MODE_EN,
    DPL_SW_A_EN,
    DPL_SW_S_EN,
} ModeEnable_t;

static uint8_t mode = CRITICAL_HOLD_MODE;
static uint8_t prev_mode = -1;
int COULOMB=0;
double BATT_CURR=0;
extern unsigned int CC_milis;

void monitorSoc(void)
{
    float soc = getBatterySoc();

    if(soc < 0.25) mode = CRITICAL_HOLD_MODE;
    else if(soc < 0.3) mode = SURVIVAL_MODE;
    else if(soc < 0.4) mode = LOW_POWER_MODE;

    // Check if mode has changed
    if(mode != prev_mode)
    {
        setPowMode();
        prev_mode = mode;
    }
}

float getBatterySoc(void)
{
    float msbVoltage = readMsbVoltage();
    // Determine state of charge
    float soc = COULOMB/45.0;
//    float soc = msbVoltage/MAX_VOLTAGE;
    return soc;
}

void setMode(uint8_t m)
{
    mode = m;
//    GPIO_t test = {1,1};
}

void setPowMode()
{
    switch (mode)
    {
        case DETUMBLE_MODE:
            setLoadSwitch(LS_HTR,1);
            setLoadSwitch(LS_ADCS,1);
            setLoadSwitch(LS_COMS,0);
            setLoadSwitch(LS_CDH,0);
            setLoadSwitch(LS_PLD,0);
            setLoadSwitch(LS_DPL_A,0);
            setLoadSwitch(LS_DPL_S,0);
            break;
        case CRITICAL_HOLD_MODE:
            // All already off
            setLoadSwitch(LS_HTR,0);
            setLoadSwitch(LS_ADCS,0);
            setLoadSwitch(LS_COMS,0);
            setLoadSwitch(LS_CDH,0);
            setLoadSwitch(LS_PLD,0);
            setLoadSwitch(LS_DPL_A,0);
            setLoadSwitch(LS_DPL_S,0);
            break;
        case SURVIVAL_MODE:
            setLoadSwitch(LS_HTR,1);
            setLoadSwitch(LS_ADCS,0);
            setLoadSwitch(LS_COMS,1);
            setLoadSwitch(LS_CDH,0);
            setLoadSwitch(LS_PLD,0);
            setLoadSwitch(LS_DPL_A,0);
            setLoadSwitch(LS_DPL_S,0);
            break;
        case LOW_POWER_MODE:
            setLoadSwitch(LS_HTR,1);
            setLoadSwitch(LS_ADCS,1);
            setLoadSwitch(LS_COMS,1);
            setLoadSwitch(LS_CDH,1);
            setLoadSwitch(LS_PLD,0);
            setLoadSwitch(LS_DPL_A,0);
            setLoadSwitch(LS_DPL_S,0);
            break;
        case IDLE_MODE:
            setLoadSwitch(LS_HTR,1);
            setLoadSwitch(LS_ADCS,0);
            setLoadSwitch(LS_COMS,1);
            setLoadSwitch(LS_CDH,1);
            setLoadSwitch(LS_PLD,0);
            setLoadSwitch(LS_DPL_A,0);
            setLoadSwitch(LS_DPL_S,0);
            break;
        case NORMAL_MODE:
            setLoadSwitch(LS_HTR,1);
            setLoadSwitch(LS_ADCS,1);
            setLoadSwitch(LS_COMS,1);
            setLoadSwitch(LS_CDH,1);
            setLoadSwitch(LS_PLD,1);
            setLoadSwitch(LS_DPL_A,0);
            setLoadSwitch(LS_DPL_S,0);
            break;
        case SUN_POINTING_MODE:
            break;
        case SCIENCE_MODE:
            break;
        default:
            break;
    }

}

// ISRs
// interrupt for coulomb counter
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S5(void)
{
    //each time this interrupt happens, 1 Coulomb is passed
    if (P4IN & BIT6)
    {
        COULOMB++;
        BATT_CURR = -(111300/(32.75*0.01))/CC_milis;
    }
    else
    {
        COULOMB--;
        BATT_CURR = (111300/(32.75*0.01))/CC_milis;
    }
    // also add a timer read option here to estimate current

    P4IFG &= ~BIT5; // clear P4.5 IRQ flag
    CC_milis = 0;
}
