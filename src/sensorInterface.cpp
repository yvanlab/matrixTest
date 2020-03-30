


#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <ST_HW_HC_SR04.h>
#include "baseManager.h"
#include "sensorInterface.h"
//ST_HW_HC_SR04 ultrasonicSensor(pin_SR04_TRG, pin_SR04_ECHO,5000);//23200); // ST_HW_HC_SR04(TRIG, ECHO)




SensorInterface::SensorInterface(uint8_t pinLed,uint8_t pin_SR04_TRG, uint8_t pin_SR04_ECHO)
 : BaseManager(pinLed), ST_HW_HC_SR04(pin_SR04_TRG,pin_SR04_ECHO,TIMEOUT_DETECTION_FAR) {
   personDetectedTime = 0;
   pinMode(A0,INPUT_PULLDOWN_16);
   digitalWrite(A0,LOW);
}

uint16_t SensorInterface::readSensor(uint32_t timeout){

  if (millis()-lastReadTime < 10) return detection;
  lastReadTime = millis();
  //uint32_t tt = getTimeout();
  setTimeout(timeout);
  detection = getHitTime()/29;

  //DEBUGLOG(detection);
  //DEBUGLOG(detection);
  //previous_page--;
  //if (detection != 0) personDetectedTime = millis();
  return detection;
}



boolean SensorInterface::isPagechangeDetected() {
  if (isPersonDetected() ) {
    if ((pageChangeDetectedTime>0) && (millis() - pageChangeDetectedTime < MAX_DURATION_PAGE_CHANGE  ) ) {
      return true;
    }
  }
  pageChangeDetectedTime = 0;
  return false;
  /*if (bForcePgeChangeDetected) {
    bForcePgeChangeDetected = false;
    return true;
  }
  if (isPersonDetected()) {
    //setTimeout(TIMEOUT_DETECTION_CLOSE);
    uint16_t distance = readSensor(TIMEOUT_DETECTION_CLOSE);
    DEBUGLOGF("dist page %d\n",distance);
    return  (distance>20 && distance<25);
  }
  return false;*/
}

boolean SensorInterface::isPersonDetected() {
  return personDetectedTime != 0;
}


boolean SensorInterface::isCfgDetected() {
  if (isPersonDetected() ) {
    if ((cfgDetectedTime>0) && (millis() - cfgDetectedTime < MAX_DURATION_CFG  ) ) {
      return true;
    }
  }
  return false;
}



void SensorInterface::checkPageChangeDetected() {
  if (bForcePgeChangeDetected) {
    bForcePgeChangeDetected = false;
    pageChangeDetectedTime = millis();
    return;
  }
  if (!isPersonDetected()) {
    return;
  }
    //setTimeout(TIMEOUT_DETECTION_CLOSE);
  uint16_t distance = readSensor(TIMEOUT_DETECTION_CLOSE);
  DEBUGLOGF("dist page %d\n",distance);
  if  (distance>20 && distance<25) {
    pageChangeDetectedTime = millis();
  } else if (distance>2 && distance<5) {
    cfgDetectedTime = millis();
  }
}

void SensorInterface::checkPersonDetected() {
  if ((personDetectedTime > 0 ) && ((millis() - personDetectedTime) < MAX_DURATION_ACTIVE) )
    return
  //setTimeout(TIMEOUT_DETECTION_FAR);
  pinMode(A0,INPUT_PULLDOWN_16);
  digitalWrite(A0,LOW);
  uint16_t distance = analogRead(A0);//readSensor(TIMEOUT_DETECTION_FAR);
  //DEBUGLOGF("%d : Analog Read %d\n",nbDetected,distance);
  if (distance>500) {
    nbDetected++;
    if (nbDetected==2) {
      nbDetected = 0;
      personDetectedTime =   millis();
      return ;
    }
  } else {
    nbDetected = 0;
  }
  personDetectedTime = 0;
  return ;

}
