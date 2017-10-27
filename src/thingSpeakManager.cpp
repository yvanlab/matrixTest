
#include "thingSpeakManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


thingSpeakManager::thingSpeakManager( unsigned char pinLed) : BaseManager(pinLed)  {
  m_index  = 0;
}

void thingSpeakManager::addVariable(uint8_t index, String value) {
  m_variables[m_index].Index=index;
  m_variables[m_index].Value=value;
  m_index++;
}

unsigned char thingSpeakManager::sendIoT(String privateKey/*ChannelNumber*/, String publicKey /*WriteAPIKey*/) {
  switchOn();

  uint8_t i = 0;
  while (i < m_index) {
    ThingSpeak.setField(m_variables[i].Index,m_variables[i].Value);
    i++;
  }
  m_index = 0;
  ThingSpeak.writeFields(atol(privateKey.c_str()), publicKey.c_str());
  switchOff();
  return 0;
}

String thingSpeakManager::toString(boolean bJson = false) {
  String res = "variables :";
  unsigned char i=0;
  while (i < m_index) {
    res += "\""+String(m_variables[i].Index) + "\" : \"" + String(m_variables[i].Value+"\"");
    i++;
  }
  return res;
}
