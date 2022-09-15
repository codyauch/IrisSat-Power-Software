
#include "main.h"
#include "eusci_a_spi.h"
#include "peripheral_driver.h"
#include "adcbankAB_driver.h"
#include "spi.h"
#include "can.h"
#include "application.h"
#include "power_modes.h"
#include "checkout_activities.h"
#include "ait_functions.h"
#include "thermal_control.h"
// FreeRTOS
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/* The heap is allocated here so the "persistent" qualifier can be used.  This
requires configAPPLICATION_ALLOCATED_HEAP to be set to 1 in FreeRTOSConfig.h.
See http://www.freertos.org/a00111.html for more information. */
#pragma PERSISTENT( ucHeap )    /* CCS version. */
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] = { 0 };

void Init_GPIO(void);
void Init_interrupts(void);
void detumbleDriver(void);

unsigned int CC_milis=0;


int main(void) {

//     Initialization
    WDT_A_hold(WDT_A_BASE);
    //Init_interrupts();
    Init_GPIO();
    // MOVE initTelemetry to commandHandler task once FreeRTOS is implemented
    initTelemetry();
    // Perform post-ejection chcekout activities
    CheckoutActivities();
    // Initialize state for SoC estimation
    InitEstimateSoc(1.0);

    // Create tasks
    xTaskCreate( checkCommands,         /* Task entry point. */
                 "CheckCmds",           /* Text name for the task - not used by the kernel. */
                 500,                   /* Stack to allocate to the task - in words not bytes! */
                 NULL,                  /* The parameter passed into the task. */
                 1,                     /* The task's priority. */
                 NULL );                /* Task handle. */



    xTaskCreate(detumbleDriver, "detumble", 1000, NULL, 1, NULL);

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

typedef enum DETUMBLE_STATES {
    COLLECT_DATA = 1,
    CALCULATE_DIPOLE = 2,
    EXECUTE_DIPOLE = 3
};

volatile int magData[100][3] = {0};
volatile int dipole[3] = {0}; //x, y, z dipoles
TimerHandle_t detumbleTimer;

void collectMagData() {
    uint8_t rawMagData[6] = {0};
    uint16_t tempMagData[3];
    int i;
    int magDataIndex = 0;
    while(pvTimerGetTimerID(detumbleTimer) == COLLECT_DATA) {
        getMagnetometerMeasurements(1, rawMagData);

        tempMagData[0] = (rawMagData[0] << 8) | rawMagData[1];
        tempMagData[1] = (rawMagData[2] << 8) | rawMagData[3];
        tempMagData[2] = (rawMagData[4] << 8) | rawMagData[5];

        for(i = 0; i < 3; i++) {
            magData[magDataIndex][i] = tempMagData[i];
        }
        magDataIndex++;
    }
}

void calculateDipole() {
    int diffs[100][3] = {0};
    float dipole[3];
    int totalMeasurements = 0;
    int i, j;
    while(pvTimerGetTimerID(detumbleTimer) == CALCULATE_DIPOLE) {
        for(i = 0; i < 99; i++) {
            if(magData[i][0] != 0 && magData[i+1][0] != 0) {
                for(j = 0; j < 3; j++) {
                    diffs[i][j] = magData[i+1][j] - magData[i][j];
                }
                totalMeasurements++;
            }
        }

        for(i = 0; i < totalMeasurements; i++) {
            dipole[0] += diffs[i][0];
            dipole[1] += diffs[i][1];
            dipole[2] += diffs[i][2];
        }

        dipole[0] = dipole[0] / totalMeasurements;
        dipole[1] = dipole[1] / totalMeasurements;
        dipole[2] = dipole[2] / totalMeasurements;

        while(pvTimerGetTimerID(detumbleTimer) == CALCULATE_DIPOLE); // wait after calc
    }
}

void executeDipole() {
    while(pvTimerGetTimerID(detumbleTimer) == EXECUTE_DIPOLE) {

    }
}

void vHandleTimer(TimerHandle_t xTimer) {
    int currentID = pvTimerGetTimerID(xTimer);
    currentID++;

    if(currentID > 3) {
        currentID = 1;
    }

    vTimerSetTimerID(xTimer, currentID);
}

void detumbleDriver(void) {
    detumbleTimer = xTimerCreate("detumbleTimer", pdMS_TO_TICKS(333), pdTRUE, COLLECT_DATA, vHandleTimer);
    for(;;) {
        while(pvTimerGetTimerID(detumbleTimer) == COLLECT_DATA) {
            collectMagData();
        }

        while(pvTimerGetTimerID(detumbleTimer) == CALCULATE_DIPOLE) {
            calculateDipole();
        }

        while(pvTimerGetTimerID(detumbleTimer) == EXECUTE_DIPOLE) {
            executeDipole();
        }
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

//    P4IES |= BIT5; // sensitivity high-to-low
    P4IES |= BIT6; // sensitivity high-to-low


    //setup IRQ
    P4IE |= BIT6; // enable P4.5 IRG


    //setup timer
    TB0CTL |= TBCLR;            // reset timer
    TB0CTL |= TBSSEL__SMCLK;    // clock = SMCLK
    TB0CTL |= MC__CONTINOUS;    // mode=continuous

    //setup interrupt
    TB0CTL |= TBIE;

    __enable_interrupt();


//    P4IFG &= ~BIT5; // clear P4.5 IRQ flag
    P4IFG &= ~BIT6; // clear P4.5 IRQ flag
    TB0CTL &= ~TBIFG;   // clear flag


}

//#pragma vector = PORT4_VECTOR
//__interrupt void ISR_Port4_S6(void)
//{
//    //each time this interrupt happens, 1 Coulomb is passed
//    if (P4IN & BIT6)
//    {
//        COULOMB++;
//        BATT_CURR = -(111300/(32.75*0.01))/CC_milis;
//    }
//    else
//    {
//        COULOMB--;
//        BATT_CURR = (111300/(32.75*0.01))/CC_milis;
//    }
//    // also add a timer read option here to estimate current
//
//
////    P4IFG &= ~BIT5; // clear P4.5 IRQ flag
//    P4IFG &= ~BIT6; // clear P4.5 IRQ flag
//    CC_milis = 0;
//}


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

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.
    See http://www.freertos.org/Stacks-and-stack-overflow-checking.html */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    __bis_SR_register( LPM4_bits + GIE );
    __no_operation();
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
const unsigned short usACLK_Frequency_Hz = 32768;

    /* Ensure the timer is stopped. */
    TA0CTL = 0;

    /* Run the timer from the ACLK. */
    TA0CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA0CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

    /* Enable the interrupts. */
    TA0CCTL0 = CCIE;

    /* Start up clean. */
    TA0CTL |= TACLR;

    /* Up mode. */
    TA0CTL |= MC_1;
}
/*-----------------------------------------------------------*/
