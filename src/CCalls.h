#ifndef C_CALLS_h
#define C_CALLS_h

// C Calls is a header file used to connect the C code with the C++ code.

// This header is used as an static instance of an SD card. This way all funcions related to FATFS
// (which are written in C) do not have to be converted to C++. This is so that the autogeneration
// of code does not mess with the files.

#include "stm32g4xx_hal.h"
#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

void updateSDTimers();
DSTATUS SD_disk_initialize(uint8_t pdrv);
DSTATUS SD_disk_status(uint8_t pdrv);
DRESULT SD_disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint16_t count);
DRESULT SD_disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint16_t count);
DRESULT SD_disk_ioctl(uint8_t pdrv, uint8_t cmd, void* buff);

void createMainMCU(UART_HandleTypeDef* huart1,
                 DMA_HandleTypeDef*  hdma_usart1_rx,
                 DMA_HandleTypeDef*  hdma_usart1_tx,
                 UART_HandleTypeDef* huart2,
                 DMA_HandleTypeDef*  hdma_usart2_rx,
                 DMA_HandleTypeDef*  hdma_usart2_tx,
                 SPI_HandleTypeDef*  hspi1,
                 SPI_HandleTypeDef*  hspi2);
void initMainMCU();
void loopMainMCU();

void toggleLoggingIRQ();

#ifdef __cplusplus
}
#endif

#endif // C_CALLS_h