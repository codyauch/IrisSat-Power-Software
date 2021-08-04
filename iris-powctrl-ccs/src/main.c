#include "driverlib.h"
#include "drivers/peripheral_driver.h"

void Init_SPIA0_1();
void Init_SPIA0_2();

int main(void) {

    WDT_A_hold(WDT_A_BASE);
    // Initialization
    //Init_GPIO();
    Init_Ports();
    //Init_SPI_Ports();
    Init_SPI();
    //Init_SPIA0_2();
    __enable_interrupt();
    /*
    uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    __delay_cycles(1000);
    GPIO_setOutputLowOnPin(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN);
    read_write_SPIA0(&data);
    GPIO_setOutputHighOnPin(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN);
    */
    /*
    __delay_cycles(100);
    GPIO_setOutputLowOnPin(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN);
    read_write_SPIA0(&data);
    GPIO_setOutputHighOnPin(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN);
    */
    /*
    GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    Init_SPIA0_2();
    __enable_interrupt();       // enable maskable IRQs
    */
    Init_CAN_Ports();
    Init_CAN();

    // Tests
    CAN_Test1();

    return (0);
}
void Init_SPIA0_1()
{
    //WDTCTL = WDTPW | WDTHOLD;                 // Stop watchdog timer
    SYSCFG3|=USCIA0RMP;                       //Set the remapping source
    P5SEL0 |= BIT0 | BIT1 | BIT2;             // set 4-SPI pin as second function
    //P4SEL0 |= BIT7;
    P4SEL0 &= ~BIT7;

    UCA0CTLW0 |= UCSWRST;                     // **Put state machine in reset**
                                              // 4-pin, 8-bit SPI master
    //UCA0CTLW0 |= UCMST|UCSYNC|UCCKPL|UCMSB|UCMODE_2|UCSTEM;
    UCA0CTLW0 |= UCMST|UCSYNC|UCMSB|UCMODE_2|UCSTEM;
                                                  // Clock polarity high, MSB
    //UCA0CTLW0 |= UCSSEL__ACLK;                // Select ACLK
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BR0 = 0x00;                           // BRCLK = ACLK/2
    UCA0BR1 = 0;                              //
    UCA0MCTLW = 0;                            // No modulation
    UCA0CTLW0 &= ~UCSWRST;                    // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
    //TXData = 0x01;                            // Holds TX data

    PM5CTL0 &= ~LOCKLPM5;                     // Disable the GPIO power-on default high-impedance mode
                                              // to activate previously configured port settings
    //__enable_interrupt();       // enable maskable IRQs
}
void Init_SPIA0_2()
{
    SYSCFG3|=USCIA0RMP;             // SETS remap source
    //-- setup SPIA0
    UCA0CTLW0 |= UCSWRST;           // put into SW reset
    UCA0CTLW0 |= UCSSEL__SMCLK;     // choose SMCLK
    UCA0BRW = 10;                   // set prscaler to 10 to get 100KHz
    //UCA0BRW = 8;                   // set prscaler to 10 to get 100KHz
    //UCA0BRW = 6;                   // set prscaler to 10 to get 100KHz
    UCA0CTLW0 |= UCSYNC;            // put into SPI
    UCA0CTLW0 |= UCMST;             // set as master
    UCA0CTLW0 |= UCMSB;
    UCA0CTLW0 |= UCMODE_2;
    UCA0CTLW0 |= UCSTEM;
    //UCA0CTLW0 |= UCCKPL;            // Clock inactive HIGH
    //configure the ports
    P5SEL1 &= ~BIT1;               // P5.1 use SOMI
    P5SEL0 |= BIT1;
    P5SEL1 &= ~BIT0;                // P5.0 use SCLK
    P5SEL0 |= BIT0;
    P5SEL1 &= ~BIT2;               // P5.2 use SIMO
    P5SEL0 |= BIT2;
    P4SEL1 &= ~BIT7;               // P4.7 use STE
    P4SEL0 |= BIT7;
    PM5CTL0 &= ~LOCKLPM5;
    UCA0CTLW0 &= ~UCSWRST;        // take out of SW reset
    // -- ENABLE IRQs
    //P4IE |= BIT1;
    //P4F
    UCA0IE |= UCTXIE;
    UCA0IFG &= ~UCTXIFG;         //clear flag

    /*
    P5DIR |= BIT6 + BIT7;       // P5.7 ==> RESET// P5.6 ==> WAKE
    P6DIR &= ~BIT0;              // P6.0 ==> INT
    P6REN |= BIT0;
    // P6IE  |= BIT0;
    //////
    P5OUT |= BIT6;              // WAKE UP
    P5OUT |= BIT7;              // RESET
    int i;
    for(i = 500; i>0; i--);
    P5OUT &= ~BIT7;             // NOT RESET
    */

    //__enable_interrupt();       // enable maskable IRQs

}
