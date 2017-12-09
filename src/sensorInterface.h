

#ifndef sensorInterface_h
#define sensorInterface_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <ST_HW_HC_SR04.h>
#include "BaseManager.h"
//ST_HW_HC_SR04 ultrasonicSensor(pin_SR04_TRG, pin_SR04_ECHO,5000);//23200); // ST_HW_HC_SR04(TRIG, ECHO)

#define MAX_DURATION_ACTIVE 30000//60000 //1mn
#define MAX_DURATION_CFG 30000 //30s
#define TIMEOUT_DETECTION_FAR 58*30//58*nb cm
#define TIMEOUT_DETECTION_CLOSE 58*30 //1mn

class SensorInterface : public ST_HW_HC_SR04,  public BaseManager
{
  public:
    SensorInterface(uint8_t pinLed,uint8_t pin_SR04_TRG, uint8_t pin_SR04_ECHO);
    uint16_t readSensor(uint32_t timeout);

    boolean isPagechangeDetected();
    boolean isPersonDetected();
    boolean isCfgDetected();

    void setPagechangeDetected(){bForcePgeChangeDetected=true;};
    void setPersonDetected(bool on){personDetectedTime=millis() ;if (!on) personDetectedTime-=MAX_DURATION_ACTIVE;};
    void setCfgDetected(bool on){cfgDetectedTime=millis()       ;if (!on) cfgDetectedTime-=MAX_DURATION_CFG;};

    String getClassName(){return "SensorInterface";}
    uint16_t detection = 0;
    uint32_t cfgDetectedTime = 0;
  private:
    uint32_t personDetectedTime = 0;

    uint32_t lastReadTime = 0;
    bool bForcePgeChangeDetected = false;
    uint8_t nbDetected = 0;
};


#endif
