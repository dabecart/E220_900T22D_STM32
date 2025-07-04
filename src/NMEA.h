#ifndef NMEA_h
#define NMEA_h

#include <stdlib.h>
#include "UART.h"

class NMEA {
    public:
    NMEA(UART* uart);
    ~NMEA();

    void update();

    private:
    double NMEAtoDecimal(const char *nmea_coord, char direction);
    void parseNMEALine(char *line);

    UART* uart;
    uint16_t gps_index = 0;

    public:
    char time[11];         // hhmmss.sss (UTC)
    double latitude;       // Decimal degrees
    double longitude;      // Decimal degrees
    int fix_quality;       // 0 = invalid, 1 = fix, etc.

};

#endif // NMEA_h