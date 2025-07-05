#ifndef NMEA_h
#define NMEA_h

#include <stdlib.h>
#include <stdio.h>
#include "UART.h"

class NMEA {
    public:
    NMEA(UART* uart);
    ~NMEA();

    uint8_t update();

    uint16_t getFormattedTime(char* out, uint16_t maxOutLen) const;
    uint16_t getFormattedDate(char* out, uint16_t maxOutLen) const;

    private:
    double NMEAtoDecimal(const char *nmea_coord, char direction);
    uint8_t parseNMEALine(char *line);
    uint8_t parseGNGGA(char *line);
    uint8_t parseGNRMC(char *line);

    char* getNextToken(char *str, int* fieldNumber);

    UART* uart;
    uint16_t gps_index = 0;

    public:
    char time[11] = {0};         // hhmmss.sss (UTC)
    char date[7] = {0};          // ddmmyy (UTC)
    double latitude = 0;         // Decimal degrees
    double longitude = 0;        // Decimal degrees
    double altitude = 0;         // Meters
    uint8_t fixQuality = 0;      // 0 = invalid, 1 = fix, etc.
    uint8_t satellitesUsed = 0;  // Number of satellites used to generate the position.
};

#endif // NMEA_h