#ifndef FATFS_SD_h
#define FATFS_SD_h

#include "diskio.h"
#include "main.h"

#define SPI_TIMEOUT 100

//************************************** MMC/SDC Commands ******************************************
#define CMD0     (0x40+0)     	// GO_IDLE_STATE
#define CMD1     (0x40+1)     	// SEND_OP_COND
#define CMD8     (0x40+8)     	// SEND_IF_COND
#define CMD9     (0x40+9)     	// SEND_CSD
#define CMD10    (0x40+10)    	// SEND_CID
#define CMD12    (0x40+12)    	// STOP_TRANSMISSION
#define CMD16    (0x40+16)    	// SET_BLOCKLEN
#define CMD17    (0x40+17)    	// READ_SINGLE_BLOCK
#define CMD18    (0x40+18)    	// READ_MULTIPLE_BLOCK
#define CMD23    (0x40+23)    	// SET_BLOCK_COUNT
#define CMD24    (0x40+24)    	// WRITE_BLOCK
#define CMD25    (0x40+25)    	// WRITE_MULTIPLE_BLOCK
#define CMD41    (0x40+41)    	// SEND_OP_COND (ACMD)
#define CMD55    (0x40+55)    	// APP_CMD
#define CMD58    (0x40+58)    	// READ_OCR

//****************************** MMC Card Types (MMC_GET_TYPE) *************************************

#define CT_MMC		0x01	// MMC ver 3
#define CT_SD1		0x02	// SD ver 1
#define CT_SD2		0x04	// SD ver 2
#define CT_SDC		0x06	// SD
#define CT_BLOCK	0x08	// Block addressing

class SD {
    public:
    SD(SPI_HandleTypeDef* hspi);
    ~SD();

    // user_diskio.c Functions

    DSTATUS SD_disk_initialize(uint8_t pdrv);
    DSTATUS SD_disk_status(uint8_t pdrv);
    DRESULT SD_disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint16_t count);
    DRESULT SD_disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint16_t count);
    DRESULT SD_disk_ioctl(uint8_t pdrv, uint8_t cmd, void* buff);

    private:
    // SPI FUNCTIONS

    // Slave select.
    void SELECT();
    // Slave deselect
    void DESELECT();
    // SPI transmit a byte
    void SPI_TxByte(uint8_t data);
    // SPI transmit buffer
    void SPI_TxBuffer(uint8_t *buffer, uint16_t len);
    // SPI receive a byte
    uint8_t SPI_RxByte();
    // SPI receive a byte via pointer
    void SPI_RxBytePtr(uint8_t *buff);

    // SD CARD FUNCTIONS.

    // Wait SD ready
    uint8_t SD_ReadyWait();
    // power on
    void SD_PowerOn();
    // Power off
    void SD_PowerOff();
    // Check power flag
    uint8_t SD_CheckPower();
    // Receive data block
    uint8_t SD_RxDataBlock(uint8_t *buff, uint16_t len);
#if _USE_WRITE == 1
    // Transmit data block
    uint8_t SD_TxDataBlock(const uint8_t *buff, uint8_t token);
#endif
    // Transmit command
    uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg);

    SPI_HandleTypeDef* 	HSPI_SDCARD;
};

#endif // FATFS_SD_h
