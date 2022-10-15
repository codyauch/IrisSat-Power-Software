/*
 * fram_driver.c
 *
 *  Created on: May 18, 2022
 *      Author: Jayden McKoy
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "fram_driver.h"
#include "nvs.h"
#include "power_modes.h"
// Note:
// - nvs: non-volatile storage
// - nvsh: nvs handle
// - nvss: nvs storage
/***** Single value storage (with redundant copy) *****/
#define REDUNDANT_COPIES 2
// --- Boot Count ---
nvs_log_handle nvsh_bootcount;
#pragma NOINIT(nvsStorage)
uint8_t nvss_bootcount[NVS_RING_STORAGE_SIZE(sizeof(uint16_t), REDUNDANT_COPIES)];
uint16_t bootcount;
//uint16_t g_bootcount;
// --- Operation Mode ---
nvs_log_handle nvsh_opmode;
#pragma NOINIT(nvss_opmode)
uint8_t nvss_opmode[NVS_RING_STORAGE_SIZE(sizeof(uint8_t), REDUNDANT_COPIES)];
//uint8_t g_mode;
// --- State of Charge ---
nvs_log_handle nvsh_soc;
#pragma NOINIT(nvss_soc)
float nvss_soc[NVS_RING_STORAGE_SIZE(sizeof(float), REDUNDANT_COPIES)];
//float g_soc;

uint16_t LogBootCount(uint8_t bootcount)
{
    int i;
    uint16_t status;
    for(i=0; i < REDUNDANT_COPIES; i++){
        status = nvs_ring_add(nvsh_bootcount, &bootcount);
    }
    return status;
}

uint16_t LogOpMode(uint8_t opmode)
{
    int i;
    uint16_t status;
    for(i=0; i < REDUNDANT_COPIES; i++){
        status = nvs_ring_add(nvsh_opmode, &opmode);
    }
    return status;
}

uint16_t LogSoc(float soc)
{
    int i;
    uint16_t status;
    for(i=0; i < REDUNDANT_COPIES; i++){
        status = nvs_ring_add(nvsh_soc, &soc);
    }
    return status;
}

uint16_t RetrieveBootCount(uint8_t * prevBootCount)
{
    int i;
    uint16_t status;
    for(i=0; i < REDUNDANT_COPIES; i++){
        // Retrieve log sample
        status = nvs_ring_retrieve(nvsh_bootcount, prevBootCount, i);
        /*
         * Status should never be not NVS_OK but if it happens trap execution.
         * Potential reason for NVS_NOK, NVS_CRC_ERROR or NVS_INDEX_OUT_OF BOUND:
         *     1. nvsStorage not initialized
         *     2. nvsStorage got corrupted by other task (buffer overflow?)
         *     3. index outside MAX_LOG_ENTRY
         */
        if (status == NVS_OK) {
            return NVS_OK;
        }
    }
    return status;
}

uint16_t RetrieveOpMode(uint8_t * prevOpMode)
{
    int i;
    uint16_t status;
    for(i=0; i < REDUNDANT_COPIES; i++){
        // Retrieve log sample
        status = nvs_ring_retrieve(nvsh_opmode, prevOpMode, i);
        /*
         * Status should never be not NVS_OK but if it happens trap execution.
         * Potential reason for NVS_NOK, NVS_CRC_ERROR or NVS_INDEX_OUT_OF BOUND:
         *     1. nvsStorage not initialized
         *     2. nvsStorage got corrupted by other task (buffer overflow?)
         *     3. index outside MAX_LOG_ENTRY
         */
        if (status == NVS_OK) {
            return NVS_OK;
        }
    }
    return status;
}

uint16_t RetrieveSoc(float * prevSoc)
{
    int i;
    uint16_t status;
    for(i=0; i < REDUNDANT_COPIES; i++){
        status = nvs_ring_retrieve(nvsh_soc, prevSoc, i);
        if (status == NVS_OK) {
            return NVS_OK;
        }
    }
    return status;
}
uint8_t NvsInit(void)
{
    // Initialize bootcount NVM, get, update bootcount
    nvsh_bootcount = nvs_ring_init(nvss_bootcount, sizeof(uint8_t), REDUNDANT_COPIES); // Check integrity of NVS container and initialize if required;
    RetrieveBootCount(bootcount);
    LogBootCount(++bootcount);
    // Initialize operation mode, SoC NVM
    nvsh_opmode = nvs_ring_init(nvss_opmode, sizeof(float), REDUNDANT_COPIES);
    nvsh_soc = nvs_ring_init(nvss_soc, sizeof(float), REDUNDANT_COPIES);
    return 0;
}

