#include "GUI.h"

GUI* gui;

GUI::GUI(SPI_HandleTypeDef* hspi) : tft(hspi) {
    gui = this;
}

GUI::~GUI() {}

void GUI::init() {
    tft.init(1);
    tft.fillScreen(BLACK);
}
