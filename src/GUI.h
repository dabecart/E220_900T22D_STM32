#ifndef GUI_h
#define GUI_h

#include "ST7735.h"
#include <stdio.h>

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
    void updateNMEA(double lat, double lon, double alt, char* time, uint8_t fixQuality, uint8_t satUsed);

    private:
    void fillGridCage(uint16_t x, uint16_t y, const char* msg, uint16_t fontColor, uint16_t backColor);

    ST7735 tft;
};

#endif // GUI_h