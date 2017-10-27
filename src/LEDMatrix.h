/**
 * LED Matrix library for https://www.seeedstudio.com/Ultrathin-16x32-RGB-LED-Matrix-Panel-p-1926.html
 * The LED Matrix panel has 32x16 pixels.
 *
 * Inspired by https://github.com/Seeed-Studio/Ultrathin_LED_Matrix/
 *
 * Revised and adapted for RGB panels by Mathieu Agopian
 */


#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__

#include <stdint.h>
#include "font/font.h"
#include "icon/icon.h"
#include "Arduino.h"

struct DisplayWindow {
  //DisplayWindow(int8_t _x,int8_t _y,int8_t _width, int8_t _height) : x(_x),y(_y),width(_width),height(_height){};
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;
};

#define MASK_PIXEL_COLOR  0b00000011
#define MASK_PIXEL_ACTION 0b11110000

#define PC_BLACK          0b00000000
#define PC_SHADOW         0b00000001
#define PC_DARK           0b00000010
#define PC_BRIGHT         0b00000011


//#define PC_ON_3           0b00001000

#define PA_INVERT         0b00010000
#define PA_BLINK_25MS     0b00100000 //every 25 ms
#define PA_BLINK_250MS    0b01000000 //every 250 ms
#define PA_BLINK_1S       0b10000000 //every second

#define SWITCH_OFF_SCREEN false
#define SWITCH_ON_SCREEN  true


class LEDMatrix {
public:
    LEDMatrix(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, /*uint8_t g1, uint8_t b1, */uint8_t stb, uint8_t clk);

    /**
     * @param displaybuf    display buffer
     * @param width         panels' width
     * @param height        panel's height
     */
    void begin(uint8_t *displaybuf, int16_t width, int16_t height);

    /**
     * draw a point
     * @param x     x
     * @param y     y
     * @param color coded on 3 bits for RGB, eg: 1: Red, 2: Green, 4: Blue, 7: White (Red + Green + Blue)
     */
    void setDisplayWindows(int16_t _x, int16_t _y,int16_t _width, int16_t _height) {
      win.x=_x;
      win.y=_y;
      win.width=_width;
      win.height=_height;
    };
    void drawPoint(int16_t x, int16_t y, uint8_t color);
    void drawText(int16_t x, int16_t y, const char *text, MyFONT font,  uint8_t color);
    void drawChar(int16_t x, int16_t y, char n, MyFONT font,  uint8_t color);

    void drawImage(int16_t x, int16_t y, MyICON icon, uint8_t pixelAction);
    void drawVerticaleLine(int16_t x, int16_t yHight,int16_t yLow, uint8_t color);
    void drawHorizontalLine(int16_t xLeft, int16_t y,int16_t xRight, uint8_t color);
    void drawSquare(int16_t x, int16_t y,int16_t xLeft,int16_t yLow, uint8_t color);
    void clear();

    void switchScreenStatus(boolean _isOn) {isOn = _isOn;};
    boolean isScreenActivated(){return isOn;};
    /**
     * Scan the whole display buffer and "apply" it to the leds.
     */
    void scan(uint8_t row);
    void displayScreen();

private:
    uint8_t clk, r1, /*g1, b1,*/ stb, oe, a, b, c, d;
    uint8_t *displaybuf;
    uint16_t width;
    uint16_t height;
    DisplayWindow win;
    boolean isOn = true;
    uint32_t   greyColor=0;
};

#endif
