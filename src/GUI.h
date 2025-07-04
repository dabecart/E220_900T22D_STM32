#ifndef GUI_h
#define GUI_h

#include "ST7735.h"

class GUI {
    public:
    GUI(SPI_HandleTypeDef* hspi);
    ~GUI();

    void init();

    ST7735 tft;
};

#endif // GUI_h