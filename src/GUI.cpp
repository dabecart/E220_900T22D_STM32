#include "GUI.h"

GUI* gui;
char tempBuffer[128];

GUI::GUI(SPI_HandleTypeDef* hspi) : tft(hspi) {
    gui = this;
}

GUI::~GUI() {}

void GUI::init() {
    tft.init(1);
    tft.fillScreen(BLACK);

    fillGridCage(0, 1, "Input:",     BLACK, RED);
    fillGridCage(0, 3, "RSSI:",      BLACK, GREEN);

    fillGridCage(1, 1, "Latitude:",  BLACK, CYAN);
    fillGridCage(1, 3, "Longitude:", BLACK, YELLOW);
    fillGridCage(1, 5, "Altitude:",  BLACK, MAGENTA);
}

void GUI::updateLoRa(char* msg, uint16_t msgLen, int16_t rssi, uint8_t channel, uint16_t crcErrors) {
    if(msg != NULL) {
        snprintf(tempBuffer, sizeof(tempBuffer), "%.*s", msgLen, msg);
        fillGridCage(0, 2, tempBuffer, WHITE, BLACK);
    }

    snprintf(tempBuffer, sizeof(tempBuffer), "%d", rssi);
    fillGridCage(0, 4, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "CRC: %d", crcErrors);
    fillGridCage(0, 10, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "Ch. %d", channel);
    fillGridCage(0, 11, tempBuffer, WHITE, BLACK);
}

void GUI::updateNMEA(const NMEA* nmea) {
    int len = nmea->getFormattedDate(tempBuffer, sizeof(tempBuffer));
    tempBuffer[len++] = ' ';
    nmea->getFormattedTime(tempBuffer + len, sizeof(tempBuffer) - len);
    tft.fillRectangle(0, 0, tft.width, GRID_CELL_HEIGHT, WHITE);
    tft.writeString(0, 0, tempBuffer, FONT, BLACK, WHITE);

    snprintf(tempBuffer, sizeof(tempBuffer), "%.8f", nmea->latitude);
    fillGridCage(1, 2, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "%.8f", nmea->longitude);
    fillGridCage(1, 4, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f", nmea->altitude);
    fillGridCage(1, 6, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "Fix: %d", nmea->fixQuality);
    fillGridCage(1, 10, tempBuffer, WHITE, BLACK);

    snprintf(tempBuffer, sizeof(tempBuffer), "Sats: %d", nmea->satellitesUsed);
    fillGridCage(1, 11, tempBuffer, WHITE, BLACK);
}

void GUI::fillGridCage(uint16_t x, uint16_t y, const char* msg, uint16_t fontColor, uint16_t backColor) {
    if(msg == NULL) return;

    tft.fillRectangle(
        x*GRID_CELL_WIDTH, y*GRID_CELL_HEIGHT, GRID_CELL_WIDTH, GRID_CELL_HEIGHT, backColor);
    tft.writeString(x*GRID_CELL_WIDTH, y*GRID_CELL_HEIGHT, msg, FONT, fontColor, backColor);
}

