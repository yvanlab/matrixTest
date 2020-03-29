

#include "settingManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

SettingManager::SettingManager(unsigned char pinLed) : BaseSettingManager(pinLed){

}

String SettingManager:: toString(boolean bJson = false){
  if (bJson ==STD_TEXT)
    return BaseSettingManager::toString(bJson) + "Text["+m_textToDisplay+"]";
  else
    return "\"text\":\""+String (m_textToDisplay) + "\"";
  }
unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();
  readEEPROM(m_textToDisplay,200);
  switchOff();
}
unsigned char SettingManager::writeData(){
  BaseSettingManager::writeData();
  switchOn();
  writeEEPROM(m_textToDisplay);
  EEPROM.commit();
  switchOff();
}
