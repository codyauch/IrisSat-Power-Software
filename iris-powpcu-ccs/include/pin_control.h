/*
 * pin_control.h
 *
 *  Created on: Jun. 5, 2021
 *      Author: asgaria1
 */

#ifndef PIN_CONTROL_H_
#define PIN_CONTROL_H_

#define HIGH    1
#define LOW     0
#define OUTPUT  1
#define INPUT   0

#define MPB     0
#define TMROUT  1
#define APB     2
#define RBF     3
#define ATMR1   4
#define ATMR2   5
#define TMRRST  6
#define WDI     7


void pinMode (char p, char m);
void digitalWrite (char p, char v);
void delay(unsigned int t);
void delaydelay(unsigned int tt);
void setSSR(char ssr, char state);
void initPins(); // sets initial states and values of the pins
char digitalRead(char p);
void resetATMRS();


#endif /* PIN_CONTROL_H_ */
