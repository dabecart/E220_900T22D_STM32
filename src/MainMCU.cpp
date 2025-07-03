#include "MainMCU.h"

MainMCU* mcu;
char serialMsg[200];

MainMCU::MainMCU(UART_HandleTypeDef* huart1,
                 DMA_HandleTypeDef*  hdma_usart1_rx,
                 DMA_HandleTypeDef*  hdma_usart1_tx):
                uart(huart1, hdma_usart1_rx, hdma_usart1_tx),
                lora(&uart) {
    mcu = this;
    lora.init();
    
    HAL_Delay(4000);
    int len = snprintf(serialMsg, sizeof(serialMsg), "Channel %d\n", lora.currentConfig.channel);
    CDC_Transmit_FS((uint8_t*) serialMsg, len);
}

MainMCU::~MainMCU() {}

void MainMCU::mainLoop() {
    static uint32_t lastMsgOutTick = HAL_GetTick();
    static uint16_t lastPayload = 0;

    uint32_t currentTick = HAL_GetTick();
    if((currentTick - lastMsgOutTick) > 1000) {
        LoRaMessage msg;
        msg.len = snprintf((char*) msg.data, sizeof(msg.data), ">%d<", lastPayload);
        
        if(lora.sendMessage(&msg) == LoRaStatus::LORA_SUCCESS) {
            int len = snprintf(serialMsg, sizeof(serialMsg), "Sent >%d<\n", lastPayload);
            CDC_Transmit_FS((uint8_t*) serialMsg, len);
            HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
        }
        
        lastPayload++;
        lastMsgOutTick = currentTick;
    }

    LoRaMessage msg;
    if(lora.getNextMessage(&msg) == LoRaStatus::LORA_SUCCESS) {
        int len = snprintf(serialMsg, sizeof(serialMsg), "Received >%.*s<\n", msg.len, msg.data);
        CDC_Transmit_FS((uint8_t*) serialMsg, len);
        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
    }

    uart.update();
}