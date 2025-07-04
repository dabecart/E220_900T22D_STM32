#include "MainMCU.h"

MainMCU* mcu;
char serialMsg[200];

MainMCU::MainMCU(UART_HandleTypeDef* huart1,
                 DMA_HandleTypeDef*  hdma_usart1_rx,
                 DMA_HandleTypeDef*  hdma_usart1_tx,
                 SPI_HandleTypeDef*  hspi1):
                uart(huart1, hdma_usart1_rx, hdma_usart1_tx),
                lora(&uart),
                gui(hspi1) {
    mcu = this;
    lora.init();
    
    gui.init();

    HAL_Delay(4000);
    int len = snprintf(serialMsg, sizeof(serialMsg), "Channel %d\n", lora.currentConfig.channel);
    CDC_Transmit_FS((uint8_t*) serialMsg, len);
}

MainMCU::~MainMCU() {}

void MainMCU::mainLoop() {
    // static uint32_t lastMsgOutTick = HAL_GetTick();
    // static uint16_t lastPayload = 0;

    // uint32_t currentTick = HAL_GetTick();
    // if((currentTick - lastMsgOutTick) > 1000) {
    //     LoRaMessage msg;
    //     msg.len = snprintf((char*) msg.data, sizeof(msg.data), ">%d<", lastPayload);
        
    //     if(lora.sendMessage(&msg) == LoRaStatus::LORA_SUCCESS) {
    //         int len = snprintf(serialMsg, sizeof(serialMsg), "Sent >%d<\n", lastPayload);
    //         CDC_Transmit_FS((uint8_t*) serialMsg, len);
    //         HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
    //         HAL_Delay(50);
    //         HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
    //     }
        
    //     lastPayload++;
    //     lastMsgOutTick = currentTick;
    // }

    LoRaMessage msg;
    if(lora.getNextMessage(&msg) == LoRaStatus::LORA_SUCCESS) {
        int len = snprintf(serialMsg, sizeof(serialMsg), "Received >%.*s<\n", msg.len, msg.data);
        CDC_Transmit_FS((uint8_t*) serialMsg, len);

        gui.tft.writeString(0, 18*0,  "Input:", Font_11x18, BLACK, RED);
        gui.tft.fillRectangle(0, 18*1, gui.tft.width, 18, BLACK);
        gui.tft.writeString(0, 18*1,  (char*) msg.data, Font_11x18, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), "%02d", msg.rssi);
        gui.tft.writeString(0, 18*2,  "RSSI:", Font_11x18, BLACK, GREEN);
        gui.tft.fillRectangle(0, 18*3, gui.tft.width, 18, BLACK);
        gui.tft.writeString(0, 18*3,  serialMsg, Font_11x18, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), "Channel %d", lora.currentConfig.channel);
        gui.tft.writeString(0, 18*4,  serialMsg, Font_11x18, BLACK, BLUE);

        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
    }

    uart.update();
}
