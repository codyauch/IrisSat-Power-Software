/*
 * can.h
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */

#ifndef INCLUDE_DRIVERS_PROTOCOL_CAN_H_
#define INCLUDE_DRIVERS_PROTOCOL_CAN_H_


#include <driverlib.h>
#include <msp430.h>

#include "drivers/device/tcan4x5x/TCAN4550.h"

typedef struct
{
    uint32_t id;                         // Message ID.
    uint8_t  dlc;                        // Data length code (i.e. number of bytes).

    /**< Frame Data - 0 to 8 bytes */
    union __attribute__((aligned(8))) {
        uint8_t data[8];
        uint16_t data16[4];
        uint32_t data32[2];
    };
} CANMessage_t;

// CAN port numbers
#define GPIO_PORT_CAN_SE                                GPIO_PORT_P4
#define GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST           GPIO_PORT_P5
#define GPIO_PORT_CAN_INT                               GPIO_PORT_P6
// CAN pin numbers
#define GPIO_PIN_CAN_SE         BIT7
#define GPIO_PIN_CAN_SCLK       BIT0
#define GPIO_PIN_CAN_MISO       BIT1
#define GPIO_PIN_CAN_MOSI       BIT2
#define GPIO_PIN_CAN_WAKE       BIT6
#define GPIO_PIN_CAN_RST        BIT7
#define GPIO_PIN_CAN_INT        BIT0

void CAN_Wake(void);
void Init_CAN(void);
void CAN_Test1(void);
void CAN_TEST3(void);
void CAN_Test2(void);
void CAN_Error(uint8_t error_code);



#endif /* INCLUDE_DRIVERS_PROTOCOL_CAN_H_ */
