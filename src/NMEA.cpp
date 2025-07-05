#include "NMEA.h"

NMEA::NMEA(UART* uart) {
    this->uart = uart;
}

NMEA::~NMEA() {}

uint16_t NMEA::getFormattedTime(char* out, uint16_t maxOutLen) const {
    if (strlen(time) < 6) {
        return snprintf(out, maxOutLen, "00:00:00");
    }

    char hours[3] = { time[0], time[1], '\0' };
    char minutes[3] = { time[2], time[3], '\0' };
    char seconds[3] = { time[4], time[5], '\0' };

    return snprintf(out, maxOutLen, "%s:%s:%s", hours, minutes, seconds);
}

uint16_t NMEA::getFormattedDate(char* out, uint16_t maxOutLen) const {
    if (strlen(date) < 6) {
        return snprintf(out, maxOutLen, "00/00/0000");
    }

    char day[3] = { date[0], date[1], '\0' };
    char month[3] = { date[2], date[3], '\0' };
    char year[3] = { date[4], date[5], '\0' };

    return snprintf(out, maxOutLen, "%s/%s/20%s", day, month, year);
}

uint8_t NMEA::update() {
    uint8_t lineBuffer[256];
    uint16_t dataLen = 1;
    uint8_t isDataUpdated = 0;

    do {
        // Initialize the loop.
        lineBuffer[0] = 0;
        dataLen = 1;

        // Find a line in the RX buffer that starts with '$'.
        while(uart->RXBuffer.peek(lineBuffer) && (lineBuffer[0] != '$')) {
            uart->RXBuffer.pop(NULL);
        }

        if(lineBuffer[0] != '$') {
            // Could not find a header.
            return isDataUpdated;
        }

        // Peek until a '\n' is found.
        while(dataLen < (sizeof(lineBuffer) - 1)) {
            if(uart->RXBuffer.peekAt(dataLen, lineBuffer)) {
                if(lineBuffer[0] == '\n') {
                    // Found it!
                    dataLen++;
                    break;
                }
            }else {
                // Failed to peek, there isn't enough data on the buffer.
                return isDataUpdated;
            }
            dataLen++;
        }

        if(dataLen >= (sizeof(lineBuffer) - 1)) {
            // Message is too long!
            uart->RXBuffer.pop(NULL);
            return isDataUpdated;
        }

        // Found a message!
        uart->RXBuffer.popN(dataLen, lineBuffer);

        if(lineBuffer[0] != '$' || lineBuffer[dataLen-1] != '\n') {
            // Buffer was overwritten.
            return isDataUpdated;
        }
        
        lineBuffer[dataLen] = 0; // Put a NULL at the end of the string.
        isDataUpdated |= parseNMEALine((char*) lineBuffer);
    } while(uart->RXBuffer.len > 0);

    return isDataUpdated;
}

double NMEA::NMEAtoDecimal(const char *nmea_coord, char direction) {
    if (strlen(nmea_coord) < 3) return 0.0;

    double deg_min = atof(nmea_coord);
    int degrees = (int)(deg_min / 100);
    double minutes = deg_min - (degrees * 100);
    double decimal = degrees + (minutes / 60.0);

    if (direction == 'S' || direction == 'W') decimal *= -1;

    return decimal;
}

uint8_t NMEA::parseNMEALine(char *line) {
    if(strstr(line, "$GNGGA") != NULL) return parseGNGGA(line);
    if(strstr(line, "$GNRMC") != NULL) return parseGNRMC(line);
    return 0;
}

uint8_t NMEA::parseGNRMC(char *line) {
    char *token;
    int field = 0;

    token = getNextToken(line, &field);
    while (token != NULL) {
        switch (field) {
            case 9: // UTC Date
                strncpy(date, token, sizeof(date));
                return 1;
        }
        token = getNextToken(NULL, &field);
    }
    return 0;
}
    
uint8_t NMEA::parseGNGGA(char *line) {
    char *token;
    int field = 0;
    char time_buf[11] = {0};
    char lat_buf[16] = {0}, lat_dir = 0;
    char lon_buf[16] = {0}, lon_dir = 0;
    double tempSat = 0, tempAlt = 0, tempFixQual = 0;

    token = getNextToken(line, &field);
    while (token != NULL) {
        switch (field) {
            case 1: // Time
                strncpy(time_buf, token, sizeof(time_buf));
                break;
            case 2: // Latitude
                strncpy(lat_buf, token, sizeof(lat_buf));
                break;
            case 3: // N/S
                lat_dir = token[0];
                break;
            case 4: // Longitude
                strncpy(lon_buf, token, sizeof(lon_buf));
                break;
            case 5: // E/W
                lon_dir = token[0];
                break;
            case 6: // Fix status
                tempFixQual = atoi(token);
                break;
            case 7: // Satellites used
                tempSat = atoi(token);
                break;
            case 9: // Altitude
                tempAlt = atof(token);
                break;
        }
        token = getNextToken(NULL, &field);
    }

    if(field >= 9) {
        strncpy(time, time_buf, sizeof(time));
        satellitesUsed = tempSat;
        fixQuality = tempFixQual;

        // fixQuality == 0 means is not positioned.
        if(fixQuality != 0) {
            latitude = NMEAtoDecimal(lat_buf, lat_dir);
            longitude = NMEAtoDecimal(lon_buf, lon_dir);
            altitude = tempAlt;
        }
        return 1;
    }
    return 0;
}

char* NMEA::getNextToken(char *str, int* fieldNumber) {
    // In NMEA you can find the following:
    // $GNRMC,160650.00,V,,,,,,,050725,,,N*62\r\n
    // 0x20007e8c "$GPGSV,3,3,11,21,66,161,18,22,29,144,16,30,58,052,13*43\r\n$GLGSV,3,1,10,65,03,111,12,66,45,083,23,67,55,342,33,68,14,307,23*65\r\n$GLGSV,3,2,10,76,44,046,21,77,63,137,20,78,15,188,,82,14,253,39*6C\r\n"
    // -strtok() will stop giving tokens after the V. If the delims are in a row, strtok returns 
    // NULL and won't return anything more.
    // - This function returns the next available token plus updates the field number. In the 
    // previous example:
    // 1st call: return $GNRMC    tokenNumber = 0
    // 2nd call: return 160650.00 tokenNumber = 1
    // 3rd call: return V         tokenNumber = 2
    // 4th call: return 050725    tokenNumber = 9  (...)

    static char* input = NULL;
    static int fieldN = 0;

    if(str != NULL) {
        // New sentence to tokenize.
        input = str;
        fieldN = 0;
        while((*input == ',') && (*input != '\0')) {
            fieldN++;
            input++;
        }
    }
    
    if(input == NULL || *input == '\0') {
        // Current sentence is empty.
        return NULL;
    }

    char* ret = input;
    char* endOfToken = NULL;
    *fieldNumber = fieldN;

    // Search for the next comma.
    while(*input != '\0') {
        if((*input == ',') && (*input != '\0')) {
            // Found the end of the field.
            endOfToken = input; 
            do{
                fieldN++;
                input++;
            } while((*input == ',') && (*input != '\0'));
            break;
        }else {
            input++;
        }
    }

    // At this point input will point to the beginning of the next token.

    if(*input == '\0') {
        // Reached the end of the line.
        input = NULL;
    }else {
        // Delimit the token by substituting the FIRST comma for a 0.
        if(endOfToken != NULL) *endOfToken = '\0';
    }

    return ret;
}
