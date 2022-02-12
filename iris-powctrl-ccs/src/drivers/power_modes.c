/*
 * power_modes.c
 *
 *  Created on: Feb 12, 2022
 *      Author: mckoyj
 */

#include "power_modes.h"
#include "peripheral_driver.h"

typedef enum{
    BATT_HTR_MODE_EN,
    ADCS_MODE_EN,
    COMS_MODE_EN,
    CDH_MODE_EN,
    PLD_MODE_EN
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
    return COULOMB/45.0;
}

void setMode(uint8_t m)
{
    mode = m;
}

void setPowMode()
{
    bool power_en[5] = {0};
    // power_en:
    // [0]: Battery Heater
    // [1]: ADCS
    // [2]: COMMS
    // [3]: CDH
    // [4]: PLD
    switch (mode)
    {
        case POST_EJECTION_HOLD_MODE:
            // All already off
            break;
        case DETUMBLE_MODE:
            power_en[BATT_HTR_MODE_EN] = true;
            power_en[ADCS_MODE_EN] = true;
            break;
        case CRITICAL_HOLD_MODE:
            // All already off
            break;
        case SURVIVAL_MODE:
            power_en[BATT_HTR_MODE_EN] = true;
            power_en[COMS_MODE_EN] = true;
            break;
        case LOW_POWER_MODE:
            power_en[BATT_HTR_MODE_EN] = true;
            power_en[ADCS_MODE_EN] = true;
            power_en[COMS_MODE_EN] = true;
            power_en[CDH_MODE_EN] = true;
            break;
        case IDLE_MODE:
            power_en[BATT_HTR_MODE_EN] = true;
            power_en[COMS_MODE_EN] = true;
            power_en[CDH_MODE_EN] = true;
            break;
        case NORMAL_MODE:
            power_en[BATT_HTR_MODE_EN] = true;
            power_en[ADCS_MODE_EN] = true;
            power_en[COMS_MODE_EN] = true;
            power_en[CDH_MODE_EN] = true;
            power_en[PLD_MODE_EN] = true;
            break;
        case SUN_POINTING_MODE:
            break;
        case SCIENCE_MODE:
            break;
        default:
            break;
    }

    // Set output loads
    setLoad(GPIO_PORT_HTR_EN,GPIO_PIN_HTR_EN,power_en[BATT_HTR_MODE_EN]);
    setLoad(GPIO_PORT_ADCS_EN,GPIO_PIN_ADCS_EN,power_en[ADCS_MODE_EN]);
    setLoad(GPIO_PORT_COMS_EN,GPIO_PIN_COMS_EN,power_en[COMS_MODE_EN]);
    setLoad(GPIO_PORT_CDH_EN,GPIO_PIN_CDH_EN,power_en[CDH_MODE_EN]);
    setLoad(GPIO_PORT_PLD_EN,GPIO_PIN_PLD_EN,power_en[PLD_MODE_EN]);

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
