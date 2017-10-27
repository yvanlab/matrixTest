

#ifndef periferic_h
#define periferic_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"
//#include "WProgram.h"
#else
#include "Arduino.h"
#include "WProgram.h"

#endif

#include "BaseManager.h"

class Periferic : public BaseManager
{
  public:
    Periferic(unsigned char pinLed);
    String collectJson(String ipAddress);
    void retrievePeriphericInfo();

    String getClassName(){return "Periferic";}

    // current
    uint8_t status_current;
    float instant_current; //Ampere
    float kwh_current;
    float instant_kwh_current;
    String  lastMesureDate_current;

    // meteo
    uint8_t status_meteoVmc;
    float extTemp_meteoVmc;
    int8_t trendMeteo_meteoVmc;
    int8_t vtsVMC_meteoVmc;
    float tempVMC_meteoVmc;
    String lastMesureDate__meteoVmc;


  private:
    String getBlock(String jSon, String key);
    String getValue(String jSon, String key);



};


#endif
