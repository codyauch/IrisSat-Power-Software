/*
 * adcs_driver.c
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */

#include "drivers/adcs_driver.h"

void vTestADCS(void *prvParameters)
{
    while(1)
    {
        bool set_pwm = set_pwm_torquerodx(1,0x55);
        bool set_mtx = set_magnetorquerx(3,0);
        __delay_cycles(1000);
    }
}

bool set_pwm_torquerodx(uint8_t num_tr, uint8_t pwm_value)
{
    uint8_t command = 0x00;
    switch(num_tr){
        case 1:
            command = ADCS_SETPWM_TORQUEROD1;
            break;
        case 2:
            command = ADCS_SETPWM_TORQUEROD1;
            break;
        case 3:
            command = ADCS_SETPWM_TORQUEROD1;
            break;
    }

    if(tx_rx_adcs(command) != 0x01) return false;
    if(tx_rx_adcs(0x00) != command) return false;
    return true;

}

bool set_magnetorquerx(uint8_t num_tr, bool turnon)
{
    uint8_t command = 0x00;
   if(turnon){
       switch(num_tr){
           case 1:
               command = ADCS_TURNON_TORQUEROD1;
               break;
           case 2:
               command = ADCS_TURNON_TORQUEROD2;
               break;
           case 3:
               command = ADCS_TURNON_TORQUEROD3;
               break;
           default:
               return false;
       }
   } else {
       switch(num_tr){
           case 1:
               command = ADCS_TURNOFF_TORQUEROD1;
               break;
           case 2:
               command = ADCS_TURNOFF_TORQUEROD2;
               break;
           case 3:
               command = ADCS_TURNOFF_TORQUEROD3;
               break;
       }
   }

   if(tx_rx_adcs(command) != 0x01) return false;
   if(tx_rx_adcs(0x00) != command) return false;
   return true;
}

void read_sunsensor()
{

}


