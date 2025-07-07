#ifndef GUI_h
#define GUI_h

#include <stdio.h>
#include "ST7735.h"
#include "NMEA.h"

#define FONT Font_7x10
#define FONT_HEIGHT 10

#define GRID_CELL_WIDTH  80
#define GRID_CELL_HEIGHT FONT_HEIGHT

class GUI {
    public:
    GUI(SPI_HandleTypeDef* hspi);
    ~GUI();

    void init();

    void updateLoRa(char* msg, uint16_t msgLen, int16_t rssi, uint8_t channel, uint16_t crcErrors);
    void updateNMEA(const NMEA* nmea);
    void updateSD(uint8_t mounted, uint8_t recording, uint32_t recFileSize);

    private:
    void fillGridCage(uint16_t x, uint16_t y, const char* msg, uint16_t fontColor, uint16_t backColor);

    ST7735 tft;
};

#endif // GUI_h