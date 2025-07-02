#include "MainMCU.h"

MainMCU mcu;

void initMainMCU(UART_HandleTypeDef* huart1,
                 DMA_HandleTypeDef*  hdma_usart1_rx,
                 DMA_HandleTypeDef*  hdma_usart1_tx) {
    initUART(&mcu.uart, huart1, hdma_usart1_rx, hdma_usart1_tx),
    initLoRa(&mcu.lora, &mcu.uart);
}

void mainLoop() {
    // static uint32_t lastMsgOutTick = HAL_GetTick();
    // static char serialMsg[200];

    // uint32_t currentTick = HAL_GetTick();
    // if((currentTick - lastMsgOutTick) < 1000) {
    //     int randPayload = rand();

    //     LoRaMessage msg;
    //     snprintf((char*) msg.data, sizeof(msg.data), ">%d<", randPayload);
        
    //     lora.sendMessage(&msg);
    //     lastMsgOutTick = currentTick;
        
    //     int len = snprintf(serialMsg, sizeof(serialMsg), "Sent >%d<", randPayload);
    //     CDC_Transmit_FS((uint8_t*) serialMsg, len);
    // }

    // LoRaMessage msg;
    // if(lora.getNextMessage(&msg) == LoRaStatus::LORA_SUCCESS) {
    //     int len = snprintf(serialMsg, sizeof(serialMsg), "Received >%.*s<", msg.len, msg.data);
    //     CDC_Transmit_FS((uint8_t*) serialMsg, len);
    // }

    updateUART(&mcu.uart);
}