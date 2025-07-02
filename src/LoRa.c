#include "LoRa.h"

void initLoRa(LoRa* lora, UART* uart) {
    if(lora == NULL || uart == NULL) return;

    readConfigurationRegisters(lora);
    setMode(lora, MODE_0_NORMAL);
}

LoRaStatus getNextMessage(LoRa* lora, LoRaMessage* msg) {
    if(lora == NULL || msg == NULL) return LORA_ERR_INVALID_PARAM;

    msg->len = lora->uart->RXBuffer.len;
    if(msg->len == 0) return LORA_ERR_NO_NEW_MSG;

    if(msg->len > sizeof(msg->data)) msg->len = sizeof(msg->data);
    
    popN_cb(&lora->uart->RXBuffer, msg->len, msg->data);
    
    if(lora->currentConfig.transmissionMode.enableRSSI) {
        // RSSI is calculated as -(255 - field)
        msg->rssi = ((int16_t) msg->data[msg->len - 1]) - 255;
        msg->len--;    
    }else {
        msg->rssi = 0;
    }
    
    return LORA_SUCCESS;
}

LoRaStatus sendMessage(LoRa* lora, LoRaMessage* msg) {
    if(msg == NULL) return LORA_ERR_INVALID_PARAM;

    return writeData(lora, msg->data, msg->len);
}

LoRaStatus readConfigurationRegisters(LoRa* lora) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    LoRaMode previousMode = lora->currentMode;

    // Switch to Program mode.
    LoRaStatus st = setMode(lora, MODE_3_PROGRAM);
    if(st != LORA_SUCCESS) return st;

    // Write the command to read the configuration register (CFG).
    uint8_t writeStatus = writeProgramCommand(lora, READ_CONFIGURATION, 
                                              REG_ADDS_ADDH, PL_CONFIGURATION);
    if(!writeStatus) {
        setMode(lora, previousMode);
        return LORA_ERR_HARDWARE;        
    }

    // Receive response.
    LoRaStatus receiveStatus = receiveData(lora, 
                                           sizeof(LoRaConfiguration), 
                                           (uint8_t*) &lora->currentConfig);
    if(receiveStatus != LORA_SUCCESS) {
        setMode(lora, previousMode);
        return LORA_ERR_HARDWARE;
    }

    // Check the command field of the response.
    if(lora->currentConfig.command == WRONG_FORMAT) {
        setMode(lora, previousMode);
        return LORA_ERR_WRONG_FORMAT;
    }else if(lora->currentConfig.command != READ_CONFIGURATION ||
             lora->currentConfig.startAddress != REG_ADDS_ADDH ||
             lora->currentConfig.length != PL_CONFIGURATION) {
        return LORA_ERR_HEAD_NOT_RECOGNIZED;
    }

    // Switch to the previous mode.
    setMode(lora, previousMode);

    return LORA_SUCCESS;
}

LoRaStatus writeConfigurationRegisters(LoRa* lora, LoRaConfiguration config, uint8_t temporarySave) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    LoRaMode previousMode = lora->currentMode;

    // Switch to Program mode.
    LoRaStatus st = setMode(lora, MODE_3_PROGRAM);
    if(st != LORA_SUCCESS) return st;

    // Write the configuration registers (CFG).
    config.command = temporarySave ? WRITE_CFG_PWR_DWN_LOSE : 
                                     WRITE_CFG_PWR_DWN_SAVE;
    config.startAddress = REG_ADDS_ADDH;
    config.length = PL_CONFIGURATION;

    LoRaStatus writeStatus = writeData(lora, (uint8_t*) &config, sizeof(LoRaConfiguration));
    if(!writeStatus) {
        setMode(lora, previousMode);
        return LORA_ERR_HARDWARE;        
    }

    // The module should respond with the same payload as the configuration register.
    LoRaConfiguration readConfig;
    LoRaStatus receiveStatus = receiveData(lora, sizeof(readConfig), (uint8_t*) &readConfig);
    if(receiveStatus != LORA_SUCCESS) {
        setMode(lora, previousMode);
        return LORA_ERR_HARDWARE;        
    }

    // Check the command field of the response.
    if(readConfig.command == WRONG_FORMAT) {
        setMode(lora, previousMode);
        return LORA_ERR_WRONG_FORMAT;
    }

    // Check that received payload is the same as the sent payload. 
    // Only check from addressH to transmission mode.
    uint8_t* wConf = (uint8_t*) &config;
    uint8_t* rConf = (uint8_t*) &readConfig;
    LoRaStatus checkStatus = LORA_SUCCESS;
    if(
        (wConf[REG_ADDS_ADDH]       != rConf[REG_ADDS_ADDH])       ||
        (wConf[REG_ADDS_ADDL]       != rConf[REG_ADDS_ADDL])       ||
        (wConf[REG_ADDS_SPED]       != rConf[REG_ADDS_SPED])       ||
        (wConf[REG_ADDS_TRANS_MODE] != rConf[REG_ADDS_TRANS_MODE]) ||            
        (wConf[REG_ADDS_CHANNEL]    != rConf[REG_ADDS_CHANNEL])    ||    
        (wConf[REG_ADDS_OPTION]     != rConf[REG_ADDS_OPTION])    
    ){
        checkStatus = LORA_ERR_INVALID_RESPONSE;
    }

    // Switch to the previous mode.
    setMode(lora, previousMode);

    return checkStatus;
}

