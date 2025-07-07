#include "CCalls.h"
#include "MainMCU.h"

MainMCU* mainMCU;
volatile uint32_t lastIRQ = 0;

void updateSDTimers() {
  if(mainMCU->sd.Timer1 > 0) mainMCU->sd.Timer1--;
  if(mainMCU->sd.Timer2 > 0) mainMCU->sd.Timer2--;
}

DSTATUS SD_disk_initialize(uint8_t pdrv) {
    return mainMCU->sd.SD_disk_initialize(pdrv);
}

DSTATUS SD_disk_status(uint8_t pdrv) {
    return mainMCU->sd.SD_disk_status(pdrv);
}

DRESULT SD_disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint16_t count) {
    return mainMCU->sd.SD_disk_read(pdrv, buff, sector, count);
}

DRESULT SD_disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint16_t count) {
    return mainMCU->sd.SD_disk_write(pdrv, buff, sector, count);
}

DRESULT SD_disk_ioctl(uint8_t pdrv, uint8_t cmd, void* buff) {
    return mainMCU->sd.SD_disk_ioctl(pdrv, cmd, buff);
}

void createMainMCU(
    UART_HandleTypeDef* huart1,
    DMA_HandleTypeDef*  hdma_usart1_rx,
    DMA_HandleTypeDef*  hdma_usart1_tx,
    UART_HandleTypeDef* huart2,
    DMA_HandleTypeDef*  hdma_usart2_rx,
    DMA_HandleTypeDef*  hdma_usart2_tx,
    SPI_HandleTypeDef*  hspi1,
    SPI_HandleTypeDef*  hspi2
) {
    mainMCU = new MainMCU(huart1, hdma_usart1_rx, hdma_usart1_tx, 
                      huart2, hdma_usart2_rx, hdma_usart2_tx, 
                      hspi1, hspi2);
}

void initMainMCU() {
    mainMCU->init();
}

void loopMainMCU() {
    mainMCU->mainLoop();
}

void toggleLoggingIRQ() {
    uint32_t t = HAL_GetTick();
    if((t-lastIRQ) < 1000) {
        // Too little time has passed between the button presses.
        return;
    }

    if(mainMCU->log.logging) {
        // Stop logging.
        mainMCU->log.stopLogging();
    }else {
        // Start logging only if there's valid GPS data.
        if(mainMCU->nmea.fixQuality != 0) {
            // date_time.txt
            snprintf(mainMCU->buf, sizeof(mainMCU->buf), 
                     "%.*s_%.*s.txt", 6, mainMCU->nmea.date, 6, mainMCU->nmea.time);
            mainMCU->log.startLogging(mainMCU->buf);
        }
    }

    lastIRQ = t;
}
