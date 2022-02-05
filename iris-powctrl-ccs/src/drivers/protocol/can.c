/*
 * can.c
 *
 *  Created on: Jul. 9, 2021
 *      Author: asgaria/mckoyj
 */

#include "drivers/protocol/can.h"

volatile uint8_t TCAN_Int_Cnt = 0;                  // A variable used to keep track of interrupts the MCAN Interrupt pin

// --- Reset the TCAN4550 ---
void CAN_Wake(void)
{
    // Set CAN_WAKE high
    GPIO_setOutputHighOnPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_WAKE);
    // Pulse CAN_RST (low-high-low 30 microsecond pulse)
    GPIO_setOutputLowOnPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_RST);
    GPIO_setOutputHighOnPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_RST);
    __delay_cycles(1000);
    GPIO_setOutputLowOnPin(GPIO_PORT_CAN_SCLK_MISO_MOSI_WAKE_RST,GPIO_PIN_CAN_RST);
    // Wait 700 microseconds before SPI access (to be removed later)
    __delay_cycles(1000);

}

void Init_CAN(void)
{
    // From sample code
    volatile int can_init_ec = 0;
    TCAN4x5x_Device_ClearSPIERR();                              // Clear any SPI ERR flags that might be set as a result of our pin mux changing during MCU startup

    /* Step one attempt to clear all interrupts */
    TCAN4x5x_Device_Interrupt_Enable dev_ie = {0};              // Initialize to 0 to all bits are set to 0.
    if(!TCAN4x5x_Device_ConfigureInterruptEnable(&dev_ie))          // Disable all non-MCAN related interrupts for simplicity
    {
        can_init_ec |= BIT0;
    }

    TCAN4x5x_Device_Interrupts dev_ir = {0};                    // Setup a new MCAN IR object for easy interrupt checking
    TCAN4x5x_Device_ReadInterrupts(&dev_ir);                    // Request that the struct be updated with current DEVICE (not MCAN) interrupt values

    if (dev_ir.PWRON)                                           // If the Power On interrupt flag is set
        TCAN4x5x_Device_ClearInterrupts(&dev_ir);               // Clear it because if it's not cleared within ~4 minutes, it goes to sleep
    TCAN4x5x_Device_ClearInterrupts(&dev_ir);
    /* Configure the CAN bus speeds */
    TCAN4x5x_MCAN_Nominal_Timing_Simple TCANNomTiming = {0};    // 500k arbitration with a 40 MHz crystal ((40E6 / 2) / (32 + 8) = 500E3)
    TCANNomTiming.NominalBitRatePrescaler = 4;
    TCANNomTiming.NominalTqBeforeSamplePoint = 32;
    TCANNomTiming.NominalTqAfterSamplePoint = 8;

    TCAN4x5x_MCAN_Data_Timing_Simple TCANDataTiming = {0};      // 2 Mbps CAN FD with a 40 MHz crystal (40E6 / (15 + 5) = 2E6)
    TCANDataTiming.DataBitRatePrescaler = 1;
    TCANDataTiming.DataTqBeforeSamplePoint = 15;
    TCANDataTiming.DataTqAfterSamplePoint = 5;

    /* Configure the MCAN core settings */
    TCAN4x5x_MCAN_CCCR_Config cccrConfig = {0};                 // Remember to initialize to 0, or you'll get random garbage!
    cccrConfig.FDOE = 1;                                        // CAN FD mode enable
    cccrConfig.BRSE = 1;                                        // CAN FD Bit rate switch enable

    /* Configure the default CAN packet filtering settings */
    TCAN4x5x_MCAN_Global_Filter_Configuration gfc = {0};
    gfc.RRFE = 1;                                               // Reject remote frames (TCAN4x5x doesn't support this)
    gfc.RRFS = 1;                                               // Reject remote frames (TCAN4x5x doesn't support this)
    gfc.ANFE = TCAN4x5x_GFC_ACCEPT_INTO_RXFIFO0;                // Default behavior if incoming message doesn't match a filter is to accept into RXFIO0 for extended ID messages (29 bit IDs)
    gfc.ANFS = TCAN4x5x_GFC_ACCEPT_INTO_RXFIFO0;                // Default behavior if incoming message doesn't match a filter is to accept into RXFIO0 for standard ID messages (11 bit IDs)

    /* ************************************************************************
     * In the next configuration block, we will set the MCAN core up to have:
     *   - 1 SID filter element
     *   - 1 XID Filter element
     *   - 5 RX FIFO 0 elements
     *   - RX FIFO 0 supports data payloads up to 64 bytes
     *   - RX FIFO 1 and RX Buffer will not have any elements, but we still set their data payload sizes, even though it's not required
     *   - No TX Event FIFOs
     *   - 2 Transmit buffers supporting up to 64 bytes of data payload
     */
    TCAN4x5x_MRAM_Config MRAMConfiguration = {0};
    MRAMConfiguration.SIDNumElements = 1;                       // Standard ID number of elements, you MUST have a filter written to MRAM for each element defined
    MRAMConfiguration.XIDNumElements = 1;                       // Extended ID number of elements, you MUST have a filter written to MRAM for each element defined
    MRAMConfiguration.Rx0NumElements = 5;                       // RX0 Number of elements
    MRAMConfiguration.Rx0ElementSize = MRAM_64_Byte_Data;       // RX0 data payload size
    MRAMConfiguration.Rx1NumElements = 0;                       // RX1 number of elements
    MRAMConfiguration.Rx1ElementSize = MRAM_64_Byte_Data;       // RX1 data payload size
    MRAMConfiguration.RxBufNumElements = 0;                     // RX buffer number of elements
    MRAMConfiguration.RxBufElementSize = MRAM_64_Byte_Data;     // RX buffer data payload size
    MRAMConfiguration.TxEventFIFONumElements = 0;               // TX Event FIFO number of elements
    MRAMConfiguration.TxBufferNumElements = 2;                  // TX buffer number of elements
    MRAMConfiguration.TxBufferElementSize = MRAM_64_Byte_Data;  // TX buffer data payload size


    /* Configure the MCAN core with the settings above, the changes in this block are write protected registers,      *
     * so it makes the most sense to do them all at once, so we only unlock and lock once                             */

    if(!TCAN4x5x_MCAN_EnableProtectedRegisters())                   // Start by making protected registers accessible
    {
        can_init_ec |= BIT1;
    }
    if(!TCAN4x5x_MCAN_ConfigureCCCRRegister(&cccrConfig))           // Enable FD mode and Bit rate switching
    {
        can_init_ec |= BIT2;
    }
    if(!TCAN4x5x_MCAN_ConfigureGlobalFilter(&gfc))                  // Configure the global filter configuration (Default CAN message behavior)
    {
        can_init_ec |= BIT3;
    }
    if(!TCAN4x5x_MCAN_ConfigureNominalTiming_Simple(&TCANNomTiming))// Setup nominal/arbitration bit timing
    {
        can_init_ec |= BIT4;
    }
    if(!TCAN4x5x_MCAN_ConfigureDataTiming_Simple(&TCANDataTiming))  // Setup CAN FD timing
    {
        can_init_ec |= BIT5;
    }
    TCAN4x5x_MRAM_Clear();                                      // Clear all of MRAM (Writes 0's to all of it)
    if(!TCAN4x5x_MRAM_Configure(&MRAMConfiguration))                // Set up the applicable registers related to MRAM configuration
    {
        can_init_ec |= BIT6;
    }
    if(!TCAN4x5x_MCAN_DisableProtectedRegisters())                  // Disable protected write and take device out of INIT mode
    {
        can_init_ec |= BIT7;
    }


    /* Set the interrupts we want to enable for MCAN */
    TCAN4x5x_MCAN_Interrupt_Enable mcan_ie = {0};               // Remember to initialize to 0, or you'll get random garbage!
    mcan_ie.RF0NE = 1;                                          // RX FIFO 0 new message interrupt enable

    TCAN4x5x_MCAN_ConfigureInterruptEnable(&mcan_ie);           // Enable the appropriate registers


    /* Setup filters, this filter will mark any message with ID 0x055 as a priority message */
    TCAN4x5x_MCAN_SID_Filter SID_ID = {0};
    SID_ID.SFT = TCAN4x5x_SID_SFT_CLASSIC;                      // SFT: Standard filter type. Configured as a classic filter
    SID_ID.SFEC = TCAN4x5x_SID_SFEC_PRIORITYSTORERX0;           // Standard filter element configuration, store it in RX fifo 0 as a priority message
    SID_ID.SFID1 = 0x055;                                       // SFID1 (Classic mode Filter)
    SID_ID.SFID2 = 0x7FF;                                       // SFID2 (Classic mode Mask)
    if(!TCAN4x5x_MCAN_WriteSIDFilter(0, &SID_ID))                   // Write to the MRAM
    {
        can_init_ec |= BIT8;
    }


    /* Store ID 0x12345678 as a priority message */
    TCAN4x5x_MCAN_XID_Filter XID_ID = {0};
    XID_ID.EFT = TCAN4x5x_XID_EFT_CLASSIC;                      // EFT
    XID_ID.EFEC = TCAN4x5x_XID_EFEC_PRIORITYSTORERX0;           // EFEC
    XID_ID.EFID1 = 0x12345678;                                  // EFID1 (Classic mode filter)
    XID_ID.EFID2 = 0x1FFFFFFF;                                  // EFID2 (Classic mode mask)
    if(!TCAN4x5x_MCAN_WriteXIDFilter(0, &XID_ID))                   // Write to the MRAM
    {
        can_init_ec |= BIT9;
    }

    /* Configure the TCAN4550 Non-CAN-related functions */
    TCAN4x5x_DEV_CONFIG devConfig = {0};                        // Remember to initialize to 0, or you'll get random garbage!
    devConfig.SWE_DIS = 0;                                      // Keep Sleep Wake Error Enabled (it's a disable bit, not an enable)
    devConfig.DEVICE_RESET = 0;                                 // Not requesting a software reset
    devConfig.WD_EN = 0;                                        // Watchdog disabled
    devConfig.nWKRQ_CONFIG = 0;                                 // Mirror INH function (default)
    devConfig.INH_DIS = 0;                                      // INH enabled (default)
    devConfig.GPIO1_GPO_CONFIG = TCAN4x5x_DEV_CONFIG_GPO1_MCAN_INT1;    // MCAN nINT 1 (default)
    devConfig.FAIL_SAFE_EN = 0;                                 // Failsafe disabled (default)
    devConfig.GPIO1_CONFIG = TCAN4x5x_DEV_CONFIG_GPIO1_CONFIG_GPO;      // GPIO set as GPO (Default)
    devConfig.WD_ACTION = TCAN4x5x_DEV_CONFIG_WDT_ACTION_nINT;  // Watchdog set an interrupt (default)
    devConfig.WD_BIT_RESET = 0;                                 // Don't reset the watchdog
    devConfig.nWKRQ_VOLTAGE = 0;                                // Set nWKRQ to internal voltage rail (default)
    devConfig.GPO2_CONFIG = TCAN4x5x_DEV_CONFIG_GPO2_NO_ACTION; // GPO2 has no behavior (default)
    devConfig.CLK_REF = 1;                                      // Input crystal is a 40 MHz crystal (default)
    devConfig.WAKE_CONFIG = TCAN4x5x_DEV_CONFIG_WAKE_BOTH_EDGES;// Wake pin can be triggered by either edge (default)
    if(!TCAN4x5x_Device_Configure(&devConfig))                      // Configure the device with the above configuration
    {
        can_init_ec |= BITA;
    }

    if(!TCAN4x5x_Device_SetMode(TCAN4x5x_DEVICE_MODE_NORMAL))       // Set to normal mode, since configuration is done. This line turns on the transceiver
    {
        can_init_ec |= BITB;
    }

    TCAN4x5x_MCAN_ClearInterruptsAll();                         // Resets all MCAN interrupts (does NOT include any SPIERR interrupts)

    // Check for any initialization errors.
    if(can_init_ec != 0) CAN_Error(0xFF);

    return can_init_ec;
}

