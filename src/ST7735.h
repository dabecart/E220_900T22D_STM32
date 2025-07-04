#ifndef ST7735_h
#define ST7735_h

#include "stm32g4xx_hal.h"
#include "fonts.h"
#include "main.h"

/****** TFT DEFINES ******/
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

#define DELAY 0x80

#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define color565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

class ST7735 {
    public:
    ST7735(SPI_HandleTypeDef* hspi);
    ~ST7735();

    void init(uint8_t rotation);
    void setRotation(uint8_t m);
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    void writeString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void fillScreen(uint16_t color);
    void drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
    void invertColors(bool invert);
    
    private:
    void select();
    void unselect();
    void reset();
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t* buff, size_t buff_size);
    void displayInit(const uint8_t *addr);
    void setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);

    SPI_HandleTypeDef* hspi;
    int16_t cursor_x; // x location to start printing text
    int16_t cursor_y; // y location to start printing text
    uint8_t rotation; // Display rotation (0 thru 3)
    uint8_t colstart; // Some displays need this changed to offset
    uint8_t rowstart; // Some displays need this changed to offset
    uint8_t xstart;
    uint8_t ystart;

    public:
    int16_t width;    // Display width as modified by current rotation
    int16_t height;   // Display height as modified by current rotation
};

#endif // ST7735_h

