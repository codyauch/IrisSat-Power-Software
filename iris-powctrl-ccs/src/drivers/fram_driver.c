/*
 * fram_driver.c
 *
 *  Created on: May 18, 2022
 *      Author: Jayden McKoy
 */

#include "main.h"
#include "fram_driver.h"
#include "nvs.h"
#include "power_modes.h"

// Maximum number of entries in data log
#define MAX_MODE_ENTRIES 8
#define MAX_SOC_ENTRIES 8

#pragma PERSISTENT(nvsMode)
nvs_log_handle nvsHandle_mode;
uint8_t nvsMode[NVS_LOG_STORAGE_SIZE(sizeof(uint8_t), MAX_MODE_ENTRIES)] = {0};

uint8_t TestNvsLog(void)
{
    // Assuming logs have already been initialized
    int i;
    uint16_t status;
    uint8_t rtn;
    // Test Mode log
    // Write some values to the log
    for(i=0; i < MAX_MODE_ENTRIES; i++){
        status = LogAddMode(i);
        if (status != NVS_OK) return -1;
    }
    // Retrieve Mode log
    uint8_t mode_log_data[MAX_MODE_ENTRIES];
    rtn = GetModeLog(mode_log_data);
    if(rtn < 0) return -1;
    return 0;
}

uint16_t LogAddMode(uint8_t mode)
{
//    return nvs_log_add(nvsHandle_mode, mode);
    return nvs_log_add(nvsHandle_mode, &mode);
}

uint8_t GetModeLog(uint8_t * log_data)
{
    int i;
    uint16_t status;
    for(i=0; i < MAX_MODE_ENTRIES; i++){
        // Retrieve log sample
//        status = nvs_log_retrieve(nvsHandle_mode, log_data[i], i);
        status = nvs_log_retrieve(nvsHandle_mode, &log_data[i], i);
        /*
         * Status should never be not NVS_OK but if it happens trap execution.
         * Potential reason for NVS_NOK, NVS_CRC_ERROR or NVS_INDEX_OUT_OF BOUND:
         *     1. nvsStorage not initialized
         *     2. nvsStorage got corrupted by other task (buffer overflow?)
         *     3. index outside MAX_LOG_ENTRY
         */
        if (status != NVS_OK) {
            return -1;
        }
    }
    return 0;
}

uint8_t NvsLogInit(void)
{
    // Check integrity of NVS container and initialize if required;
    nvsHandle_mode = nvs_log_init(nvsMode, sizeof(uint8_t), MAX_MODE_ENTRIES);

    return 0;
}

uint8_t NvsLogReset(void)
{
    uint16_t status;
    // Reset data logging storage
    status = nvs_log_reset(nvsMode);
    if (status != NVS_OK) return -1;
    return 0;
}
