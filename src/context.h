

#ifndef context_h
#define context_h

#include "Arduino.h"

#include "BaseManager.h"
#include "LEDMatrix.h"
#include <HourManager.h>
#include <ST_HW_HC_SR04.h>
//#include "SparkfunManager.h"
#include "WifiManager.h"
#include "SettingManager.h"
//#include "ioManager.h"
#include "periferic.h"

#include "myTimer.h"
#include "sensorInterface.h"




extern MyTimer         mtTimer;
extern SettingManager  smManager;
extern HourManager     hrManager;
extern WifiManager     wfManager;
//extern ioManager       sfManager;
extern LEDMatrix       matrix;
extern Periferic       pPeriferic;
extern SensorInterface         siInterface;

extern MyFONT          font4_6;
extern MyFONT          font8_8;
extern MyFONT          font8_16;

extern MyICON         yingyung;
extern MyICON         cornes;
extern MyICON         hamburger;
extern MyICON         star;

/*#ifdef MCPOC_TELNET
extern RemoteDebug    Debug;
#endif*/


#endif
