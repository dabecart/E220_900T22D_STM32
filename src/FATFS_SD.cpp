#include "main.h"
#include "diskio.h"
#include "FATFS_SD.h"

SD::SD(SPI_HandleTypeDef* hspi) {
  this->HSPI_SDCARD = hspi;
}

SD::~SD() {}

/* initialize SD */
DSTATUS SD::SD_disk_initialize(uint8_t drv) {
  uint8_t n, type, ocr[4];
  /* single drive, drv should be 0 */
  if(drv) return STA_NOINIT;
  /* no disk */
  if(Stat & STA_NODISK) return Stat;
  /* power on */
  SD_PowerOn();
  /* slave select */
  SELECT();
  /* check disk type */
  type = 0;
  /* send GO_IDLE_STATE command */
  if (SD_SendCmd(CMD0, 0) == 1) {
    /* timeout 1 sec */
    Timer1 = 1000;
    /* SDC V2+ accept CMD8 command, http://elm-chan.org/docs/mmc/mmc_e.html */
    if (SD_SendCmd(CMD8, 0x1AA) == 1) {
      /* operation condition register */
      for (n = 0; n < 4; n++) {
        ocr[n] = SPI_RxByte();
      }
      /* voltage range 2.7-3.6V */
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
        /* ACMD41 with HCS bit */
        do {
          if (SD_SendCmd(CMD55, 0) <= 1 && SD_SendCmd(CMD41, 1UL << 30) == 0) break;
        } while (Timer1);

        /* READ_OCR */
        if (Timer1 && SD_SendCmd(CMD58, 0) == 0) {
          /* Check CCS bit */
          for (n = 0; n < 4; n++) {
            ocr[n] = SPI_RxByte();
          }

          /* SDv2 (HC or SC) */
          type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
        }
      }
    }
    else {
      /* SDC V1 or MMC */
      type = (SD_SendCmd(CMD55, 0) <= 1 && SD_SendCmd(CMD41, 0) <= 1) ? CT_SD1 : CT_MMC;
      do {
        if (type == CT_SD1) {
          if (SD_SendCmd(CMD55, 0) <= 1 && SD_SendCmd(CMD41, 0) == 0) break; /* ACMD41 */
        }
        else {
          if (SD_SendCmd(CMD1, 0) == 0) break; /* CMD1 */
        }
      } while (Timer1);
      /* SET_BLOCKLEN */
      if (!Timer1 || SD_SendCmd(CMD16, 512) != 0) type = 0;
    }
  }
  CardType = type;
  /* Idle */
  DESELECT();
  SPI_RxByte();
  /* Clear STA_NOINIT */
  if (type) {
    Stat &= ~STA_NOINIT;
  }
  else {
    /* Initialization failed */
    SD_PowerOff();
  }
  return Stat;
}

/* return disk status */
DSTATUS SD::SD_disk_status(uint8_t drv) {
  if (drv) return STA_NOINIT;
  return Stat;
}

/* read sector */
DRESULT SD::SD_disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint16_t count) {
  /* pdrv should be 0 */
  if (pdrv || !count) return RES_PARERR;

  /* no disk */
  if (Stat & STA_NOINIT) return RES_NOTRDY;

  /* convert to byte address */
  if (!(CardType & CT_SD2)) sector *= 512;

  SELECT();

  if (count == 1) {
    /* READ_SINGLE_BLOCK */
    if ((SD_SendCmd(CMD17, sector) == 0) && SD_RxDataBlock(buff, 512)) count = 0;
  }
  else {
    /* READ_MULTIPLE_BLOCK */
    if (SD_SendCmd(CMD18, sector) == 0) {
      do {
        if (!SD_RxDataBlock(buff, 512)) break;
        buff += 512;
      } while (--count);

      /* STOP_TRANSMISSION */
      SD_SendCmd(CMD12, 0);
    }
  }

  /* Idle */
  DESELECT();
  SPI_RxByte();

  return count ? RES_ERROR : RES_OK;
}

