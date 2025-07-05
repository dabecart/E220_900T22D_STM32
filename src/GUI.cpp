#include "GUI.h"

GUI* gui;
char tempBuffer[200];

GUI::GUI(SPI_HandleTypeDef* hspi) : tft(hspi) {
    gui = this;
}

GUI::~GUI() {}

void GUI::init() {
    tft.init(1);
    tft.fillScreen(BLACK);

    fillGridCage(0, 0, "Input:",     BLACK, RED);
    fillGridCage(0, 2, "RSSI:",      BLACK, GREEN);

    fillGridCage(1, 0, "Latitude:",  BLACK, CYAN);
    fillGridCage(1, 2, "Longitude:", BLACK, YELLOW);
    fillGridCage(1, 4, "Altitude:",  BLACK, color565(0xff, 0xa2, 0x00)); // Orange
    fillGridCage(1, 6, "Time:",      BLACK, MAGENTA);
}

void GUI::updateLoRa(char* msg, uint16_t msgLen, int16_t rssi, uint8_t channel, uint16_t crcErrors) {
    if(msg != NULL) {
        snprintf(tempBuffer, sizeof(tempBuffer), "%.*s", msgLen, msg);
        fillGridCage(0, 1, tempBuffer, WHITE, BLACK);
    }

    snprintf(tempBuffer, sizeof(tempBuffer), "%d", rssi);
    fillGridCage(0, 3, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "CRC: %d", crcErrors);
    fillGridCage(0, 10, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "Ch. %d", channel);
    fillGridCage(0, 11, tempBuffer, WHITE, BLACK);
}

void GUI::updateNMEA(double lat, double lon, double alt, char* time, uint8_t fixQuality, uint8_t satUsed) {
    snprintf(tempBuffer, sizeof(tempBuffer), "%.6f", lat);
    fillGridCage(1, 1, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "%.6f", lon);
    fillGridCage(1, 3, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "%.6f", alt);
    fillGridCage(1, 5, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "%s", time);
    fillGridCage(1, 7, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "Fix: %d", fixQuality);
    fillGridCage(1, 10, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "Sats: %d", satUsed);
    fillGridCage(1, 11, tempBuffer, WHITE, BLACK);
}

void GUI::fillGridCage(uint16_t x, uint16_t y, const char* msg, uint16_t fontColor, uint16_t backColor) {
    if(msg == NULL) return;

    tft.fillRectangle(
        x*GRID_CELL_WIDTH, y*GRID_CELL_HEIGHT, GRID_CELL_WIDTH, GRID_CELL_HEIGHT, backColor);
    tft.writeString(x*GRID_CELL_WIDTH, y*GRID_CELL_HEIGHT, msg, FONT, fontColor, backColor);
}