void CAN_Test1(void)
{
    /* Define the CAN message we want to send*/
    TCAN4x5x_MCAN_TX_Header header = {0};           // Remember to initialize to 0, or you'll get random garbage!
    uint8_t data[4] = {0x55, 0x66, 0x77, 0x88};     // Define the data payload
    header.DLC = MCAN_DLC_4B;                       // Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
    header.TXID = 0x144;                                // Set the ID
    header.FDF = 1;                                 // CAN FD frame enabled
    header.BRS = 1;                                 // Bit rate switch enabled
    header.EFC = 0;
    header.MM  = 0;
    header.RTR = 0;
    header.XTD = 0;                                 // We are not using an extended ID in this example
    header.ESI = 0;                                 // Error state indicator


    TCAN4x5x_MCAN_WriteTXBuffer(0, &header, data);  // This function actually writes the header and data payload to the TCAN's MRAM in the specified TX queue number. It returns the bit necessary to write to TXBAR,
                                                    // but does not necessarily require you to use it. In this example, we won't, so that we can send the data queued up at a later point.

    /* Let's make a different CAN message */
    data[0] = 0x11;
    data[1] = 0x22;
    data[2] = 0x33;
    data[3] = 0x44;                                 // Define the data payload

    header.DLC = MCAN_DLC_4B;                       // Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
    header.TXID = 0x123;                                // Set the ID
    header.FDF = 1;                                 // CAN FD frame enabled
    header.BRS = 1;                                 // Bit rate switch enabled
    header.EFC = 0;
    header.MM  = 0;
    header.RTR = 0;
    header.XTD = 0;                                 // We are not using an extended ID in this example
    header.ESI = 0;                                 // Error state indicator

    TCAN4x5x_MCAN_WriteTXBuffer(1, &header, data);  // This line writes the data and header to TX FIFO 1
    TCAN4x5x_MCAN_TransmitBufferContents(1);        // Request that TX Buffer 1 be transmitted


    TCAN4x5x_MCAN_TransmitBufferContents(0);        // Now we can send the TX FIFO element 0 data that we had queued up earlier but didn't send.

    while(1)
    {
        TCAN4x5x_MCAN_WriteTXBuffer(0, &header, data);
        TCAN4x5x_MCAN_TransmitBufferContents(0);
    }
}