/* write sector */
#if _USE_WRITE == 1
DRESULT SD::SD_disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint16_t count) {
  /* pdrv should be 0 */
  if (pdrv || !count) return RES_PARERR;

  /* no disk */
  if (Stat & STA_NOINIT) return RES_NOTRDY;

  /* write protection */
  if (Stat & STA_PROTECT) return RES_WRPRT;

  /* convert to byte address */
  if (!(CardType & CT_SD2)) sector *= 512;

  SELECT();

  if (count == 1) {
    /* WRITE_BLOCK */
    if ((SD_SendCmd(CMD24, sector) == 0) && SD_TxDataBlock(buff, 0xFE))
      count = 0;
  }
  else {
    /* WRITE_MULTIPLE_BLOCK */
    if (CardType & CT_SD1) {
      SD_SendCmd(CMD55, 0);
      SD_SendCmd(CMD23, count); /* ACMD23 */
    }

    if (SD_SendCmd(CMD25, sector) == 0) {
      do {
        if(!SD_TxDataBlock(buff, 0xFC)) break;
        buff += 512;
      } while (--count);

      /* STOP_TRAN token */
      if(!SD_TxDataBlock(0, 0xFD)) {
        count = 1;
      }
    }
  }

  /* Idle */
  DESELECT();
  SPI_RxByte();

  return count ? RES_ERROR : RES_OK;
}
#endif /* _USE_WRITE */

/* ioctl */
DRESULT SD::SD_disk_ioctl(uint8_t drv, uint8_t ctrl, void *buff) {
  DRESULT res;
  uint8_t n, csd[16], *ptr = (uint8_t*) buff;
  WORD csize;

  /* pdrv should be 0 */
  if (drv) return RES_PARERR;
  res = RES_ERROR;

  if (ctrl == CTRL_POWER) {
    switch (*ptr) {
    case 0:
      SD_PowerOff();    /* Power Off */
      res = RES_OK;
      break;
    case 1:
      SD_PowerOn();   /* Power On */
      res = RES_OK;
      break;
    case 2:
      *(ptr + 1) = SD_CheckPower();
      res = RES_OK;   /* Power Check */
      break;
    default:
      res = RES_PARERR;
    }
  }
  else {
    /* no disk */
    if (Stat & STA_NOINIT){
    	return RES_NOTRDY;
    }
    SELECT();
    switch (ctrl) {
    case GET_SECTOR_COUNT:
      /* SEND_CSD */
      if ((SD_SendCmd(CMD9, 0) == 0) && SD_RxDataBlock(csd, 16)) {
        if ((csd[0] >> 6) == 1) {
          /* SDC V2 */
          csize = csd[9] + ((WORD) csd[8] << 8) + 1;
          *(uint32_t*) buff = (uint32_t) csize << 10;
        }
        else {
          /* MMC or SDC V1 */
          n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
          csize = (csd[8] >> 6) + ((WORD) csd[7] << 2) + ((WORD) (csd[6] & 3) << 10) + 1;
          *(uint32_t*) buff = (uint32_t) csize << (n - 9);
        }
        res = RES_OK;
      }
      break;
    case GET_SECTOR_SIZE:
      *(WORD*) buff = 512;
      res = RES_OK;
      break;
    case CTRL_SYNC:
      if (SD_ReadyWait() == 0xFF) res = RES_OK;
      break;
    case MMC_GET_CSD:
      /* SEND_CSD */
      if (SD_SendCmd(CMD9, 0) == 0 && SD_RxDataBlock(ptr, 16)) res = RES_OK;
      break;
    case MMC_GET_CID:
      /* SEND_CID */
      if (SD_SendCmd(CMD10, 0) == 0 && SD_RxDataBlock(ptr, 16)) res = RES_OK;
      break;
    case MMC_GET_OCR:
      /* READ_OCR */
      if (SD_SendCmd(CMD58, 0) == 0) {
        for (n = 0; n < 4; n++) {
          *ptr++ = SPI_RxByte();
        }
        res = RES_OK;
      }
    default:
      res = RES_PARERR;
    }
    DESELECT();
    SPI_RxByte();
  }
  return res;
}


void SD::SELECT() {
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
}

void SD::DESELECT() {
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
}

void SD::SPI_TxByte(uint8_t data) {
  while(!__HAL_SPI_GET_FLAG(HSPI_SDCARD, SPI_FLAG_TXE));
  HAL_SPI_Transmit(HSPI_SDCARD, &data, 1, SPI_TIMEOUT);
}

void SD::SPI_TxBuffer(uint8_t *buffer, uint16_t len) {
  while(!__HAL_SPI_GET_FLAG(HSPI_SDCARD, SPI_FLAG_TXE));
  HAL_SPI_Transmit(HSPI_SDCARD, buffer, len, SPI_TIMEOUT);
}

