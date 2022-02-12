/*
 * telemetry.c
 *
 *  Created on: Jan. 22, 2022
 *      Author: jayden
 */

#include "telemetry.h"
#include "TCAN4550.h"


TCAN4x5x_MCAN_TX_Header cdhTx_header = {0};


uint8_t tempBuff[256]={0};

void initTelemetry(void)
{
    cdhTx_header.DLC = MCAN_DLC_4B;                       // Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
    cdhTx_header.TXID = CDH_TXID;                                // Set the ID
//    cdhTx_header.FDF = 1;                                 // CAN FD frame enabled
//    cdhTx_header.BRS = 1;                                 // Bit rate switch enabled
    cdhTx_header.EFC = 0;
    cdhTx_header.MM  = 0;
    cdhTx_header.RTR = 0;
    cdhTx_header.XTD = 0;                                 // We are not using an extended ID in this example
    cdhTx_header.ESI = 0;                                 // Error state indicator
}

void unpackTelemetry(uint8_t * data, telemetryPacket_t* output)
{

    memcpy((void*)&output->timestamp,&data[0],sizeof(Calendar_t));
    memcpy(&output->telem_id, &data[sizeof(Calendar_t)],1);
    memcpy(&output->length, &data[sizeof(Calendar_t)+1],1);
    memcpy(tempBuff,&data[sizeof(Calendar_t)+2],output->length);
    output->data = tempBuff;
}

void sendTelemetryRaw(uint8_t telem_id, uint8_t * data)
{
    // Send telem_id
    uint8_t id[4] = {0};
    id[3] = telem_id;
    TCAN4x5x_MCAN_WriteTXBuffer(0, &cdhTx_header, id);
    TCAN4x5x_MCAN_TransmitBufferContents(0);
//    __delay_cycles(1000);
    // Send data
    uint8_t txData[4] = {0};
    int length = 4;
    int i;
    for(i=0; i < length-1; i++){
        txData[i+1] = data[i];
    }
    // Send using TCAN4550
//    TCAN4x5x_MCAN_WriteTXBuffer(0, &cdhTx_header, txData);
    TCAN4x5x_MCAN_WriteTXBuffer(1, &cdhTx_header, data);
    TCAN4x5x_MCAN_TransmitBufferContents(1);
}

void sendTelemetryRaw1(uint8_t * data)
{
    uint8_t tlm_data[4] = {0};
    uint8_t len = 4;
    int i;
    for(i=0; i < len; i++){
        tlm_data[i+1] = data[i];
    }
//    TCAN4x5x_MCAN_WriteTXBuffer(0, &cdhTx_header, tlm_data);
    TCAN4x5x_MCAN_WriteTXBuffer(0, &cdhTx_header, data);
    TCAN4x5x_MCAN_TransmitBufferContents(0);
}


void sendTelemetry(telemetryPacket_t * packet)
{
//    // Send telem_id
//    uint8_t id[4] = {0};
//    id[3] = packet->id;
//    TCAN4x5x_MCAN_WriteTXBuffer(0, &cdhTx_header, id);
//    TCAN4x5x_MCAN_TransmitBufferContents(0);
//    __delay_cycles(100);
//    // Send data
//    uint8_t txData[4] = {0};
//    int i;
//    for(i=0; i < length-1; i++){
//        txData[i+1] = packet->data[i];
//    }
//    // Send using TCAN4550
////    TCAN4x5x_MCAN_WriteTXBuffer(0, &cdhTx_header, txData);
//    TCAN4x5x_MCAN_WriteTXBuffer(1, &cdhTx_header, packet->data);
//    TCAN4x5x_MCAN_TransmitBufferContents(1);
}




