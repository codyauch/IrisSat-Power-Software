
#include "main.h"
#include "eusci_a_spi.h"
#include "peripheral_driver.h"
#include "adcbankAB_driver.h"
#include "spi.h"
#include "can.h"
#include "application.h"
#include "power_modes.h"

void Init_GPIO(void);
void Init_interrupts(void);
void postEjectionHold(void);
void mainFlightMission(void);


unsigned int CC_milis=0;
//float temp_tmp;


extern volatile uint8_t TCAN_Int_Cnt;

/*
typedef struct ADC_A_pins{


    } ADC_bank_A
*/



int main(void) {

//    int cnt = 0;
//    unsigned int bank[16]={0};


//     Initialization
    WDT_A_hold(WDT_A_BASE);
    Init_interrupts();
    Init_GPIO();

    // todo: can sometimes gets stuck in CAN_error. fix it!

    //////////////////////////////////////////////////////////////////////////////////
    // the set of activities that this program runs are as follows      //  Status  //
    //////////////////////////////////////////////////////////////////////////////////
    // reset WDT: done through timer overflow interrupt                 //  done!   //
    // Read ADC bank A:                                                 //  done!   //
    // Read ADC bank B:                                                 //  done!   //
    // Read battery SOC: done through the pin interrupts configured     //  done!   //
    // Turn on solar panel inputs:                                      //  done!   //
    // do load shedding                                                 //  TBD     //
    // switch mode between load shedding and detumbling                 //  TBD     //
    // check CAN messages                                               //  done!   //
    // make Telemetry data                                              //  done!   //
    // send CAN message                                                 //  done!   //
    //////////////////////////////////////////////////////////////////////////////////
    // Progress                                                         //  80%     //
    //////////////////////////////////////////////////////////////////////////////////

    // Post ejection 30-minute hold (not complete)
    postEjectionHold();
    // Spacecraft can begin operation
    mainFlightMission();


}

void postEjectionHold(void)
{
    setMode(POST_EJECTION_HOLD_MODE);
    setPowMode();
}

void mainFlightMission(void)
{
    // MOVE initTelemetry to commandHandler task once FreeRTOS is implemented
    initTelemetry();
    // Once FreeRTOS is integrated, these will become FreeRTOS tasks
    while(1)
    {
        /* Command handling loop */
//        commandHandler();
        commandHandler_noInterrupt();

        /* Attend to CDH commands */
        checkCommands();

        /* Check Battery State of Charge */
        monitorSoc();
    }
}





void Init_GPIO(void)
{
    Init_Ports();
    Init_ADC_A();
    Init_ADC_B();
    Init_SPI();
    CAN_Wake();
    Init_CAN();


    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

//    __enable_interrupt();
}



void Init_interrupts(void) // this function configures the timer and pin interrupts
{


    //setup the interrupt pins: P4.5 & P4.6 are for Coulomb counter
    P4DIR &= ~(BIT5 + BIT6);
    P4REN |= BIT5 + BIT6; //enable resistor
    P4OUT |= BIT5 + BIT6;

    P4IES |= BIT5; // sensitivity high-to-low


    //setup IRQ
//    P4IE |= BIT5; // enable P4.5 IRG


    //setup timer
    TB0CTL |= TBCLR;            // reset timer
    TB0CTL |= TBSSEL__SMCLK;    // clock = SMCLK
    TB0CTL |= MC__CONTINOUS;    // mode=continuous

    //setup interrupt
    TB0CTL |= TBIE;

    __enable_interrupt();


    P4IFG &= ~BIT5; // clear P4.5 IRQ flag
    TB0CTL &= ~TBIFG;   // clear flag


}




#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_Overflow(void)
{
    // toggle the WDT heart beat
    P6OUT ^= BIT2;
    P6OUT ^= BIT1; // this is the USER_LED
    TB0R = 0xFC16; // this makes sure we have 1 ms intervals

    // add one millisecond to CC_millis

    CC_milis++;
    // clear flag
    TB0CTL &= ~TBIFG;   // clear flag
}

#pragma vector = PORT6_VECTOR
__interrupt void PORT6_ISR(void)
{
    switch(__even_in_range(P6IV, P6IV_P6IFG7))
    {
    case P6IV_NONE : break;
    case P6IV_P6IFG0 : TCAN_Int_Cnt++; break;
    case P6IV_P6IFG1 : break;
    case P6IV_P6IFG2 : break;
    case P6IV_P6IFG3 : break;
    case P6IV_P6IFG4 : break;
    case P6IV_P6IFG5 : break;
    case P6IV_P6IFG6 : break;
    case P6IV_P6IFG7 : break;
    }
}
