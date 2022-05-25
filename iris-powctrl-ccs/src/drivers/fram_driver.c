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

/***** Single value storage *****/
// Operation mode
nvs_log_handle nvsHandleMode;
#pragma DATA_SECTION(nvsMode,".infoA")
uint8_t nvsMode[NVS_DATA_STORAGE_SIZE(sizeof(uint8_t))];
uint8_t g_mode;
// State of charge
nvs_log_handle nvsHandleSoc;
#pragma DATA_SECTION(nvsSoc,".infoB")
float nvsSoc[NVS_DATA_STORAGE_SIZE(sizeof(float))];
float g_soc;

/***** Data Logging *****/ // ---> Needs to be changed to ring!!!
// Operation mode
#define MODE_NVS_RING_SIZE 8 // Maximum number of entries in data log
#pragma PERSISTENT(nvsModeLog)
nvs_log_handle nvsHandleModeLog;
uint8_t nvsModeLog[NVS_RING_STORAGE_SIZE(sizeof(uint8_t), MODE_NVS_RING_SIZE)] = {0};
// State of charge
#define SOC_NVS_RING_SIZE 8
nvs_log_handle nvsHandleSocLog;
float nvsSocLog[NVS_RING_STORAGE_SIZE(sizeof(float), SOC_NVS_RING_SIZE)] = {0};

uint16_t TestNvsLog(void)
{
    // Assuming logs have already been initialized
    int i;
    uint16_t status;

    // Test Mode NVS
    status = NvsCommitMode(++g_mode);
    if (status != NVS_OK) {
        while (1);
    }

    // Test Soc NVS
    status = NvsCommitSoc(++g_soc);
    if (status != NVS_OK) {
        while (1);
    }

    // Test Mode log
    // Write some values to the log
    for(i=0; i < MODE_NVS_RING_SIZE; i++){
        status = LogAddMode(i*g_mode);
        if (status != NVS_OK) {
            while (1);
        }
    }
    // Retrieve Mode log
    uint8_t mode_log_data[MODE_NVS_RING_SIZE-1];
    status = GetModeLog(mode_log_data);
    if (status != NVS_OK) {
        while (1);
    }

    // Test Soc log
    // Write some values to the log
    for(i=0; i < SOC_NVS_RING_SIZE; i++){
        status = LogAddSoc((float)i*g_mode);
        if (status != NVS_OK) {
            while (1);
        }
    }
    // Retrieve Soc log
    uint8_t soc_log_data[SOC_NVS_RING_SIZE-1];
    status = GetModeLog(soc_log_data);
    if (status != NVS_OK) {
        while (1);
    }

    return NVS_OK;
}

uint16_t LogAddMode(uint8_t mode)
{
//    return nvs_log_add(nvsHandle_mode, mode);
    return nvs_ring_add(nvsHandleModeLog, &mode);
}

uint16_t LogAddSoc(float soc)
{
//    return nvs_log_add(nvsHandle_mode, mode);
    return nvs_ring_add(nvsHandleSocLog, &soc);
}

uint16_t GetModeLog(uint8_t * log_data)
{
    int i;
    uint16_t status;
    for(i=0; i < MODE_NVS_RING_SIZE-1; i++){
        // Retrieve log sample
//        status = nvs_log_retrieve(nvsHandle_mode, log_data[i], i);
        status = nvs_ring_retrieve(nvsHandleModeLog, &log_data[i], i);
        /*
         * Status should never be not NVS_OK but if it happens trap execution.
         * Potential reason for NVS_NOK, NVS_CRC_ERROR or NVS_INDEX_OUT_OF BOUND:
         *     1. nvsStorage not initialized
         *     2. nvsStorage got corrupted by other task (buffer overflow?)
         *     3. index outside MAX_LOG_ENTRY
         */
        if (status != NVS_OK) {
            return status;
        }
    }
    return NVS_OK;
}

uint16_t GetSocLog(float * log_data)
{
    int i;
    uint16_t status;
    for(i=0; i < SOC_NVS_RING_SIZE-1; i++){
        // Retrieve log sample
//        status = nvs_log_retrieve(nvsHandle_mode, log_data[i], i);
        status = nvs_ring_retrieve(nvsHandleSocLog, &log_data[i], i);
        /*
         * Status should never be not NVS_OK but if it happens trap execution.
         * Potential reason for NVS_NOK, NVS_CRC_ERROR or NVS_INDEX_OUT_OF BOUND:
         *     1. nvsStorage not initialized
         *     2. nvsStorage got corrupted by other task (buffer overflow?)
         *     3. index outside MAX_LOG_ENTRY
         */
        if (status != NVS_OK) {
            return status;
        }
    }
    return NVS_OK;
}

uint16_t NvsCommitMode(uint8_t mode)
{
    uint16_t status;
    status = nvs_data_commit(nvsHandleMode, &mode);
    return status;
}

uint16_t NvsCommitSoc(float soc)
{
    uint16_t status;
    status = nvs_data_commit(nvsHandleSoc, &soc);
    return status;
}

uint8_t NvsInit(void)
{
    uint16_t status;
//    uint8_t mode;
//    float soc;

    nvsHandleMode = nvs_data_init(nvsMode, sizeof(g_mode)); // Check integrity of NVS container and initialize if required
    status = nvs_data_restore(nvsHandleMode, &g_mode);
    switch (status) {
    case NVS_OK: break;
    case NVS_EMPTY:
        // Initialize mode value.
        g_mode = 0;
        break;
    default:
        /*
         * Status should never be NVS_NOK or NVS_CRC_ERROR, potential reasons:
         *     1. nvsStorage not initialized
         *     2. nvsStorage got corrupted by other task (buffer overflow?)
         */
        while (1);
    }
    nvsHandleSoc = nvs_data_init(nvsSoc, sizeof(g_soc));
    status = nvs_data_restore(nvsHandleSoc, &g_soc);
    switch (status) {
    case NVS_OK: break;
    case NVS_EMPTY:
        g_soc = 0.0;
        break;
    default:
        while (1);
    }

    nvsHandleModeLog = nvs_ring_init(nvsModeLog, sizeof(uint8_t), MODE_NVS_RING_SIZE); // Check integrity of NVS container and initialize if required;
    nvsHandleSocLog = nvs_ring_init(nvsSocLog, sizeof(float), SOC_NVS_RING_SIZE);


    return 0;
}

