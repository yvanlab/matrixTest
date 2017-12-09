

#include "SettingManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

SettingManager::SettingManager(unsigned char pinLed) : BaseSettingManager(pinLed){

}

String SettingManager:: toString(boolean bJson = false){
  //return "SSID[" + String(m_ssid) + "] PASS[" + m_password + "] privateKey[" + m_privateKey + "] publicKey[" + m_publicKey + "] Hum["+m_HUM_SEUIL+"]";
  return BaseSettingManager::toString(bJson) + "Text["+m_textToDisplay+"]";
}
unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();
  readEEPROM(m_textToDisplay);
  switchOff();
}
unsigned char SettingManager::writeData(){
  BaseSettingManager::writeData();
  switchOn();
  writeEEPROM(m_textToDisplay);
  EEPROM.commit();
  switchOff();
}
