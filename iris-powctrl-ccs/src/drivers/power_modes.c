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

static uint8_t mode = -1;
int COULOMB=0;
double BATT_CURR=0;
extern unsigned int CC_milis;

void monitorSoc(void)
{
    float soc = getBatterySoc();

    if(soc < 0.25) mode = CRITICAL_HOLD_MODE;
    else if(soc < 0.3) mode = SURVIVAL_MODE;
    else if(soc < 0.4) mode = LOW_POWER_MODE;

    setPowMode();
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
    bool power_en[7] = {0};
    // power_en:
    // [0]: Battery Heater
    // [1]: ADCS
    // [2]: COMMS
    // [3]: CDH
    // [4]: PLD
    // [5]: DPL SW A
    // [6]: DPL SW S
    switch (mode)
    {
        case DETUMBLE_MODE:
            power_en[LS_HTR] = true;
            power_en[LS_ADCS] = true;
            break;
        case CRITICAL_HOLD_MODE:
            // All already off
            break;
        case SURVIVAL_MODE:
            power_en[LS_HTR] = true;
            power_en[LS_COMS] = true;
            break;
        case LOW_POWER_MODE:
            power_en[LS_HTR] = true;
            power_en[LS_ADCS] = true;
            power_en[LS_COMS] = true;
            power_en[LS_CDH] = true;
            break;
        case IDLE_MODE:
            power_en[LS_HTR] = true;
            power_en[LS_COMS] = true;
            power_en[LS_CDH] = true;
            break;
        case NORMAL_MODE:
            power_en[LS_HTR] = true;
            power_en[LS_ADCS] = true;
            power_en[LS_COMS] = true;
            power_en[LS_CDH] = true;
            power_en[LS_PLD] = true;
            break;
        case SUN_POINTING_MODE:
            break;
        case SCIENCE_MODE:
            break;
        default:
            break;
    }

    // Set output loads
    int i;
    for(i=0; i < NUM_LOAD_SWITCHES; i++)
    {
        setLoadSwitch(i,power_en[i]);
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
