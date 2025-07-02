/***************************************************************************************************
 * @file UART.h
 * @brief Handler for serial communications over UART.
 * 
 * @version 1.0
 * @date    2024-12-07
 * @author  @dabecart
 * 
 * @license This project is licensed under the MIT License - see the LICENSE file for details.
***************************************************************************************************/
#include "UART.h"
#include "MainMCU.h"

void initUART(UART* uart,
              UART_HandleTypeDef*     hUART,
              DMA_HandleTypeDef*      rxDMA,
              DMA_HandleTypeDef*      txDMA) {

    if(uart == NULL) return;

    uart->hUART        = hUART;
    uart->rxDMA        = rxDMA; 
    uart->txDMA        = txDMA;

    attachDMAToUART(uart);
}

void updateUART(UART* uart) {
    if(uart->txSend) {
        // Lock the buffer until the DMA has sent the message. The callback will unlock it.
        uart->TXBuffer.locked = 1;

        HAL_UART_Transmit_DMA(uart->hUART, uart->TXBuffer.data, uart->TXBuffer.len);

        // Disables an interruption that gets called half in transmission.
        __HAL_DMA_DISABLE_IT(uart->txDMA, DMA_IT_HT);
        
        // The buffer has been sent!
        uart->txSend = 0;
    }
}

void attachDMAToUART(UART* uart) {
    // This function takes the buffer directly as a circular one. It won't update the indices of the 
    // struct, so that will be left upon us. 
    HAL_UARTEx_ReceiveToIdle_DMA(uart->hUART, uart->RXBuffer.data, uart->RXBuffer.size);

    // // This disables an interruption that triggers when the buffer gets filled to its full size.
    // __HAL_DMA_DISABLE_IT(pstUART->rxDMA, DMA_IT_TC);
    // // This disables an interruption that triggers when the buffer gets filled to half its size.
    // __HAL_DMA_DISABLE_IT(pstUART->rxDMA, DMA_IT_HT);
}

uint8_t sendToUART(UART* uart, uint8_t* pucMessage, uint16_t usMessageLength) {
    // Push the data to the array.
    uint8_t readyToSend = pushN_cb(&uart->TXBuffer, pucMessage, usMessageLength);
    if(readyToSend) uart->txSend = 1;
    return readyToSend;
}

/***************************************************************************************************
* This function is declared inside stm32l4xx_hal_uart.h for the user to define it. 
* This function gets called when a reception over a serial port has ended.
***************************************************************************************************/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *hUART, uint16_t newHeadIndex) {   
    UART* referencedUART = NULL;
    if(hUART == mcu.uart.hUART) {
        referencedUART = &mcu.uart;
    } // Add more UART handlers if used.

    if(referencedUART != NULL) {
        updateIndices_cb(&referencedUART->RXBuffer, newHeadIndex);
        referencedUART->rxSend = 1;
    }
}

/***************************************************************************************************
* This function is declared inside stm32l4xx_hal_uart.h for the user to define it. 
* This function gets called when a transmission over a serial port has ended.
***************************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *hUART) {
    UART* referencedUART = NULL;
    if(hUART == mcu.uart.hUART) {
        referencedUART = &mcu.uart;
    } // Add more UART handlers if used.

    if(referencedUART != NULL) {
        // Unlock the TX buffer.
        referencedUART->TXBuffer.locked = 0;
        // The transmission doesn't use circular buffers, but we're using them so that we don't have
        // to implement a "simple buffer". To convert a circular buffer to simple buffer, just 
        // delete its content and restart the indices.
        empty_cb(&referencedUART->TXBuffer);
    }
}

/***************************************************************************************************
* This function is declared inside stm32l4xx_hal_uart.h for the user to define it. 
* This function gets called when an UART fails.
***************************************************************************************************/
void HAL_UART_ErrorCallback(UART_HandleTypeDef* hUART) {
    UART* referencedUART = NULL;
    if(hUART == mcu.uart.hUART) {
        referencedUART = &mcu.uart;
    } // Add more UART handlers if used.

    if(referencedUART != NULL) {
        empty_cb(&referencedUART->TXBuffer);
        referencedUART->TXBuffer.locked = 0;
        referencedUART->txSend = 0;

        empty_cb(&referencedUART->RXBuffer);
        referencedUART->RXBuffer.locked = 0;
        referencedUART->rxSend = 0;

        attachDMAToUART(referencedUART);
    }
}

