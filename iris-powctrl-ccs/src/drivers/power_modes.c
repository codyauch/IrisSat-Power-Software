/*
 * power_modes.c
 *
 *  Created on: Feb 12, 2022
 *      Author: mckoyj
 */

#include "main.h"
#include "power_modes.h"
#include "peripheral_driver.h"
#include "intrinsics.h"

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

static uint8_t operating_mode = LOW_POWER_MODE;
static uint8_t prev_mode = 0xFF;
//int COULOMB=0;
int COULOMB=23.0;
double BATT_CURR=0;
extern unsigned int CC_milis;
// SOC estimation
double C_RATED = 4500.0;
//double mA_hours_to_secs =
float V_CHARGED = 7.1;
float DOD_t, SOC_t;

void monitorSoc(void)
{
    float soc = getBatterySoc();

    if(soc < 0.25) operating_mode = CRITICAL_HOLD_MODE;
    else if(soc < 0.3) operating_mode = SURVIVAL_MODE;
    else if(soc < 0.4) operating_mode = LOW_POWER_MODE;

    // Check if mode has changed
    if(operating_mode != prev_mode)
    {
        setPowMode();
        prev_mode = operating_mode;
    }
}

void InitEstimateSoc(float soc_t0)
{
    SOC_t = soc_t0;
    DOD_t = 1.0 - soc_t0;
}

float EstimateSoc(void)
{
    // Measure V and I at time t
    float voltage = readMsbVoltage();
    double I = BATT_CURR;
    float dt = (float) CC_milis;
    float delta_dod_t = -(I*dt)/C_RATED/3600.0;
//    DOD_t = DOD_t + delta_dod_t;
    // Check charge vs. discharge and fully charged
    if(I > 0) // Charging mode
    {
        // Check if fully battery fully charged
        if(voltage > V_CHARGED) // Fully charged
        {
            SOC_t = 1.0;
        }
        else
        {
//            SOC_t = fSOC + DOD_t;
            SOC_t = SOC_t + delta_dod_t;
        }
    }
    else // Discharging mode
    {
//        SOC_t = fSOC - DOD_t;
        SOC_t = SOC_t + delta_dod_t;
    }
}

float getBatterySoc(void)
{
//    // Determine state of charge
//    float soc = COULOMB/45.0;
////    float soc = msbVoltage/MAX_VOLTAGE;
//    return soc;
    return SOC_t;
}

uint8_t loadOperatingMode(void)
{
    // intrinsics.h, lines 83-86
    // Using general-purpose registers R4 and R5 to store mode
    // Not sure if they're non-volatile??
    unsigned short mode = __get_R4_register();
    unsigned short modeRedundant = __get_R5_register();
    if(mode == modeRedundant)
    {
        operating_mode = (uint8_t) mode;
    }
    else
    {
        // What mode to go to if mode != modeRedundant???
        operating_mode = IDLE_MODE;
    }

    return operating_mode;
}

bool GetMode(void)
{
    return operating_mode;
}

void setMode(uint8_t m)
{
    // intrinsics.h, lines 83-86
//    __set_R4_register((unsigned short) m);
//    __set_R5_register((unsigned short) m);

    operating_mode = m;
}

bool UpdatePrevMode(void)
{
    return prev_mode = operating_mode;
}

bool GetModeChange(void)
{
    return prev_mode != operating_mode;
}

void setPowMode()
{
    switch (operating_mode)
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
__interrupt void ISR_Port4_S6(void)
{
    //each time this interrupt happens, 1 Coulomb is passed
    if (P4IN & BIT5)
    {
        COULOMB++;
        BATT_CURR = -(111300/(32.75*0.01))/CC_milis;
    }
    else
    {
        COULOMB--;
//        BATT_CURR = (111300/(32.75*0.01))/CC_milis;
//        BATT_CURR = (111300/(32.75))/CC_milis;
        BATT_CURR = 2947.2/CC_milis;
    }
    // also add a timer read option here to estimate current

    // Estimate soc
    EstimateSoc();

//    P4IFG &= ~BIT5; // clear P4.5 IRQ flag
    P4IFG &= ~BIT6; // clear P4.5 IRQ flag
    CC_milis = 0;
}
