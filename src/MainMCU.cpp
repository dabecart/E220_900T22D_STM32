#include "MainMCU.h"

MainMCU* mcu;
char serialMsg[200];

MainMCU::MainMCU(UART_HandleTypeDef* huart1,
                 DMA_HandleTypeDef*  hdma_usart1_rx,
                 DMA_HandleTypeDef*  hdma_usart1_tx,
                 UART_HandleTypeDef* huart2,
                 DMA_HandleTypeDef*  hdma_usart2_rx,
                 DMA_HandleTypeDef*  hdma_usart2_tx,
                 SPI_HandleTypeDef*  hspi1) :
                uartLoRa(huart1, hdma_usart1_rx, hdma_usart1_tx),
                uartNMEA(huart2, hdma_usart2_rx, hdma_usart2_tx),
                comms(&uartLoRa),
                nmea(&uartNMEA),
                gui(hspi1) {
    mcu = this;
    comms.init();
    gui.init();

    gui.tft.writeString(0, 10*0,  "Input:", Font_7x10, BLACK, RED);
    gui.tft.writeString(0, 10*2,  "RSSI:", Font_7x10, BLACK, GREEN);
    gui.tft.writeString(0, 10*4,  "Latitude:", Font_7x10, BLACK, CYAN);
    gui.tft.writeString(0, 10*6,  "Longitude:", Font_7x10, BLACK, YELLOW);
    gui.tft.writeString(0, 10*8,  "Time:", Font_7x10, BLACK, MAGENTA);
}

MainMCU::~MainMCU() {}

void MainMCU::mainLoop() {
//     static uint32_t lastMsgOutTick = HAL_GetTick();
//     static uint16_t lastPayload = 0;
//
//     uint32_t currentTick = HAL_GetTick();
//     if((currentTick - lastMsgOutTick) > 1000) {
//         uint16_t len = snprintf(serialMsg, sizeof(serialMsg), ">%d<", lastPayload);
//
//         if(comms.sendMessage(COMMS_MULTICAST_ID, (uint8_t*) serialMsg, len)) {
//             // int len = snprintf(serialMsg, sizeof(serialMsg), "Sent >%d<\n", lastPayload);
//             // CDC_Transmit_FS((uint8_t*) serialMsg, len);
//             HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
//             HAL_Delay(50);
//             HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
//         }
//
//         lastPayload++;
//         lastMsgOutTick = currentTick;
//     }

    CommsMsg msg;
    if(comms.getNextMessage(&msg)) {
        // int len = snprintf(serialMsg, sizeof(serialMsg), 
        //                    "Received >%.*s<\n", msg.header.length, msg.payload);
        // CDC_Transmit_FS((uint8_t*) serialMsg, len);

        gui.tft.fillRectangle(0, 10*1, gui.tft.width, 10, BLACK);
        snprintf(serialMsg, sizeof(serialMsg), 
                 "%.*s", msg.header.length - COMMS_HEADER_LEN - COMMS_CRC_LEN, msg.payload);
        gui.tft.writeString(0, 10*1, serialMsg, Font_7x10, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), "%02d", msg.rssi);
        gui.tft.fillRectangle(0, 10*3, gui.tft.width, 10, BLACK);
        gui.tft.writeString(0, 10*3,  serialMsg, Font_7x10, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), "%.6f", nmea.latitude);
        gui.tft.fillRectangle(0, 10*5, gui.tft.width, 10, BLACK);
        gui.tft.writeString(0, 10*5,  serialMsg, Font_7x10, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), "%.6f", nmea.longitude);
        gui.tft.fillRectangle(0, 10*7, gui.tft.width, 10, BLACK);
        gui.tft.writeString(0, 10*7,  serialMsg, Font_7x10, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), "%s", nmea.time);
        gui.tft.fillRectangle(0, 10*9, gui.tft.width, 10, BLACK);
        gui.tft.writeString(0, 10*9,  serialMsg, Font_7x10, WHITE, BLACK);

        snprintf(serialMsg, sizeof(serialMsg), 
                 "Channel %d - Fix %d", comms.getLoRaConfiguration().channel, nmea.fix_quality);
        gui.tft.writeString(0, gui.tft.height - 11,  serialMsg, Font_7x10, BLACK, BLUE);

        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
    }

    nmea.update();

    uartLoRa.update();
    uartNMEA.update();
}
