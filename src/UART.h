/***************************************************************************************************
 * @file UART.h
 * @brief Handler for serial communications over UART.
 * 
 * @version 1.0
 * @date    2024-12-07
 * @author  @dabecart
 * 
 * @license This project is licensed under the MIT License - see the LICENSE file for details.
***********************************************************************************************/
#ifndef UART_h
#define UART_h

#include "CircularBuffer.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_dma.h"

class UART {
    public:
    /************************************** FUNCTION ***********************************************
     * @brief Initializes an UART_st with the main.c variables.
     * @param hUART. The UART handler.
     * @param rxDMA. The DMA handler of the RX.
     * @param txDMA. The DMA hablder of the TX. 
     * @return None
    ***********************************************************************************************/
    UART(
        UART_HandleTypeDef*     hUART,
        DMA_HandleTypeDef*      rxDMA,
        DMA_HandleTypeDef*      txDMA
    );

    ~UART();

    /************************************** FUNCTION ***********************************************
     * @brief Call this function in the main loop. Sends messages stored in the TX buffer.
     * @return None
    ***********************************************************************************************/
    void update();

    /************************************** FUNCTION ***********************************************
     * @brief Sends a buffer through UART.
     * @param pucMessage. The buffer containing the message.
     * @param usMessageLength. The length of the message.
     * @return 1 if it was correctly sent.
    ***********************************************************************************************/
    uint8_t sendToUART(uint8_t* pucMessage, uint16_t usMessageLength);

    /************************************** FUNCTION ***********************************************
     * @brief Links a DMA to the circular buffers of the serial port.
     * @return None
    ***********************************************************************************************/
    void attachDMAToSerialPort();

    public:
    CircularBuffer          RXBuffer;
    CircularBuffer          TXBuffer;
    UART_HandleTypeDef*     hUART;
    DMA_HandleTypeDef*      rxDMA;
    DMA_HandleTypeDef*      txDMA;
    uint8_t                 txSend = 0;    // 1 when a message is ready to be sent
    uint8_t                 rxSend = 0;    // 1 when a message has been received
};

#endif // UART_h