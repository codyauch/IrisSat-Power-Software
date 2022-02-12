/*
 * peripheral_driver.c
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */
#include <math.h>
#include "peripheral_driver.h"
#include "adcbankAB_driver.h"
#include "spi.h"
#include "can.h"



// Solar cells
#define NUM_SOLAR_CELLS     7
const uint8_t SOLAR_CELL_EN_PORTS[7] = {
GPIO_PORT_EN_A1,
GPIO_PORT_EN_A2,
GPIO_PORT_EN_A3,
GPIO_PORT_EN_A4,
GPIO_PORT_EN_A5,
GPIO_PORT_EN_A6,
GPIO_PORT_EN_A7
};
const uint8_t SOLAR_CELL_EN_Ax[7] = {
GPIO_PIN_EN_A1,
GPIO_PIN_EN_A2,
GPIO_PIN_EN_A3,
GPIO_PIN_EN_A4,
GPIO_PIN_EN_A5,
GPIO_PIN_EN_A6,
GPIO_PIN_EN_A7
};


void Init_Ports(void)
{
    // Set all GPIO pins to output low to prevent floating input and reduce power consumption
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    // TCAN4550
    GPIO_setAsOutputPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_WAKE); // Set CAN_WAKE as output
    GPIO_setAsOutputPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_RST); // Set CAN_RST as output

    /*********************************************************
     *                  SPI A0 Interface Pins
     *********************************************************/
    //P1.4(SPI CLK on UCB0CLK) --> P5.0
    //GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_CLK);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_CLK, GPIO_PRIMARY_MODULE_FUNCTION);
    //P1.6(MOSI on UCB0SIMO) --> P5.2
    //GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MOSI);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MOSI, GPIO_PRIMARY_MODULE_FUNCTION );
    //P1.7(MISO on UCB0SOMI) --> P5.1
    //GPIO_setAsInputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MISO);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MISO, GPIO_PRIMARY_MODULE_FUNCTION);
    //set P2.5 as SPI CS, already set to output above --> P4.7
    //GPIO_setAsOutputPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS, GPIO_PRIMARY_MODULE_FUNCTION);

//    GPIO_setOutputLowOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_CLK);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CLK_MISO_MOSI, GPIO_PIN_SPIA0_MOSI);
    GPIO_setOutputHighOnPin(GPIO_PORT_SPIA0_CS, GPIO_PIN_SPIA0_CS);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN6);


}

float read_temperature(uint8_t therm) /// input ADC channel from 0 to 6
{
    float R, T, ADC_voltage;
    ADC_voltage = (3*read_ADC_A(therm))/4095.0;
    R = 1000*((30/ADC_voltage)-10);
    T = (1/(0.002364+((log(R/300))/3453)))-273;

    return T;
}


float read_solar_current(uint8_t solar) /// load number from 0 to 6
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


float read_load_current(uint8_t load) /// load number from 0 to 6
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
    const unsigned long waitCycles = 500;
    // Heaters
    setLoad(GPIO_PORT_HTR_EN,GPIO_PIN_HTR_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_HTR_EN,GPIO_PIN_HTR_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
    // ADCS
    setLoad(GPIO_PORT_ADCS_EN,GPIO_PIN_ADCS_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_ADCS_EN,GPIO_PIN_ADCS_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
    // COMS
    setLoad(GPIO_PORT_COMS_EN,GPIO_PIN_COMS_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_COMS_EN,GPIO_PIN_COMS_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
    // CDH
    setLoad(GPIO_PORT_CDH_EN,GPIO_PIN_CDH_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_CDH_EN,GPIO_PIN_CDH_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
    // PLD
    setLoad(GPIO_PORT_PLD_EN,GPIO_PIN_PLD_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_PLD_EN,GPIO_PIN_PLD_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
    // DPL SW A
    setLoad(GPIO_PORT_A_DPL_EN,GPIO_PIN_A_DPL_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_A_DPL_EN,GPIO_PIN_A_DPL_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
    // DPL SW S
    setLoad(GPIO_PORT_S_DPL_EN,GPIO_PIN_S_DPL_EN,GPIO_INPUT_PIN_HIGH);
    __delay_cycles(waitCycles);
    setLoad(GPIO_PORT_S_DPL_EN,GPIO_PIN_S_DPL_EN,GPIO_INPUT_PIN_LOW);
    __delay_cycles(waitCycles);
}

void setSolar(uint8_t n, uint8_t val) //n=[1~7], val=[HIGH/LOW]
{
//    unsigned int i;
//    for(i=0; i < NUM_SOLAR_CELLS; i++)
//    {
//        GPIO_setAsOutputPin(SOLAR_CELL_EN_PORTS[i], SOLAR_CELL_EN_Ax[i]);
//    }

    GPIO_setAsOutputPin(SOLAR_CELL_EN_PORTS[n-1], SOLAR_CELL_EN_Ax[n-1]);

    if(val == GPIO_INPUT_PIN_LOW)
    {
        GPIO_setOutputLowOnPin(SOLAR_CELL_EN_PORTS[n-1], SOLAR_CELL_EN_Ax[n-1]);
    }
    else if(val == GPIO_INPUT_PIN_HIGH)
    {
        GPIO_setOutputHighOnPin(SOLAR_CELL_EN_PORTS[n-1], SOLAR_CELL_EN_Ax[n-1]);
    }

    /*
    const char ports[7] = {3, 4, 4, 2, 3, 2, 4};
    const char EN_As[7] = {EN_A1, EN_A2, EN_A3, EN_A4, EN_A5, EN_A6, EN_A7};
    pinMode(ports[0], EN_A1, OUTPUT);
    pinMode(ports[1], EN_A2, OUTPUT);
    pinMode(ports[2], EN_A3, OUTPUT);
    pinMode(ports[3], EN_A4, OUTPUT);
    pinMode(ports[4], EN_A5, OUTPUT);
    pinMode(ports[5], EN_A6, OUTPUT);
    pinMode(ports[6], EN_A7, OUTPUT);

    digitalWrite(ports[n-1], EN_As[n-1], val);
    */
}




void setLoad(uint8_t port, uint8_t pin, uint8_t val)
{

    GPIO_setAsOutputPin(port, pin);

    if(val == GPIO_INPUT_PIN_LOW)
    {
        GPIO_setOutputLowOnPin(port,pin);
    }
    else if(val == GPIO_INPUT_PIN_HIGH)
    {
        GPIO_setOutputHighOnPin(port,pin);
    }
}
