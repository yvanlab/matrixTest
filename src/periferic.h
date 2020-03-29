

#ifndef periferic_h
#define periferic_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"
//#include "WProgram.h"
#else
#include "Arduino.h"
#include "WProgram.h"

#endif

#include "baseManager.h"

class Periferic : public BaseManager
{
  public:
    Periferic(unsigned char pinLed);
    String collectJson(String ipAddress);
    String buildPerifericOutput(String perifericName, uint8_t statusCode);
    void retrievePeriphericInfo();

    String getExtTemp()     {return m_ExtTemp;}
    String getExtTempMax()  {return m_ExtTempMax;}
    String getExtTempMin()  {return m_ExtTempMin;}
    int8_t getTrendMeteo()  {return m_ExtTrendMeteo;}
    String getVMVvts()      {return m_VMVvts;}
    String getVMCtemp()     {return m_VMCtemp;}

    String getInstantCurrent1()   {return m_InstantCurrent1;}
    String getInstantCurrent2()   {return m_InstantCurrent2;}
    String getDifferential()      {return m_Differential;}
    String geTemperarture()       {return m_Temperarture;}
    String getInstantCurrent3()   {return m_InstantCurrent3;}
    String getKWH1()              {return m_KWH1;}



    String getDaySaintName()    {return getKeyValue(m_saintJson,"name");}


    String getMeteoVMCJson()  {return  m_meteoVMCJson;};
    String getcurrentJson()   {return  m_currentJson;};
    String getSnorerVMCJson() {return  m_snorerJson;};
    String getsaintJson()     {return  m_saintJson;};
    String getClassName()     {return "Periferic";}

    String getVmcMetoValue(String key);
    String getCurrentValue(String key);
    String getSonorerValue(String key);

    int sendToVoiceBox(String text);

    // current
    String m_InstantCurrent1;
    String m_InstantCurrent2;
    String m_Differential;
    String m_Temperarture;
    String m_InstantCurrent3;
    String m_KWH1;

    // meteo
    String m_ExtTemp;
    String m_ExtTempMax;
    String m_ExtTempMin;
    uint8_t m_ExtTrendMeteo;
    String m_VMVvts;
    String m_VMCtemp;



    String m_meteoVMCJson;
    String m_currentJson;
    String m_snorerJson;
    String m_saintJson;
    //String m_snorerJson;

  private:
    String getBlock(String jSon, String key);
    String getValue(String jSon, String key);
    String getKeyValue(String jSon, String key);


};


#endif
