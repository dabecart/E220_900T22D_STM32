#include "MainMCU.h"

MainMCU* mcu;

MainMCU::MainMCU(
    UART_HandleTypeDef* huart1,
    DMA_HandleTypeDef*  hdma_usart1_rx,
    DMA_HandleTypeDef*  hdma_usart1_tx,
    UART_HandleTypeDef* huart2,
    DMA_HandleTypeDef*  hdma_usart2_rx,
    DMA_HandleTypeDef*  hdma_usart2_tx,
    SPI_HandleTypeDef*  hspi1
) :
    uartLoRa(huart1, hdma_usart1_rx, hdma_usart1_tx),
    uartNMEA(huart2, hdma_usart2_rx, hdma_usart2_tx),
    comms(&uartLoRa),
    nmea(&uartNMEA),
    gui(hspi1) 
{
    mcu = this;

    comms.init();
    gui.init();
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

        gui.updateLoRa((char*) msg.payload, msg.header.getPayloadLength(), 
                       msg.rssi, comms.getLoRaConfiguration().channel, comms.crcErrorCount);

        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
        HAL_Delay(30);
        HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
    }

    if(nmea.update()) {
        gui.updateNMEA(nmea.latitude, nmea.longitude, nmea.altitude, nmea.time, nmea.fixQuality, nmea.satellitesUsed);
    }

    uartLoRa.update();
    uartNMEA.update();
}
