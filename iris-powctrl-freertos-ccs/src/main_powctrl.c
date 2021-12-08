#include "driverlib.h"
#include "drivers/peripheral_driver.h"
#include "drivers/adcbankAB_driver.h"
#include "drivers/protocol/spi.h"
#include "driverlib/MSP430FR2xx_4xx/eusci_a_spi.h"
#include <math.h>
#include "drivers/protocol/can.h"

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Standard demo includes. */
#include "partest.h"

/* Standard demo application includes. */
#include "dynamic.h"
#include "blocktim.h"
#include "countsem.h"
#include "GenQTest.h"
#include "recmutex.h"
#include "partest.h"
#include "EventGroupsDemo.h"
#include "TaskNotify.h"

int main_powctrl(void);
void readAllSensors(void *pvParameters);
void Init_GPIO(void);
void TestSolarCells(void);
void TestPowerSupply(void);
void Init_interrupts_main(void);
float read_temperature(unsigned char therm); /// input ADC channel from 0 to 6
float read_solar_current(unsigned char solar);
float read_load_current(unsigned char load);
float read_MSB_voltage();

int COULOMB=0;
double BATT_CURR=0;
float temp_tmp;
unsigned int CC_milis=0;

int tmp = 0;

/*
typedef struct ADC_A_pins{


    } ADC_bank_A
*/



