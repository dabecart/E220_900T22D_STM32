#ifndef LORA_h
#define LORA_h

#include <stdint.h>
#include <string.h>
#include "stm32g4xx_hal.h"
#include "main.h"
#include "UART.h"

#define LORA_BROADCAST_ADDRESS 255
#define LORA_HEADER_LEN 3   // command, startAddress, length
#define LORA_MAX_MSG_LEN 200

typedef enum LoRaStatus {
  LORA_SUCCESS = 1,
  LORA_ERR_UNKNOWN,
  LORA_ERR_INVALID_PARAM,
  LORA_ERR_TIMEOUT,
  LORA_ERR_HARDWARE,
  LORA_ERR_HEAD_NOT_RECOGNIZED,
  LORA_ERR_WRONG_FORMAT,
  LORA_ERR_INVALID_RESPONSE,
  LORA_ERR_NO_NEW_MSG
} LoRaStatus;

typedef enum LoRaUARTParity {
  MODE_00_8N1 = 0b00,
  MODE_01_8O1 = 0b01,
  MODE_10_8E1 = 0b10,
  MODE_11_8N1 = 0b11
} LoRaUARTParity;

typedef enum LoRaUARTBaudrate {
  UART_BPS_1200 = 0b000,
  UART_BPS_2400 = 0b001,
  UART_BPS_4800 = 0b010,
  UART_BPS_9600 = 0b011,
  UART_BPS_19200 = 0b100,
  UART_BPS_38400 = 0b101,
  UART_BPS_57600 = 0b110,
  UART_BPS_115200 = 0b111
} LoRaUARTBaudrate;

typedef enum LoRaAirDataRate {
  AIR_DATA_RATE_000_24 = 0b000,
  AIR_DATA_RATE_001_24 = 0b001,
  AIR_DATA_RATE_010_24 = 0b010,
  AIR_DATA_RATE_011_48 = 0b011,
  AIR_DATA_RATE_100_96 = 0b100,
  AIR_DATA_RATE_101_192 = 0b101,
  AIR_DATA_RATE_110_384 = 0b110,
  AIR_DATA_RATE_111_625 = 0b111
} LoRaAirDataRate;

typedef enum LoRaSubPacketSetting {
    SPS_200_00 = 0b00,
    SPS_128_01 = 0b01,
    SPS_064_10 = 0b10,
    SPS_032_11 = 0b11
} LoRaSubPacketSetting;

typedef enum LoRaRSSIAmbientNoiseEnable {
    RSSI_AMBIENT_NOISE_ENABLED = 0b1,
    RSSI_AMBIENT_NOISE_DISABLED = 0b0
} LoRaRSSIAmbientNoiseEnable;

typedef enum LoRaWORPeriod {
    WOR_500_000 = 0b000,
    WOR_1000_001 = 0b001,
    WOR_1500_010 = 0b010,
    WOR_2000_011 = 0b011,
    WOR_2500_100 = 0b100,
    WOR_3000_101 = 0b101,
    WOR_3500_110 = 0b110,
    WOR_4000_111 = 0b111
} LoRaWORPeriod;

typedef enum LoRaLBTEnable {
    LBT_ENABLED = 0b1,
    LBT_DISABLED = 0b0
} LoRaLBTEnable;

typedef enum LoRaRSSIEnable {
    RSSI_ENABLED = 0b1,
    RSSI_DISABLED = 0b0
} LoRaRSSIEnable;

typedef enum LoRaFixedTransmission {
  FT_TRANSPARENT_TRANSMISSION = 0b0,
  FT_FIXED_TRANSMISSION = 0b1
} LoRaFixedTransmission;

typedef enum LoRaTransmissionPower { // in dBm
      POWER_22 = 0b00,
      POWER_17 = 0b01,
      POWER_13 = 0b10,
      POWER_10 = 0b11
} LoRaTransmissionPower;

typedef enum LoRaMode {
    MODE_0_NORMAL           = 0,
    MODE_0_TRANSMISSION     = 0,
    MODE_1_WOR_TRANSMITTER  = 1,
    MODE_1_WOR              = 1,
    MODE_2_WOR_RECEIVER     = 2,
    MODE_2_POWER_SAVING     = 2,
    MODE_3_CONFIGURATION    = 3,
    MODE_3_PROGRAM          = 3,
    MODE_3_SLEEP            = 3,
} LoRaMode;

typedef enum LoRaCommand {
    WRITE_CFG_PWR_DWN_SAVE      = 0xC0, // Saves configuration to NON-volatile memory.
    READ_CONFIGURATION          = 0xC1,
    WRITE_CFG_PWR_DWN_LOSE      = 0xC2, // Saves configuration to volatile memory.
    WRONG_FORMAT                = 0xFF,
    RETURNED_COMMAND            = 0xC1,
    SPECIAL_WIFI_CONF_COMMAND   = 0xCF
} LoRaCommand;

