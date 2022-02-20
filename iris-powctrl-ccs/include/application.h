/*
 * application.h
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#ifndef INCLUDE_APPLICATION_H_
#define INCLUDE_APPLICATION_H_

#include "telemetry.h"

typedef struct {
    uint8_t cmd_id;
    uint8_t params[4];
} CdhCmd_t;

void commandHandler(void);
void commandHandler_noInterrupt(void);
void handleCommand(CdhCmd_t * command);
void checkCommands(void);


#endif /* INCLUDE_APPLICATION_H_ */