void CAN_TEST3(void)
{
    int cnt=8;
    /* Define the CAN message we want to send*/
        TCAN4x5x_MCAN_TX_Header header = {0};           // Remember to initialize to 0, or you'll get random garbage!
        uint8_t data[4] = {0x55, 0x66, 0x77, 0x88};     // Define the data payload
        header.DLC = MCAN_DLC_4B;                       // Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
        header.TXID = 0x123456A;                        // Set the ID
        header.FDF = 0;                                 // CAN FD frame enabled
        header.BRS = 0;                                 // Bit rate switch enabled
        header.FDF = 0;                                 // CAN FD frame enabled
        header.BRS = 0;                                 // Bit rate switch enabled
        header.EFC = 0;
        header.MM  = 0;
        header.RTR = 0;
        header.XTD = 1;                                 // We are using an extended ID in this example
        header.ESI = 0;                                 // Error state indicator


        for (cnt = 0; cnt<10; cnt++)
        {
            header.TXID = cnt*16;
            data[1]= cnt;
            TCAN4x5x_MCAN_WriteTXBuffer(0, &header, data);  // This function actually writes the header and data payload to the TCAN's MRAM in the specified TX queue number. It returns the bit necessary to write to TXBAR,
                                                            // but does not necessarily require you to use it. In this example, we won't, so that we can send the data queued up at a later point.
            TCAN4x5x_MCAN_TransmitBufferContents(0);        // Request that TX Buffer 1 be transmitted
        }
}