uint8_t SD::SPI_RxByte() {
  uint8_t dummy, data;
  dummy = 0xFF;
  while(!__HAL_SPI_GET_FLAG(HSPI_SDCARD, SPI_FLAG_TXE));
  HAL_SPI_TransmitReceive(HSPI_SDCARD, &dummy, &data, 1, SPI_TIMEOUT);
  return data;
}

void SD::SPI_RxBytePtr(uint8_t *buff) {
  *buff = SPI_RxByte();
}

/* wait SD ready */
uint8_t SD::SD_ReadyWait() {
  uint8_t res;
  /* timeout 500ms */
  Timer2 = 500;
  /* if SD goes ready, receives 0xFF */
  do {
    res = SPI_RxByte();
  } while ((res != 0xFF) && Timer2);
  return res;
}

/* power on */
void SD::SD_PowerOn() {
  uint8_t args[6];
  uint32_t cnt = 0x1FFF;
  /* transmit bytes to wake up */
  DESELECT();
  for(int i = 0; i < 10; i++) {
    SPI_TxByte(0xFF);
  }
  /* slave select */
  SELECT();
  /* make idle state */
  args[0] = CMD0;   /* CMD0:GO_IDLE_STATE */
  args[1] = 0;
  args[2] = 0;
  args[3] = 0;
  args[4] = 0;
  args[5] = 0x95;
  SPI_TxBuffer(args, sizeof(args));
  /* wait response */
  while ((SPI_RxByte() != 0x01) && cnt) {
    cnt--;
  }
  DESELECT();
  SPI_TxByte(0XFF);
  PowerFlag = 1;
}

/* power off */
void SD::SD_PowerOff() {
  PowerFlag = 0;
}

/* check power flag */
uint8_t SD::SD_CheckPower() {
  return PowerFlag;
}

/* receive data block */
uint8_t SD::SD_RxDataBlock(uint8_t *buff, uint16_t len) {
  uint8_t token;
  /* timeout 200ms */
  Timer1 = 200;
  /* loop until receive a response or timeout */
  do {
    token = SPI_RxByte();
  } while((token == 0xFF) && Timer1);
  /* invalid response */
  if(token != 0xFE) return 0;
  /* receive data */
  do {
    SPI_RxBytePtr(buff++);
  } while(len--);
  /* discard CRC */
  SPI_RxByte();
  SPI_RxByte();
  return 1;
}

/* transmit data block */
#if _USE_WRITE == 1
uint8_t SD::SD_TxDataBlock(const uint8_t *buff, uint8_t token) {
  uint8_t resp = 0;
  uint8_t i = 0;
  /* wait SD ready */
  if (SD_ReadyWait() != 0xFF) return 0;
  /* transmit token */
  SPI_TxByte(token);
  /* if it's not STOP token, transmit data */
  if (token != 0xFD) {
    SPI_TxBuffer((uint8_t*)buff, 512);
    /* discard CRC */
    SPI_RxByte();
    SPI_RxByte();
    /* receive response */
    while (i <= 64) {
      resp = SPI_RxByte();
      /* transmit 0x05 accepted */
      if ((resp & 0x1F) == 0x05) break;
      i++;
    }
    /* recv buffer clear */
    while (SPI_RxByte() == 0);
  }
  /* transmit 0x05 accepted */
  if ((resp & 0x1F) == 0x05) return 1;

  return 0;
}
#endif /* _USE_WRITE */

/* transmit command */
uint8_t SD::SD_SendCmd(uint8_t cmd, uint32_t arg) {
  uint8_t crc, res;
  /* wait SD ready */
  if (SD_ReadyWait() != 0xFF) return 0xFF;
  /* transmit command */
  SPI_TxByte(cmd);          /* Command */
  SPI_TxByte((uint8_t)(arg >> 24));   /* Argument[31..24] */
  SPI_TxByte((uint8_t)(arg >> 16));   /* Argument[23..16] */
  SPI_TxByte((uint8_t)(arg >> 8));  /* Argument[15..8] */
  SPI_TxByte((uint8_t)arg);       /* Argument[7..0] */
  /* prepare CRC */
  if(cmd == CMD0) crc = 0x95; /* CRC for CMD0(0) */
  else if(cmd == CMD8) crc = 0x87;  /* CRC for CMD8(0x1AA) */
  else crc = 1;
  /* transmit CRC */
  SPI_TxByte(crc);
  /* Skip a stuff byte when STOP_TRANSMISSION */
  if (cmd == CMD12) SPI_RxByte();
  /* receive response */
  uint8_t n = 10;
  do {
    res = SPI_RxByte();
  } while ((res & 0x80) && --n);

  return res;
}