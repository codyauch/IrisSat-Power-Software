#include "driverlib.h"
#include "drivers/peripheral_driver.h"

void Init_GPIO(void);
void TestSolarCells(void);
void TestPowerSupply(void);

int main(void) {

    // Initialization
    WDT_A_hold(WDT_A_BASE);
    Init_GPIO();

    // Tests
    CAN_Test1();

    return 0;
}

void Init_GPIO(void)
{
    Init_Ports();
    Init_SPI();
    CAN_Wake();
    Init_CAN();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    __enable_interrupt();
}

void TestSolarCells(void)
{

}
void TestPowerSupply(void)
{

}
