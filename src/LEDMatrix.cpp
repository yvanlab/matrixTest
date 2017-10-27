/**
 * LED Matrix library for https://www.seeedstudio.com/Ultrathin-16x32-RGB-LED-Matrix-Panel-p-1926.html
 * The LED Matrix panel has 32x16 pixels.
 *
 * Inspired by https://github.com/Seeed-Studio/Ultrathin_LED_Matrix/

 * Revised and adapted for RGB panels by Mathieu Agopian
 */

#include "LEDMatrix.h"
#include "Arduino.h"
#include "BaseManager.h"
#include "context.h"


LEDMatrix::LEDMatrix(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, /*uint8_t g1, uint8_t b1,*/ uint8_t stb, uint8_t clk) {
    this->clk = clk;
    this->r1 = r1;
    /*this->g1 = g1;
    this->b1 = b1;*/
    this->stb = stb;
    this->oe = oe;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;

    //greyColor = 1;
}

void LEDMatrix::begin(uint8_t *displaybuf, int16_t width, int16_t height) {
    this->displaybuf = displaybuf;
    this->width = width;
    this->height = height;
    setDisplayWindows(0,0, width, height);

    pinMode(a, OUTPUT);
    pinMode(b, OUTPUT);
    pinMode(c, OUTPUT);
    pinMode(d, OUTPUT);
    pinMode(oe, OUTPUT);
    pinMode(r1, OUTPUT);
    /*pinMode(g1, OUTPUT);
    pinMode(b1, OUTPUT);*/
    pinMode(clk, OUTPUT);
    pinMode(stb, OUTPUT);
}

void LEDMatrix::drawPoint(int16_t x, int16_t y, uint8_t color) {
    uint16_t index = x + y * width;
    displaybuf[index] = color;
}

void LEDMatrix::drawHorizontalLine(int16_t xLeft, int16_t y,int16_t xRight, uint8_t color) {
  int16_t xDot = win.x+xLeft;
  int16_t yDot = win.y+y;
  uint16_t indexBuffer = xDot + yDot * width;

  for (int8_t x=xLeft;x<xRight;x++) {
    if ((xDot>=win.x  && xDot<win.width) && (yDot>=win.y  && yDot<=win.height)) {
      displaybuf[indexBuffer] = color;
      indexBuffer++;
      xDot++;
    } else
      break;
  }
}

void LEDMatrix::drawVerticaleLine(int16_t x, int16_t yHight,int16_t yLow, uint8_t color) {
  int16_t xDot = win.x+x;
  int16_t yDot = win.y+yHight;
  uint16_t indexBuffer = xDot + yDot * width;

  for (int8_t y=yHight;y<=yLow;y++) {
    if ((xDot>=win.x  && xDot<win.width) && (yDot>=win.y  && yDot<=win.height)) {
      displaybuf[indexBuffer] = color;
      yDot++;
      indexBuffer+= width;
    };
  }
}

void LEDMatrix::drawSquare(int16_t x, int16_t y,int16_t xRight,int16_t yLow, uint8_t color) {
  drawHorizontalLine(x     ,y   ,xRight, color);
  drawHorizontalLine(x     ,yLow,xRight, color);
  drawVerticaleLine (x     ,y   ,yLow , color);
  drawVerticaleLine (xRight,y   ,yLow , color);
}

void LEDMatrix::displayScreen() {
  if (!isScreenActivated() ) return;
  greyColor++;
  for (uint8_t row=0 ;row<height;row++)
    scan(row);
}

void LEDMatrix::scan(uint8_t row) {
    //static uint8_t row = 0;  // from 0 to 15

    uint8_t *ptr = displaybuf + row * width;
    uint8_t pixelColor;

    for (uint8_t col = 0; col < width; col++) {
        uint8_t pixel = *ptr;
        uint8_t bDisplay = false;
        pixelColor = pixel & MASK_PIXEL_COLOR;
        if (pixelColor == PC_BRIGHT)
          bDisplay = true;
        else if (pixelColor == PC_SHADOW)
          bDisplay = (greyColor%3)==0;
        else if (pixelColor == PC_DARK)
          bDisplay = (greyColor%7)==0;
      /*  else if (pixel & PC_ON_3)
          bDisplay = (greyColor%7)==0;*/

        if (bDisplay) {
          if (pixel & PA_BLINK_25MS)
            bDisplay = mtTimer.is25MSFrequence();
          else if (pixel & PA_BLINK_250MS)
            bDisplay = mtTimer.is250MSFrequence();
          else if (pixel & PA_BLINK_1S)
            bDisplay = mtTimer.is1SFrequence();

          if (pixel & PA_INVERT)
            bDisplay = bDisplay ^ true;
        }

        digitalWrite(clk, LOW);

        digitalWrite(r1, bDisplay);

        digitalWrite(clk, HIGH);

        ptr++;
    }

    digitalWrite(oe, HIGH);              // disable display

    // select row
    digitalWrite(a, (row & 0x01));
    digitalWrite(b, (row & 0x02));
    digitalWrite(c, (row & 0x04));
    digitalWrite(d, (row & 0x08));

    // latch data
    digitalWrite(stb, LOW);
    digitalWrite(stb, HIGH);
    digitalWrite(stb, LOW);

    digitalWrite(oe, LOW);              // enable display

    //row = (row + 1) & 0x0F;
    //Serial.println(0);
    /*if (row == 0) {
      greyColor++;
      if (greyColor > 254)
        greyColor = 1;
      //Serial.println(greyColor);
    }*/

}



