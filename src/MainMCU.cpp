#include "MainMCU.h"

MainMCU* mcu;

MainMCU::MainMCU(UART_HandleTypeDef* huart1,
                 DMA_HandleTypeDef*  hdma_usart1_rx,
                 DMA_HandleTypeDef*  hdma_usart1_tx):
                uart(huart1, hdma_usart1_rx, hdma_usart1_tx),
                lora(&uart) {
    mcu = this;
}

MainMCU::~MainMCU() {}

void MainMCU::mainLoop() {
    static uint32_t lastMsgOutTick = HAL_GetTick();
    static uint16_t lastPayload = 0;
    static char serialMsg[200];

    uint32_t currentTick = HAL_GetTick();
    if((currentTick - lastMsgOutTick) < 1000) {
        LoRaMessage msg;
        snprintf((char*) msg.data, sizeof(msg.data), ">%d<", lastPayload);
        
        lora.sendMessage(&msg);
        lastMsgOutTick = currentTick;
        
        int len = snprintf(serialMsg, sizeof(serialMsg), "Sent >%d<", lastPayload);
        CDC_Transmit_FS((uint8_t*) serialMsg, len);

        lastPayload++;
    }

    LoRaMessage msg;
    if(lora.getNextMessage(&msg) == LoRaStatus::LORA_SUCCESS) {
        int len = snprintf(serialMsg, sizeof(serialMsg), "Received >%.*s<", msg.len, msg.data);
        CDC_Transmit_FS((uint8_t*) serialMsg, len);
    }

    uart.update();
}