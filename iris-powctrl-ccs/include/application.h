/*
 * application.h
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#ifndef INCLUDE_APPLICATION_H_
#define INCLUDE_APPLICATION_H_

#include "telemetry.h"

void commandHandler(void);
void commandHandler_noInterrupt(void);
void handleCommand(telemetryPacket_t* command);


#endif /* INCLUDE_APPLICATION_H_ */
