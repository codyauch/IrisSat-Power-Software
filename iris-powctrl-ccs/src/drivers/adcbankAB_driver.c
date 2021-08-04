/*
 * adcbankAB_driver.c
 *
 *  Created on: Jul. 15, 2021
 *      Author: asgaria/mckoyj
 */

#include "drivers/adcbankAB_driver.h"


void Init_ADC_A()
{
    // Dummy conversion
    /*
    // starts a dummy conversion with AD7928
    pinMode(1, 0, OUTPUT);

    digitalWrite(1, 0, LOW);
    writeSPIB0(0xFF);
    writeSPIA1(readSPIB0());
    writeSPIB0(0xFF);
    writeSPIA1(readSPIB0());
    digitalWrite(1, 0, HIGH);

    _delay_cycles(500);

    digitalWrite(1, 0, LOW);
    writeSPIB0(0xFF);
    writeSPIA1(readSPIB0());
    writeSPIB0(0xFF);
    writeSPIA1(readSPIB0());
    digitalWrite(1, 0, HIGH);
    */
}

void Init_ADC_B()
{
    /*
    // starts a dummy conversion with AD7928
    pinMode(5, ADCB_SE, OUTPUT);

    digitalWrite(5, ADCB_SE, LOW);
    writeSPIB1(0xFF);
    writeSPIA1(readSPIB1());
    writeSPIB1(0xFF);
    writeSPIA1(readSPIB1());
    digitalWrite(5, ADCB_SE, HIGH);

    _delay_cycles(500);

    digitalWrite(5, ADCB_SE, LOW);
    writeSPIB1(0xFF);
    writeSPIA1(readSPIB1());
    writeSPIB1(0xFF);
    writeSPIA1(readSPIB1());
    digitalWrite(5, ADCB_SE, HIGH);
    */
}

unsigned int read_ADC_A(unsigned char channel)
{
    return 0;
    /*
    const unsigned int aa = (0b1000001100110011 | (7<<10));
    const unsigned char aH = ((aa & 0xFF00) >> 8) | 0x80;
    const unsigned char aL = (aa & 0x00FF)|0x01;
    unsigned int myval;
    unsigned char ch;

    // ADC_A control reg =
    // WRITE | SEQ | DON'T CARE | ADD2 | ADD1 | ADD0 | PM1 | PM0 | SHADOW | DON'T CARE | RANGE | CODING
    // refer to the datasheet for meaning of each bit

    // WRITE | SEQ | DON'T CARE | ADD2 | ADD1 | ADD0 | PM1 | PM0 |||| SHADOW | DON'T CARE | RANGE | CODING

    do{
     digitalWrite(1, ADCA_SE, LOW);
     writeSPIB0(aH);
     myval = readSPIB0()<<8;
     writeSPIB0(aL);
     myval |= readSPIB0();
     digitalWrite(1, ADCA_SE, HIGH);

     myval >>= 1;

     ch = (myval & 0xf000) >> 12;

    }while (ch != channel);

    myval &= 0x0FFF;
    */
/*
    writeSPIA1(ch);
    writeSPIA1(myval >> 8);
    writeSPIA1(myval & 0xFF);
*/
    //return myval;

}

unsigned int read_ADC_B(unsigned char channel)
{
    return 0;
    /*
    const unsigned int aa = (0b0011111000111111);
    const unsigned char aH = ((aa & 0xFF00) >> 8);// | 0x80;
    const unsigned char aL = (aa & 0x00FF)|0x01;
    unsigned int myval;
    unsigned char ch;

    // ADC_B control reg =
    // WRITE | SEQ | ADD3 | ADD2 | ADD1 | ADD0 | PM1 | PM0 | SHADOW | WEAK/TRI | RANGE | CODING
    // refer to the datasheet for meaning of each bit

    do{
     digitalWrite(5, ADCB_SE, LOW);
     writeSPIB1(aH);
     myval = readSPIB1()<<8;
     writeSPIB1(aL);
     myval |= readSPIB1();
     digitalWrite(5, ADCB_SE, HIGH);

     //myval >>= 1;

     ch = ((myval & 0xe000) >> 13) | (myval & 0x0001)<<3 ;

    }while (ch != channel);

    //myval &= 0x0FFF;

    //writeSPIA1(ch);
    myval &= 0x1FFE;
    myval >>= 1;
    //writeSPIA1(myval >> 8);
    //writeSPIA1(myval & 0xFF);

    return myval;
    */
}

