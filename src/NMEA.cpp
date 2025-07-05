#include "NMEA.h"

NMEA::NMEA(UART* uart) {
    this->uart = uart;
}

NMEA::~NMEA() {}

uint8_t NMEA::update() {
    uint8_t lineBuffer[256];
    uint16_t bufferIndex = 0;

    // Find a line in the RX buffer that starts with '$'.
    while(uart->RXBuffer.peek(lineBuffer) && (lineBuffer[0] != '$')) {
        uart->RXBuffer.pop(NULL);
    }

    while(uart->RXBuffer.pop(lineBuffer + bufferIndex) && (lineBuffer[bufferIndex] != '\n')) {
        if(bufferIndex >= sizeof(lineBuffer)) {
            // Message is too long!
            uart->RXBuffer.pop(NULL);
            return 0;
        }
        bufferIndex++;
    }

    if(lineBuffer[bufferIndex] != '\n') {
        // Message is incomplete.
        return 0;
    } 

    // Found a message!
    lineBuffer[bufferIndex + 1] = 0; // Put a NULL at the end of the string.
    return parseNMEALine((char*) lineBuffer);
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
    if(strstr(line, "$GNGGA") == NULL) return 0;

    char *token;
    int field = 0;
    char lat_buf[16] = {0}, lat_dir = 0;
    char lon_buf[16] = {0}, lon_dir = 0;

    token = strtok(line, ",");
    while (token != NULL) {
        switch (field) {
            case 1: // Time
                strncpy(time, token, sizeof(time));
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
                fixQuality = atoi(token);
                break;
            case 7: // Satellites used
                satellitesUsed = atoi(token);
                break;
            case 9: // Altitude
                altitude = atof(token);
                break;
        }
        token = strtok(NULL, ",");
        field++;
    }

    // fixQuality == 0 means is not positioned.
    if(fixQuality != 0) {
        latitude = NMEAtoDecimal(lat_buf, lat_dir);
        longitude = NMEAtoDecimal(lon_buf, lon_dir);
    }
    return 1;
}
