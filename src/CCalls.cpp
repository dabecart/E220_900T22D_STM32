#include "CCalls.h"
#include "FATFS_SD.h"
#include "MainMCU.h"

SD* sd;
MainMCU* mainMCU;

void initSD(SPI_HandleTypeDef* hspi) {
    sd = new SD(hspi);
}

void updateSDTimers() {
  if(sd->Timer1 > 0) sd->Timer1--;
  if(sd->Timer2 > 0) sd->Timer2--;
}

DSTATUS SD_disk_initialize(uint8_t pdrv) {
    return sd->SD_disk_initialize(pdrv);
}

DSTATUS SD_disk_status(uint8_t pdrv) {
    return sd->SD_disk_status(pdrv);
}

DRESULT SD_disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint16_t count) {
    return sd->SD_disk_read(pdrv, buff, sector, count);
}

DRESULT SD_disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint16_t count) {
    return sd->SD_disk_write(pdrv, buff, sector, count);
}

DRESULT SD_disk_ioctl(uint8_t pdrv, uint8_t cmd, void* buff) {
    return sd->SD_disk_ioctl(pdrv, cmd, buff);
}

void initMainMCU(
    UART_HandleTypeDef* huart1,
    DMA_HandleTypeDef*  hdma_usart1_rx,
    DMA_HandleTypeDef*  hdma_usart1_tx,
    UART_HandleTypeDef* huart2,
    DMA_HandleTypeDef*  hdma_usart2_rx,
    DMA_HandleTypeDef*  hdma_usart2_tx,
    SPI_HandleTypeDef*  hspi1
) {
    mainMCU = new MainMCU(huart1, hdma_usart1_rx, hdma_usart1_tx, 
                      huart2, hdma_usart2_rx, hdma_usart2_tx, 
                      hspi1);
}

void loopMainMCU() {
    mainMCU->mainLoop();
}