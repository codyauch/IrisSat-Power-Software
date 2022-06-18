/*
 * nvs.c
 *
 *  Created on: Jun. 18, 2022
 *      Author: Jayden McKoy
 */


#include "nvs.h"



void WriteOpMode(uint8_t opmode)
{
    uint8_t *Flash_ptrC;                         // Segment C pointer
    uint8_t *Flash_ptrD;                         // Segment D pointer
    Flash_ptrC = (uint8_t *) LOCATION_SEGMENT_C;             // Initialize Flash segment C pointer
    Flash_ptrD = (uint8_t *) LOCATION_SEGMENT_D;             // Initialize Flash segment D pointer

    // Write to main storage location
    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *Flash_ptrC = 0;                           // Dummy write to erase Flash segment
    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    *Flash_ptrC = opmode;

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit

    // Write to redundant storage location
    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *Flash_ptrD = 0;                           // Dummy write to erase Flash segment
    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    *Flash_ptrD = opmode;

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit

}
uint8_t GetOpMode(void)
{
    uint8_t *Flash_ptrC;                         // Segment C pointer
    uint8_t *Flash_ptrD;                         // Segment D pointer
    Flash_ptrC = (uint8_t *) LOCATION_SEGMENT_C;             // Initialize Flash segment C pointer
    Flash_ptrD = (uint8_t *) LOCATION_SEGMENT_D;             // Initialize Flash segment D pointer
    uint8_t main;
    uint8_t redundant;

    // Get main and redundant values
    main = *Flash_ptrC;
    redundant = *Flash_ptrD;

    // Compare
    if(main == redundant)
    {
        return main;
    }

    // Return default mode
    return MODE_NORMAL_OPERATIONS;
}

