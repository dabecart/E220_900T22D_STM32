#ifndef MAIN_MCU_h
#define MAIN_MCU_h

#include "stm32g4xx_hal.h"
#include <stdio.h>
// #include "usb_device.h"
// #include "usbd_cdc_if.h"
#include "UART.h"
#include "GUI.h"
#include "Comms.h"

class MainMCU {
    public:
    /**
     * @brief Construct a new MainMCU object.
     * 
     * @param huart1. Handler to UART1.
     * @param hdma_usart1_rx. Handler to the DMA_UART1_RX.
     * @param hdma_usart1_tx. Handler to the DMA_UART1_TX.
     * @param hspi1. Handler to the TFT SPI.
     */
    MainMCU(UART_HandleTypeDef* huart1,
            DMA_HandleTypeDef*  hdma_usart1_rx,
            DMA_HandleTypeDef*  hdma_usart1_tx,
            SPI_HandleTypeDef*  hspi1);

    /**
     * @brief Destroy the MainMCU object.
     */
    ~MainMCU();

    /**
     * @brief Main routine of the MCU. 
     */
    void mainLoop();

    UART uart;
    Comms comms;
    GUI gui;
};

extern MainMCU* mcu;

#endif // MAIN_MCU_h