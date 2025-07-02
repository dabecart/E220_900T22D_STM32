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

typedef struct UART {
    CircularBuffer          RXBuffer;
    CircularBuffer          TXBuffer;
    UART_HandleTypeDef*     hUART;
    DMA_HandleTypeDef*      rxDMA;
    DMA_HandleTypeDef*      txDMA;
    uint8_t                 txSend;    // 1 when a message is ready to be sent
    uint8_t                 rxSend;    // 1 when a message has been received
} UART;

/************************************** FUNCTION ***********************************************
 * @brief Initializes an UART with the main.c variables.
 * @param UART. Pointer to the UART struct to initialize.
 * @param hUART. The UART handler.
 * @param rxDMA. The DMA handler of the RX.
 * @param txDMA. The DMA hablder of the TX. 
 * @return None
***********************************************************************************************/
void initUART(
    UART* uart,
    UART_HandleTypeDef*     hUART,
    DMA_HandleTypeDef*      rxDMA,
    DMA_HandleTypeDef*      txDMA
);

/************************************** FUNCTION ***********************************************
 * @brief Call this function in the main loop. Sends messages stored in the TX buffer.
 * @param UART. Pointer to the UART struct to update.
 * @return None
***********************************************************************************************/
void updateUART(UART* uart);

/************************************** FUNCTION ***********************************************
 * @brief Sends a buffer through UART.
 * @param UART. Pointer to the UART struct to update.
 * @param pucMessage. The buffer containing the message.
 * @param usMessageLength. The length of the message.
 * @return 1 if it was correctly sent.
***********************************************************************************************/
uint8_t sendToUART(UART* uart, uint8_t* pucMessage, uint16_t usMessageLength);

/************************************** FUNCTION ***********************************************
 * @brief Links a DMA to the circular buffers of the serial port.
 * @param UART. Pointer to the UART struct to update.
 * @return None
***********************************************************************************************/
void attachDMAToUART(UART* uart);

#endif // UART_h