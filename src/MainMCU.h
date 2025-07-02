#ifndef MAIN_MCU_h
#define MAIN_MCU_h

#include "stm32g4xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "UART.h"
#include "LoRa.h"

class MainMCU {
    public:
    MainMCU(UART_HandleTypeDef* huart1,
            DMA_HandleTypeDef* hdma_usart1_rx,
            DMA_HandleTypeDef* hdma_usart1_tx);

    ~MainMCU();

    void mainLoop();

    UART uart;
    LoRa lora;
};

extern MainMCU* mcu;

#endif // MAIN_MCU_h