void check_message(unsigned int RXID)
{
    TCAN4x5x_Device_Interrupts dev_ir = {0};            // Define a new Device IR object for device (non-CAN) interrupt checking
    TCAN4x5x_MCAN_Interrupts mcan_ir = {0};             // Setup a new MCAN IR object for easy interrupt checking
    TCAN4x5x_Device_ReadInterrupts(&dev_ir);            // Read the device interrupt register
    TCAN4x5x_MCAN_ReadInterrupts(&mcan_ir);             // Read the interrupt register

    if (dev_ir.SPIERR)                                  // If the SPIERR flag is set
        TCAN4x5x_Device_ClearSPIERR();                  // Clear the SPIERR flag

    if (mcan_ir.RF0N)                                   // If a new message in RX FIFO 0
    {
        TCAN4x5x_MCAN_RX_Header MsgHeader = {0};        // Initialize to 0 or you'll get garbage
        uint8_t numBytes = 0;                           // Used since the ReadNextFIFO function will return how many bytes of data were read
        uint8_t dataPayload[64] = {0};                  // Used to store the received data

        TCAN4x5x_MCAN_ClearInterrupts(&mcan_ir);        // Clear any of the interrupt bits that are set.

        numBytes = TCAN4x5x_MCAN_ReadNextFIFO( RXFIFO0, &MsgHeader, dataPayload);   // This will read the next element in the RX FIFO 0

        // numBytes will have the number of bytes it transfered in it. Or you can decode the DLC value in MsgHeader.DLC
        // The data is now in dataPayload[], and message specific information is in the MsgHeader struct.
        if (MsgHeader.RXID == 0x0AA)        // Example of how you can do an action based off a received address
        {
            // Do something

        }
    }
}

