

#ifndef SettingManager_h
#define SettingManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <EEPROM.h>
#include <BaseSettingManager.h>

class SettingManager : public BaseSettingManager
{
  public:
    /*char m_ssid[32];
    char m_password[64];
    char m_privateKey[64];
    char m_publicKey[64];*/
    char m_textToDisplay[200];
    SettingManager(unsigned char pinLed);
    String toString(boolean bJson);
    virtual unsigned char readData();
    virtual unsigned char writeData();
    /*unsigned char clearData();*/
    String getClassName(){return "SettingManager";}
  private:
  /*  unsigned char writeEEPROM(char *str);
    unsigned char readEEPROM(char *str);
    int m_iEEprom = 0;*/
};

#endif
