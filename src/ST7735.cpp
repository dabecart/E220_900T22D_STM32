#include "ST7735.h"

const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100                     //     100 ms delay
};

ST7735::ST7735(SPI_HandleTypeDef* hspi) {
    this->hspi = hspi;
}

ST7735::~ST7735() {}

void ST7735::init(uint8_t rotation) {
    select();
    reset();
    displayInit(init_cmds1);
    displayInit(init_cmds2);
    displayInit(init_cmds3);
    colstart = 0;
    rowstart = 0;
    setRotation(rotation);
    unselect();
}

void ST7735::setRotation(uint8_t m) {
    uint8_t madctl = 0;

    rotation = m % 4; // can't be higher than 3

    switch (rotation) {
        case 0:
            madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB;
            height = ST7735_HEIGHT;
            width = ST7735_WIDTH;
            xstart = colstart;
            ystart = rowstart;
            break;

        case 1:
            madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
            width = ST7735_HEIGHT;
            height = ST7735_WIDTH;
            ystart = colstart;
            xstart = rowstart;
            break;

        case 2:
            madctl = ST7735_MADCTL_RGB;
            height = ST7735_HEIGHT;
            width = ST7735_WIDTH;
            xstart = colstart;
            ystart = rowstart;
            break;
        
        case 3:
            madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
            width = ST7735_HEIGHT;
            height = ST7735_WIDTH;
            ystart = colstart;
            xstart = rowstart;
            break;
    }

    select();
    writeCommand(ST7735_MADCTL);
    writeData(&madctl,1);
    unselect();
}

void ST7735::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= width) || (y >= height)) return;

    select();

    setAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    writeData(data, sizeof(data));

    unselect();
}

void ST7735::writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    setAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
                writeData(data, sizeof(data));
            } else {
                uint8_t data[] = {(uint8_t)(bgcolor >> 8), (uint8_t)(bgcolor & 0xFF)};
                writeData(data, sizeof(data));
            }
        }
    }
}

void ST7735::writeString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    select();

    while(*str) {
        if(x + font.width >= width) {
            x = 0;
            y += font.height;
            if(y + font.height >= height) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        writeChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    unselect();
}

void ST7735::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if((x >= width) || (y >= height)) return;
    if((x + w - 1) >= width) w = width - x;
    if((y + h - 1) >= height) h = height - y;

    select();
    setAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    HAL_GPIO_WritePin(TFT_A0_GPIO_Port, TFT_A0_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(hspi, data, sizeof(data), HAL_MAX_DELAY);
        }
    }

    unselect();
}

void ST7735::fillScreen(uint16_t color) {
    fillRectangle(0, 0, width, height, color);
}

void ST7735::drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= width) || (y >= height)) return;
    if((x + w - 1) >= width) return;
    if((y + h - 1) >= height) return;

    select();
    setAddressWindow(x, y, x+w-1, y+h-1);
    writeData((uint8_t*)data, sizeof(uint16_t)*w*h);
    unselect();
}

void ST7735::invertColors(bool invert) {
    select();
    writeCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    unselect();
}

void ST7735::select() {
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);
}

void ST7735::unselect() {
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

void ST7735::reset() {
    HAL_GPIO_WritePin(TFT_RESET_GPIO_Port, TFT_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(TFT_RESET_GPIO_Port, TFT_RESET_Pin, GPIO_PIN_SET);
}

void ST7735::writeCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(TFT_A0_GPIO_Port, TFT_A0_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

void ST7735::writeData(uint8_t* buff, size_t buff_size) {
    HAL_GPIO_WritePin(TFT_A0_GPIO_Port, TFT_A0_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(hspi, buff, buff_size, HAL_MAX_DELAY);
}

void ST7735::displayInit(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        writeCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            writeData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

void ST7735::setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // Column address set
    writeCommand(ST7735_CASET);
    uint8_t data[] = {0x00, (uint8_t)(x0 + xstart), 0x00, (uint8_t)(x1 + xstart)};
    writeData(data, sizeof(data));

    // Row address set
    writeCommand(ST7735_RASET);
    data[1] = y0 + ystart;
    data[3] = y1 + ystart;
    writeData(data, sizeof(data));

    // Write to RAM
    writeCommand(ST7735_RAMWR);
}