void CAN_Test2(void)
{
    /* Define the CAN message we want to send*/
    TCAN4x5x_MCAN_TX_Header header = {0};           // Remember to initialize to 0, or you'll get random garbage!
    uint8_t data[4] = {0x55, 0x66, 0x77, 0x88};     // Define the data payload
    header.DLC = MCAN_DLC_4B;                       // Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
    header.TXID = 0x123456A;                                // Set the ID
//    header.FDF = 1;                                 // CAN FD frame enabled
//    header.BRS = 1;                                 // Bit rate switch enabled
    header.FDF = 0;                                 // CAN FD frame enabled
    header.BRS = 0;                                 // Bit rate switch enabled
    header.EFC = 0;
    header.MM  = 0;
    header.RTR = 0;
    header.XTD = 1;                                 // We are not using an extended ID in this example
    header.ESI = 0;                                 // Error state indicator


    TCAN4x5x_MCAN_WriteTXBuffer(0, &header, data);  // This function actually writes the header and data payload to the TCAN's MRAM in the specified TX queue number. It returns the bit necessary to write to TXBAR,
                                                    // but does not necessarily require you to use it. In this example, we won't, so that we can send the data queued up at a later point.

    /* Let's make a different CAN message */
    data[0] = 0x11;
    data[1] = 0x22;
    data[2] = 0x33;
    data[3] = 0x44;                                 // Define the data payload

    header.DLC = MCAN_DLC_4B;                       // Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
    header.TXID = 0x123;                                // Set the ID
//    header.FDF = 1;                                 // CAN FD frame enabled
//    header.BRS = 1;                                 // Bit rate switch enabled
    header.FDF = 0;                                 // CAN FD frame enabled
    header.BRS = 0;                                 // Bit rate switch enabled
    header.EFC = 0;
    header.MM  = 0;
    header.RTR = 0;
    header.XTD = 0;                                 // We are not using an extended ID in this example
    header.ESI = 0;                                 // Error state indicator

    TCAN4x5x_MCAN_WriteTXBuffer(1, &header, data);  // This line writes the data and header to TX FIFO 1
//    TCAN4x5x_MCAN_TransmitBufferContents(1);        // Request that TX Buffer 1 be transmitted


//    TCAN4x5x_MCAN_TransmitBufferContents(0);        // Now we can send the TX FIFO element 0 data that we had queued up earlier but didn't send.

    while (1)
    {
        TCAN4x5x_MCAN_TransmitBufferContents(0);
        if (TCAN_Int_Cnt > 0 )
        {
//            TCAN_Int_Cnt--;
            TCAN4x5x_Device_Interrupts dev_ir = {0};            // Define a new Device IR object for device (non-CAN) interrupt checking
            TCAN4x5x_MCAN_Interrupts mcan_ir = {0};             // Setup a new MCAN IR object for easy interrupt checking
            TCAN4x5x_Device_ReadInterrupts(&dev_ir);            // Read the device interrupt register
            TCAN4x5x_MCAN_ReadInterrupts(&mcan_ir);             // Read the interrupt register

            if (dev_ir.SPIERR)                                  // If the SPIERR flag is set
                TCAN4x5x_Device_ClearSPIERR();                  // Clear the SPIERR flag

            if (mcan_ir.RF0N)                                   // If a new message in RX FIFO 0
            {
                TCAN4x5x_MCAN_RX_Header MsgHeader = {0};        // Initialize to 0 or you'll get garbage
                uint8_t numBytes = 0;                           // Used since the ReadNextFIFO function will return how many bytes of data were read
                uint8_t dataPayload[64] = {0};                  // Used to store the received data

                TCAN4x5x_MCAN_ClearInterrupts(&mcan_ir);        // Clear any of the interrupt bits that are set.

                numBytes = TCAN4x5x_MCAN_ReadNextFIFO( RXFIFO0, &MsgHeader, dataPayload);   // This will read the next element in the RX FIFO 0


                // numBytes will have the number of bytes it transfered in it. Or you can decode the DLC value in MsgHeader.DLC
                // The data is now in dataPayload[], and message specific information is in the MsgHeader struct.
                if (MsgHeader.RXID == 0x0AA)        // Example of how you can do an action based off a received address
                {
                    // Do something
                }
            }
        }
    }
}

/*
 * PORT6 Interrupt Service Routine
 * Handles Interrupt from the TCAN4550 on P6.0
 */
//#pragma vector = PORT6_VECTOR
//__interrupt void PORT6_ISR(void)
//{
//    switch(__even_in_range(P6IV, P6IV_P6IFG7))
//    {
//    case P6IV_NONE : break;
//    case P6IV_P6IFG0 : TCAN_Int_Cnt++; break;
//    case P6IV_P6IFG1 : break;
//    case P6IV_P6IFG2 : break;
//    case P6IV_P6IFG3 : break;
//    case P6IV_P6IFG4 : break;
//    case P6IV_P6IFG5 : break;
//    case P6IV_P6IFG6 : break;
//    case P6IV_P6IFG7 : break;
//    }
//}


void CAN_Error(uint8_t error_code)
{
    while(1){
        __delay_cycles(1000);
    }
}