typedef enum LoRaRegAdds {
    REG_ADDS_ADDH        = 0x00,
    REG_ADDS_ADDL        = 0x01,
    REG_ADDS_SPED        = 0x02,
    REG_ADDS_TRANS_MODE  = 0x03,
    REG_ADDS_CHANNEL     = 0x04,
    REG_ADDS_OPTION      = 0x05,
    REG_ADDS_CRYPT       = 0x06,
    REG_ADDS_PID         = 0x80
} LoRaRegAdds;

// Enum with the payloads of the module messages.
typedef enum LoRaPacketLength {
    // Read the configuration registers (0x00 to 0x07).
    PL_CONFIGURATION     = 0x08,
    PL_SPED              = 0x01,
    PL_OPTION            = 0x01,
    PL_TRANSMISSION_MODE = 0x01,
    PL_CHANNEL           = 0x01,
    PL_CRYPT             = 0x02,
    PL_PID               = 0x07
} LoRaPacketLength;

#pragma pack(push, 1)
typedef struct LoRaSpeed {
    LoRaAirDataRate   airDataRate     :3; 
    LoRaUARTParity    uartParity      :2; 
    LoRaUARTBaudrate  uartBaudRate    :3; 
} LoRaSpeed;

typedef struct LoRaTransmissionMode {
    LoRaWORPeriod           WORPeriod           :3; 
    uint8_t                 reserved2           :1; 
    LoRaLBTEnable           enableLBT           :1; 
    uint8_t                 reserved            :1; 
    LoRaFixedTransmission   fixedTransmission   :1; 
    LoRaRSSIEnable          enableRSSI          :1;
} LoRaTransmissionMode;

typedef struct LoRaOptions {
    LoRaTransmissionPower       transmissionPower   :2;
    uint8_t                     reserved            :3; 
    LoRaRSSIAmbientNoiseEnable  RSSIAmbientNoise    :1; 
    LoRaSubPacketSetting        subPacketSetting    :2; 
} LoRaOptions;

// In MODE_3_PROGRAM, the inner registers of the module can be read. This struct contains the 
// response of the module to a read request from register 0x00 to 0x07. 
// Header + register 0x00 to 0x07. 
typedef struct LoRaConfiguration {
    LoRaCommand command;
    uint8_t startAddress;
    uint8_t length;

    uint8_t addressH;
    uint8_t addressL;
    
    LoRaSpeed speed;
    LoRaOptions options;
    
    // For 868MHz LoRa E220 module, the base frequency for channel 0 is 850 MHz.
    // Get the frequency by adding 850 + channel number (MHz).
    uint8_t channel;

    LoRaTransmissionMode transmissionMode;

    uint8_t cryptH;
    uint8_t cryptL;
} LoRaConfiguration;

typedef struct LoRaPID {
    LoRaCommand command;
    uint8_t startAddress;
    uint8_t length;

    uint8_t pid[PL_PID];
} LoRaPID;

typedef struct LoRaModuleInformation {
    uint8_t command;
    uint8_t startAddress;
    uint8_t length;
    uint8_t model;
    uint8_t version;
    uint8_t features;
} LoRaModuleInformation;

typedef struct LoRaMessage {
    uint16_t len;
    int16_t rssi;
    uint8_t data[LORA_MAX_MSG_LEN];
} LoRaMessage;
#pragma pack(pop)

typedef struct LoRa {
    UART* uart;
    LoRaMode currentMode;
    LoRaConfiguration currentConfig;
} LoRa;

void initLoRa(LoRa* lora, UART* uart);

LoRaStatus getNextMessage(LoRa* lora, LoRaMessage* msg);
LoRaStatus sendMessage(LoRa* lora, LoRaMessage* msg);

LoRaStatus readConfigurationRegisters(LoRa* lora);

/**
 * @brief Sets the configuration of the module.
 * 
 * @param config. Configuration fields to write.
 * @param temporarySave. Set to 1 if the configuration is to be temporarily saved. Upon reset of
 * the module, this configuration will be lost. Set to 0 to save it into non-volatile memory.
 * @return LoRaStatus. SUCCESS if the configuration was well set on the device.
 */
LoRaStatus writeConfigurationRegisters(LoRa* lora, LoRaConfiguration config, uint8_t temporarySave);

LoRaStatus getModuleInformation(LoRa* lora, LoRaPID* pid);

LoRaStatus setMode(LoRa* lora, LoRaMode mode);
LoRaMode getMode(LoRa* lora);

LoRaStatus receiveData(LoRa* lora, uint16_t dataLen, uint8_t* dataBuffer);
LoRaStatus writeData(LoRa* lora, uint8_t* dataBuffer, uint16_t dataLen);
LoRaStatus waitAUXPin(LoRa* lora, uint32_t timeout_ms);
uint8_t writeProgramCommand(LoRa* lora, 
    LoRaCommand cmd, LoRaRegAdds addrs, LoRaPacketLength packetLength);

#endif // LORA_h