
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
#include "fram_driver.h"
#include "adcs_driver.h"
#include "torque_rods.h"
// FreeRTOS
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"


//#define MAX_DETUMBLE_TIME_SECONDS 4500
#define MAX_DETUMBLE_TIME_SECONDS 30
#define MIN_OMEGA 0.1
// Prototypes
void InitNormalOps(void);
bool detumblingComplete(void);
void vTestHandler(TimerHandle_t xTimer);
void testTickSchedule(void);
/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

//#define INCLUDE_vTaskDelete 1
TaskHandle_t xHandleDetumble;

/* The heap is allocated here so the "persistent" qualifier can be used.  This
requires configAPPLICATION_ALLOCATED_HEAP to be set to 1 in FreeRTOSConfig.h.
See http://www.freertos.org/a00111.html for more information. */

//#pragma PERSISTENT( ucHeap )    /* CCS version. */
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] = { 0 };

void Init_GPIO(void);
void Init_interrupts(void);

unsigned int CC_milis=0;


int main(void) {

//     Initialization
    WDT_A_hold(WDT_A_BASE);
//    WDTCTL = WDTPW | WDTHOLD;
    // Get mode from NVM
    Init_interrupts();
    Init_GPIO();

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

    // Initialize non-volatile storage used for telemetry logging
    NvsInit();

    /*** BELOW LINE(s) ONLY FOR TESTING PURPOSES ***/
    LogOpMode(DETUMBLE_MODE);
//    LogSoc(1.0);
    /*** ABOVE LINE(s) ONLY FOR TESTING PURPOSES ***/

    // Perform post-ejection checkout activities
    CheckoutActivities();
    // Initialize state for SoC estimation
    float initial_soc;
    RetrieveSoc(&initial_soc);
    InitEstimateSoc(initial_soc);
    // Get operation mode
    uint16_t op_mode;
    RetrieveOpMode(&op_mode);

    // Tasks dependent upon op-mode
    xHandleDetumble = NULL;
    xTaskCreate(detumbleDriver, "detumble", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleDetumble);

    // Task which always run, no matter the operation mode
    xTaskCreate( monitorSoc,         /* Task entry point. */
                 "monSoc",           /* Text name for the task - not used by the kernel. */
                 configMINIMAL_STACK_SIZE,                   /* Stack to allocate to the task - in words not bytes! */
                 NULL,                  /* The parameter passed into the task. */
                 1,                     /* The task's priority. */
                 NULL );                /* Task handle. */
    xTaskCreate( MainThermalControl,         /* Task entry point. */
                 "therm",           /* Text name for the task - not used by the kernel. */
                 2*configMINIMAL_STACK_SIZE,                   /* Stack to allocate to the task - in words not bytes! */
                 NULL,                  /* The parameter passed into the task. */
                 1,                     /* The task's priority. */
                 NULL );                /* Task handle. */


//    xTaskCreate(testTickSchedule, "test", 500, NULL, 1, NULL);
    if(op_mode !=  DETUMBLE_MODE)
    {
        //InitNormalOps();
    }


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



void InitNormalOps(void)
{
    /*** BELOW LINE(s) ONLY FOR TESTING PURPOSES ***/
    setLoadSwitch(LS_ADCS,0);
    setLoadSwitch(LS_CDH,1);
    LogOpMode(DETUMBLE_MODE);
    /*** ABOVE LINE(s) ONLY FOR TESTING PURPOSES ***/
//    LogOpMode(NORMAL_MODE);
    initTelemetry();
    xTaskCreate(checkCommands, "CheckCmds", 500, NULL, 1, NULL);
    if(xHandleDetumble != NULL) vTaskDelete(xHandleDetumble);
    //vTaskSuspend(detumbleDriver);

}

typedef enum DETUMBLE_STATES {
    COLLECT_DATA = 1,
    CALC_EXEC_DIPOLE = 2,
    DETUMBLE_WAIT = 3
};

volatile int magData[100][3] = {0};
//volatile int dipole[3] = {0}; //x, y, z dipoles
TimerHandle_t detumbleTimer;

void collectMagData() {
    // Get measurements
    uint8_t rawMagData[6] = {0};
    uint16_t tempMagData[3];
    int i;
    int magDataIndex = 0;
    while(pvTimerGetTimerID(detumbleTimer) == COLLECT_DATA && magDataIndex < 100) {
        // Get measurements
        getMagnetometerMeasurements(1, rawMagData);

        tempMagData[0] = (rawMagData[0] << 8) | rawMagData[1];
        tempMagData[1] = (rawMagData[2] << 8) | rawMagData[3];
        tempMagData[2] = (rawMagData[4] << 8) | rawMagData[5];

        for(i = 0; i < 3; i++) {
            magData[magDataIndex][i] = tempMagData[i];
        }
        magDataIndex++;
    }
    // Wait
    while(pvTimerGetTimerID(detumbleTimer) == COLLECT_DATA);
}

int diffs[100][3] = {0};
volatile const float gain = -1.0;
volatile uint8_t polarity[3] = {0};
volatile uint8_t pwm[3] = {0};
void calculateExecuteDipole() {
    float dipole[3];
    int totalMeasurements = 0;
    int i, j;
    float voltage = 0.0;
    while(pvTimerGetTimerID(detumbleTimer) == CALC_EXEC_DIPOLE) {
        /*** Calculate Dipole ***/
        // Calculate finite different of magnetometer data
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
        // Calculate dipole
        dipole[0] = dipole[0] / totalMeasurements;
        dipole[1] = dipole[1] / totalMeasurements;
        dipole[2] = dipole[2] / totalMeasurements;
        // Calculate polarity, pwm
        for(i=0; i < 3; i++){
            // Gain
            dipole[i] *= gain;
            // Voltage
            voltage = dipoleToVoltage(dipole[i]);
            // Polarity
            polarity[i] = ((voltage >= 0) ? 1 : 0);
            // PWM
            pwm[i] = (uint8_t)( ((polarity[1]-1) * voltage ) * ( MAX_VOLTAGE / MAX_PWM ));
        }
        /*** Execute Dipole ***/
        // Set polarity
        setTorqueRodPolarity(ADCS_CMD_SET_POLARITY_TORQUE_ROD_1,polarity[0]);
        setTorqueRodPolarity(ADCS_CMD_SET_POLARITY_TORQUE_ROD_2,polarity[1]);
        setTorqueRodPolarity(ADCS_CMD_SET_POLARITY_TORQUE_ROD_3,polarity[2]);
        // Set PWM
        setTorqueRodPwm(ADCS_CMD_SET_PWM_TORQUE_ROD_1,pwm[0]);
        setTorqueRodPwm(ADCS_CMD_SET_PWM_TORQUE_ROD_2,pwm[1]);
        setTorqueRodPwm(ADCS_CMD_SET_PWM_TORQUE_ROD_3,pwm[2]);
        // Turn torque rods on
        setTorqueRodState(ADCS_CMD_SET_ON_TORQUE_ROD_1);
        setTorqueRodState(ADCS_CMD_SET_ON_TORQUE_ROD_2);
        setTorqueRodState(ADCS_CMD_SET_ON_TORQUE_ROD_3);

        while(pvTimerGetTimerID(detumbleTimer) == CALC_EXEC_DIPOLE); // wait after calc
    }
}

uint16_t detumbling_cycles = 0;
void detumbleWait() {
    // Turn torque rods off
    setTorqueRodState(ADCS_CMD_SET_OFF_TORQUE_ROD_1);
    setTorqueRodState(ADCS_CMD_SET_OFF_TORQUE_ROD_2);
    setTorqueRodState(ADCS_CMD_SET_OFF_TORQUE_ROD_3);
    // Detumbling complete??
    if(detumblingComplete())
    {
        InitNormalOps();
        while(1);
    }
    detumbling_cycles++;
    // Wait
    while(pvTimerGetTimerID(detumbleTimer) == DETUMBLE_WAIT); // wait
}

bool detumblingComplete(void)
{
    /*** BELOW LINE(s) ONLY FOR TESTING PURPOSES ***/
    return detumbling_cycles > MAX_DETUMBLE_TIME_SECONDS;
    /*** ABOVE LINE(s) ONLY FOR TESTING PURPOSES ***/


    /*** BELOW LINE(s) COMMENTED ONLY FOR TESTING PURPOSES ***/
    /*
    if(detumbling_cycles > MAX_DETUMBLE_TIME_SECONDS)
        return true;
    int i;
    for(i=0; i < 3; i++){
        if(dipole[i] > MAX_DIPOLE)
            return false;
    }
    return true;
    */
    /*** ABOVE LINE(s) COMMENTED ONLY FOR TESTING PURPOSES ***/
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
    /*** BELOW LINE(s) ONLY FOR TESTING PURPOSES ***/
//    setLoadSwitch(LS_ADCS,0);
    setLoadSwitch(LS_ADCS,1);
    setLoadSwitch(LS_CDH,0);
    /*** ABOVE LINE(s) ONLY FOR TESTING PURPOSES ***/
    detumbleTimer = xTimerCreate("detumbleTimer", pdMS_TO_TICKS(333), pdTRUE, ( void * ) COLLECT_DATA, vHandleTimer);
//    StaticTimer_t timerSpace;
//    TimerHandle_t detumbleTimerStatic = xTimerCreateStatic("detumbleTimer", pdMS_TO_TICKS(333), pdTRUE, ( void * ) COLLECT_DATA, vHandleTimer, &timerSpace);
    if(detumbleTimer == NULL)
    {
//        InitNormalOps();
        while(1);
    }
    else
    {
        xTimerStart(detumbleTimer,portMAX_DELAY);
    }
    for(;;) {
        while(pvTimerGetTimerID(detumbleTimer) == COLLECT_DATA) {
            collectMagData();
        }

        while(pvTimerGetTimerID(detumbleTimer) == CALC_EXEC_DIPOLE) {
            calculateExecuteDipole();
        }

        while(pvTimerGetTimerID(detumbleTimer) == DETUMBLE_WAIT) {
            detumbleWait();
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
//    configASSERT( ( volatile void * ) NULL );
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