LoRaStatus getModuleInformation(LoRa* lora, LoRaPID* pid) {
    if(lora == NULL || pid == NULL) return LORA_ERR_INVALID_PARAM;

    LoRaMode previousMode = lora->currentMode;

    // Switch to Program mode.
    LoRaStatus st = setMode(lora, MODE_3_PROGRAM);
    if(st != LORA_SUCCESS) return st;

    // Write the command to read the configuration register (CFG).
    uint8_t writeStatus = writeProgramCommand(lora, READ_CONFIGURATION, 
                                              REG_ADDS_PID, PL_PID);
    if(!writeStatus) {
        setMode(lora, previousMode);
        return LORA_ERR_HARDWARE;        
    }

    // Receive response.
    LoRaStatus receiveStatus = receiveData(lora, sizeof(LoRaPID), (uint8_t*) &pid);
    if(receiveStatus != LORA_SUCCESS) {
        setMode(lora, previousMode);
        return LORA_ERR_HARDWARE;
    }

    // Check the command field of the response.
    if(pid->command == WRONG_FORMAT) {
        setMode(lora, previousMode);
        return LORA_ERR_WRONG_FORMAT;
    }

    // Switch to the previous mode.
    setMode(lora, previousMode);

    return LORA_SUCCESS;
}

LoRaStatus receiveData(LoRa* lora, uint16_t dataLen, uint8_t* dataBuffer) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    // Read data from the UART.
    HAL_StatusTypeDef status = HAL_UART_Receive(lora->uart->hUART, dataBuffer, dataLen, 1000);
    // While the LoRa module is transmitting the AUX pin remains LOW.
    LoRaStatus auxStatus = waitAUXPin(lora, 1000);
    if(status == HAL_TIMEOUT) return LORA_ERR_TIMEOUT;
    else if(status == HAL_ERROR) return LORA_ERR_UNKNOWN;
    return auxStatus; 
}

LoRaStatus writeData(LoRa* lora, uint8_t* dataBuffer, uint16_t dataLen) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    // Write data to the UART.
    HAL_StatusTypeDef status = HAL_UART_Transmit(lora->uart->hUART, dataBuffer, dataLen, 1000);
    // While the LoRa module is transmitting the AUX pin remains LOW.
    LoRaStatus auxStatus = waitAUXPin(lora, 1000);
    if(status == HAL_TIMEOUT) return LORA_ERR_TIMEOUT;
    else if(status == HAL_ERROR) return LORA_ERR_UNKNOWN;
    return auxStatus; 
}

LoRaStatus waitAUXPin(LoRa* lora, uint32_t timeout_ms) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    // The AUX pin remains LOW while the module is busy.
    uint32_t t0 = HAL_GetTick();
    while(HAL_GPIO_ReadPin(LORA_AUX_GPIO_Port, LORA_AUX_Pin) == GPIO_PIN_RESET) {
        if(((uint32_t)(HAL_GetTick() - t0)) > timeout_ms) {
            return LORA_ERR_TIMEOUT;
        }
    }

    HAL_Delay(20);
    return LORA_SUCCESS;
}

uint8_t writeProgramCommand(LoRa* lora, 
                            LoRaCommand cmd, LoRaRegAdds addrs, LoRaPacketLength packetLength) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    uint8_t outMsg[3] = {cmd, addrs, packetLength};
    HAL_StatusTypeDef status = HAL_UART_Transmit(lora->uart->hUART, outMsg, sizeof(outMsg), 1000);
    HAL_Delay(50);
    return status == HAL_OK;
}

LoRaStatus setMode(LoRa* lora, LoRaMode mode) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    HAL_Delay(40);

    switch (mode) {
        case MODE_0_NORMAL:
            HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET);
            break;
        case MODE_1_WOR_TRANSMITTER:
            HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET);
            break;
        case MODE_2_WOR_RECEIVER:
            HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET);
            break;
        case MODE_3_CONFIGURATION:
            HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET);
            break;

        default:
            return LORA_ERR_INVALID_PARAM;
    }

    HAL_Delay(40);

    LoRaStatus st = waitAUXPin(lora, 1000);
    if(st == LORA_SUCCESS) {
        lora->currentMode = mode;
    } 
    
    return st;
}

LoRaMode getMode(LoRa* lora) {
    if(lora == NULL) return LORA_ERR_INVALID_PARAM;

    return lora->currentMode;
}