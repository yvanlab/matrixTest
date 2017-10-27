#ifndef __FONT_H__
#define __FONT_H__


struct MyFONT {
  MyFONT(uint8_t _nbRow,uint8_t _nbCol,uint16_t _firstChar, const uint8_t *_pFont) : nbRow(_nbRow),nbCol(_nbCol),firstChar(_firstChar),pFont(_pFont){}
  uint8_t nbRow;
  uint8_t nbCol;
  uint16_t firstChar;
  const uint8_t *pFont ;
};


#endif
