#ifndef __ICON_H__
#define __ICON_H__


struct MyICON {
  MyICON(uint8_t _nbRow, uint8_t _nbCol, uint8_t _NbColor, const uint8_t *_pIcon) : nbRow(_nbRow),nbCol(_nbCol),NbColor(_NbColor), pIcon(_pIcon){}
  uint8_t nbRow;
  uint8_t nbCol;
  uint8_t NbColor;
  const uint8_t *pIcon ;
};


#endif