void LEDMatrix::drawChar(int16_t x, int16_t y, char n, MyFONT font, uint8_t color) {
    byte charbytes[16];
    uint16_t indexFont;
    uint16_t indexBuffer;
    int16_t xDot;
    int16_t yDot;

    indexFont = (n-font.firstChar)*font.nbRow; // go to the right code for this character*/
    xDot = win.x+x;
    yDot = win.y+y;
    indexBuffer = xDot + yDot * width;
    for (byte i=0;i<font.nbRow;i++) {  // fill up the charbytes array with the right bits
      charbytes[i] = pgm_read_byte(&(font.pFont[indexFont+i]));
      //if (inverse) charbytes[i] = ~charbytes[i];
    };
    const byte *pSrc = charbytes; // point at the first set of 8 pixels
    for (byte i=0; i<font.nbRow; i++) {
        byte iMask = 0b00000001;
        for (byte iBit=0; iBit<font.nbCol; iBit++) {
          if ((xDot>=win.x  && xDot<win.width) && (yDot>=win.y  && yDot<=win.height)) {
            if (*pSrc&iMask) {
              //Serial.print('1');
              displaybuf[indexBuffer] = color;
            }
            else{
              //Serial.print('0');
              displaybuf[indexBuffer] = 0;
              //Serial.print("-0");
            }
          }
          xDot++;
          indexBuffer++;
          iMask=iMask<<1;
        //Serial.print("X:");Serial.print(xDot);Serial.print(" y:");Serial.println(yDot);
        }
        indexBuffer += width-font.nbCol;
        yDot++;
        xDot = win.x+x;
        pSrc++;            // go to next set of 8 pixels in character
    }
};



void  LEDMatrix::drawText(int16_t x, int16_t y, const char *text, MyFONT font, uint8_t color) {
  //DEBUGLOG(text);
  for (uint8_t i=0;text[i] !=0;i++) {
    drawChar(x+(i*font.nbCol), y, text[i],  font, color);
  }
}

void  LEDMatrix::clear() {
  for (uint16_t i=0;i<height*width;i++) {
    displaybuf[i]=0;
  }
}

void LEDMatrix::drawImage(int16_t x, int16_t y, MyICON icon, uint8_t pixelAction) {
  int16_t xDot        = win.x+x;
  int16_t yDot        = win.y+y;
  int16_t indexIcon   = 0;
  uint16_t indexDisplayBuffer = xDot + yDot * width;
  uint8_t fourPixels  = 0;
  //Serial.println("");
  for (byte y=0; y<icon.nbRow;y++) {
    //Serial.printf("row %d : ",yDot);
    for (byte x=0; x<icon.nbCol/4;x++) {
      fourPixels = pgm_read_byte(&(icon.pIcon[indexIcon]));
      //Serial.printf("fourPixels : %d\n",fourPixels);
      for (int8_t ifourPixelsIndex=3;ifourPixelsIndex>=0;ifourPixelsIndex--) {
        if ((xDot>=win.x  && xDot<win.width) && (yDot>=win.y  && yDot<=win.height)) {
          displaybuf[indexDisplayBuffer] = ((fourPixels>>(2*ifourPixelsIndex)) & MASK_PIXEL_COLOR ) | (pixelAction & MASK_PIXEL_ACTION);
          /*if (displaybuf[indexDisplayBuffer] == PC_BLACK) Serial.print('-');
          if (displaybuf[indexDisplayBuffer] == PC_SHADOW) Serial.print('*');
          if (displaybuf[indexDisplayBuffer] == PC_BRIGHT) Serial.print('+');*/
          //erial.print(displaybuf[indexDisplayBuffer]);
          //Serial.print(ifourPixelsIndex);
        }
        xDot++;
        indexDisplayBuffer++;
      }
      indexIcon++;
    }
    indexDisplayBuffer += width-icon.nbCol;
    yDot++;
    xDot = win.x+x;
    //Serial.println("");
  }
}