int main_powctrl(void) {

//     Initialization - moved to prvSetupHardware (new main.c)
//    WDT_A_hold(WDT_A_BASE);
//    Init_GPIO();
//    Init_interrupts();

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
    // check CAN messages                                               //  TBD     //
    // make Telemetry data                                              //  TBD     //
    // send CAN message                                                 //  TBD     //
    //////////////////////////////////////////////////////////////////////////////////
    // Progress                                                         //  50%     //
    //////////////////////////////////////////////////////////////////////////////////

    /* Start all the standard demo/test tasks.  They have no particular
    functionality, but do demonstrate how to use the FreeRTOS API and test the
//    kernel port. */
//    vStartDynamicPriorityTasks();
//    vCreateBlockTimeTasks();
//    vStartCountingSemaphoreTasks();
//    vStartGenericQueueTasks( tskIDLE_PRIORITY );
//    vStartRecursiveMutexTasks();
//    vStartEventGroupTasks();
//    vStartTaskNotifyTask();


    // Task for receiving CAN messages
    xTaskCreate( CAN_Test2,                         /* Task entry point. */
                 "CAN_Test",                        /* Text name for the task - not used by the kernel. */
                 140,          /* Stack to allocate to the task - in words not bytes! */
                 NULL,                              /* The parameter passed into the task. */
                 1,                                 /* The task's priority. */
                 NULL );                            /* Task handle is not needed, so NULL is passed. */

    xTaskCreate( readAllSensors, "SensorReading", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was either insufficient FreeRTOS heap memory available for the idle
    and/or timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details on the FreeRTOS heap
    http://www.freertos.org/a00111.html. */
    for( ;; );
}

void  readAllSensors(void *pvParameters)
{
    int cnt = 0;
    unsigned int bank[16]={0};
    while (1)
    {
        temp_tmp = read_temperature(6);


        setLoads(3, 1);
        temp_tmp = read_load_current(2);

//        temp_tmp = read_MSB_voltage();


//        read_ADC_B(1)
        __delay_cycles(1000);
    }

}

float read_temperature(unsigned char therm) /// input ADC channel from 0 to 6
{
    float R, T, ADC_voltage;
    ADC_voltage = (3*read_ADC_A(therm))/4095.0;
    R = 1000*((30/ADC_voltage)-10);
    T = (1/(0.002364+((log(R/300))/3453)))-273;

    return T;
}


float read_solar_current(unsigned char solar) /// load number from 0 to 6
{
    const float offset_values[7]        = {13.6,    13.6,   15.4,   13.9,   14.1,   12.0,   12.9};
    const float conversion_factors[7]   = {2.026,   2.000,  1.992,  2.020,  1.992,  2.010,  1.982};
    const float intercepts[7]           = {18.6,    19.4,   19.5,   13.8,   11.5,   14.3,   9.5};
    float I, ADC_voltage;
    unsigned char cntr;

    ADC_voltage = 0;
    for (cntr = 0; cntr<32; cntr++)
      ADC_voltage += (3*read_ADC_B(solar))/4096.0; // voltage in milivolts

    ADC_voltage /= 32;

    ADC_voltage *= 1000;

    if (ADC_voltage < offset_values[solar])
        I = 0;
    else
        I = (ADC_voltage+intercepts[solar])/(conversion_factors[solar]);

    return I;
}


float read_load_current(unsigned char load) /// load number from 0 to 6
{
    // load switch current sense orders:
    // ADC_B pin    Load
    // 7            MINCO
    // 8            ANT_DPL
    // 9            COMS
    // 10           ADCS
    // 11           DATEC
    // 12           PLD
    // 14           CDH
    // loads[7] = {ADCS_EN, CDH_EN, COM_EN, PLD_EN, A_DPL_EN, S_DPL_EN, HTR_EN};

    // TODO: These values shall be updated for FM
    const float offset_values[7]        = {13.6,    3.0,    1.6,    5.7,    9.6,    6.1,    6.2};
    const float conversion_factors[7]   = {1.984,   0.290,  0.326,  1.293,  0.486,  2.260,  1.880};
    const float intercepts[7]           = {20.0,    4.00,   0.00,   18.05,  7.00,   16.0,   21.0};
    float I, ADC_voltage;
    unsigned char load_ADC, cntr;

    if(load < 6)        load_ADC = load+7;
    else if(load == 6)  load_ADC = 14;

    ADC_voltage = 0;
    for (cntr = 0; cntr<32; cntr++)
      ADC_voltage += (3*read_ADC_B(load_ADC))/4096.0; // voltage in milivolts

    ADC_voltage /= 32;

    ADC_voltage *= 1000;

    if (ADC_voltage < offset_values[load])
        I = 0;
    else
        I = (ADC_voltage-intercepts[load])/(conversion_factors[load]);

    return I;
}

float read_MSB_voltage()
{
    float MSB_voltage, ADC_voltage;
    unsigned char cntr;

    ADC_voltage = 0;
    for (cntr = 0; cntr<32; cntr++)
      ADC_voltage += (3*read_ADC_B(13))/4096.0; // voltage in milivolts

    ADC_voltage /= 32;

    MSB_voltage = ADC_voltage*(6.38/2.42);

    return MSB_voltage;
}

void set_POW_mode(unsigned char mode)
{
//    switch (mode)
//    {
//    case POW_MODE_NORMAL:
//        break;
//
//
//    }
}

void Init_GPIO(void)
{
    Init_Ports();
    Init_SPI();
    CAN_Wake();
    Init_CAN();
    //////// Initiate ADC ICs
    Init_ADC_A();
    Init_ADC_B();
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

//    __enable_interrupt();
}


void TestSolarCells(void)
{
    int cnt = 0;
       for (cnt = 1; cnt<8; cnt++)
       {
          setSolar(cnt, 1);
          // PUT A BREAKPOINT HERE
          setSolar(cnt, 0);
       }
}
void TestPowerSupply(void)
{
    int cnt = 0;
    for (cnt = 1; cnt<8; cnt++)
    {
       setLoads(cnt, 1);
       // PUT A BREAKPOINT HERE
       setLoads(cnt, 0);
    }

}

void Init_interrupts_main(void) // this function configures the timer and pin interrupts
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


// ISRs
// interrupt for coulomb counter
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S5(void)
{
    //each time this interrupt happens, 1 Coulomb is passed
tmp ++;

if (tmp > 2)
    tmp = 0;

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
