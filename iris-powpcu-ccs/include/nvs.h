/*
 * nvs.h
 *
 *  Created on: Jun. 18, 2022
 *      Author: Jayden McKoy
 */

#ifndef INCLUDE_NVS_H_
#define INCLUDE_NVS_H_

#include "main.h"

#define LOCATION_SEGMENT_C 0x1040
#define LOCATION_SEGMENT_D 0x1000

#define MODE_POST_EJECTION_HOLD 0x55
#define MODE_NORMAL_OPERATIONS 0xAA

void WriteOpMode(uint8_t opmode);
uint8_t GetOpMode(void);


#endif /* INCLUDE_NVS_H_ */
