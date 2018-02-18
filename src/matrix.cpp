
#include <Arduino.h>

#include "WifiManagerV2.h"
#include "SettingManager.h"
#include "thingSpeakManager.h"
#include "MatrixPages.h"
#include "periferic.h"
#include "SensorInterface.h"

#include "myTimer.h"

#ifdef MCPOC_TELNET
#include "RemoteDebug.h"
#endif


#include <WiFiClient.h>
#include "LEDMatrix.h"

#include "context.h"

#include "font/font8x8_basic.h"
#include "font/font8x16_basic.h"
#include "font/font4x6_basic.h"

#include "icon/yingyung.h"

#define MODULE_NAME MATRIX_DISPLAY_NAME
#define MODULE_MDNS MATRIX_DISPLAY_MDNS
#define MODULE_MDNS_AP MATRIX_DISPLAY_MDNS_AP
#define MODULE_IP   MATRIX_DISPLAY_IP

#define  pinLed           D4 //D4
#define  pin_DETECTION    D7
#define  pin_SR04_TRG     D7
#define  pin_SR04_ECHO    D6

//#ifdef NODEMCU // for nodeMCU
  #include <pins_arduino.h>
  #define PIN_A D0
  #define PIN_B D1
  #define PIN_C D2
  #define PIN_D D3
  #define PIN_OE D4
  #define PIN_R1 D5
  #define PIN_G1 D6
  #define PIN_B1 D7
  #define PIN_STB D8
  #define PIN_CLK D9
//#endif
#define WIDTH   128
#define HEIGHT  16


#define PRESENCE_LABEL 1 //"current"
#define KWH_LABEL     2 //"KWH"
#define HUM_LABEL     3 //"HUM"
#define TEMP_LABEL    4 //"TEMP"




//#endif



LEDMatrix matrix(PIN_A, PIN_B, PIN_C, PIN_D, PIN_OE, PIN_R1, /*PIN_G1, PIN_B1,*/ PIN_STB, PIN_CLK);
uint8_t displaybuf[WIDTH * HEIGHT]; // Display Buffer

SettingManager          smManager(pinLed);
WifiManager             wfManager(pinLed,&smManager);
Periferic               pPeriferic(pinLed);
SensorInterface         siInterface(pinLed, pin_SR04_TRG, pin_SR04_ECHO);//23200); // ST_HW_HC_SR04(TRIG, ECHO)
thingSpeakManager       sfManager(pinLed);
MatrixPages             mpPages(&pPeriferic,pinLed);

#ifdef MCPOC_TELNET
RemoteDebug             Debug;
#endif

#ifdef MCPOC_TELNET // Not in PRODUCTION
void processCmdRemoteDebug() {
    String lastCmd = Debug.getLastCommand();
    if (lastCmd == "next") {
      siInterface.setPagechangeDetected();
    } else if (lastCmd == "on") {
      siInterface.setPersonDetected(true);
    } else if (lastCmd == "off") {
      siInterface.setPersonDetected(false);
    } else if (lastCmd == "cfgon") {
      siInterface.setCfgDetected(true);
    } else if (lastCmd == "cfgoff") {
      siInterface.setCfgDetected(false);
    } else if (lastCmd == "perif") {
      pPeriferic.retrievePeriphericInfo();
    }



}
#endif

void timerrestartESP(void *pArg) {
    ESP.restart();
}

String getJson()
{

  String tt("{\"module\":{");
    tt += "\"nom\":\"" + String(MODULE_NAME) +"\"," ;
    tt += "\"version\":\"" + String(MATRIX_VERSION) +"\"," ;
    tt += "\"uptime\":\"" + NTP.getUptimeString() +"\"," ;
    tt += "\"status\":\"" + String(STATUS_PERIPHERIC_WORKING) +"\"," ;
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"datetime\":{" + wfManager.getHourManager()->toDTString(JSON_TEXT) + "},";
    tt += "\"setting\":{";
    tt += "\"screenActivated\":\""+ String(matrix.isScreenActivated()) +"\"," ;
    tt += smManager.toString(JSON_TEXT) + "},";
    tt += "\"periferic\":[" /*+pPeriferic.getsaintJson() + ","*/+  pPeriferic.getcurrentJson() + "," + pPeriferic.getSnorerVMCJson() + "," + pPeriferic.getMeteoVMCJson() + "]";

    //  tt += "\"LOG\":[]";
    tt += "}";
    return tt;
}

/*
{
  "module" : {
    "nom":"blabla",
    "version" : "1.0.2",
    "date":"10/08/2017 18:15"
  },
"log" : [
    {
    "nom" : "class name",
    "code" : "10",
    "description" : "blabla"
    },
    {
    "nom" : "class name",
    "code" : "10",
    "description" : "blabla"
    }
  ],
  "VMC" : {
    "dhtTemp" : "10.25",
    "dhtHum"  : "40.25",
    "Vitesse" : "0"
  },
  "EXT" : {
    "bmpTemp" : "10.25",
    "bmpPress"  : "40.25",
    "dhtHum" : "41.70",
    "dhtTemp" : "27.70"
  }
}

*/

void dataSummaryJson() {
	digitalWrite ( pinLed, LOW );
  wfManager.getServer()->send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}
void dataSummaryPage() {
	digitalWrite ( pinLed, LOW );
  wfManager.getServer()->send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}


void forcePerifericUpdate() {
  pPeriferic.retrievePeriphericInfo();
  wfManager.getServer()->send ( 200, "text/html", "Ok");
}



void setData(){
  String str = wfManager.getServer()->arg("texte");
  if (str.length()>0)
    strcpy(smManager.m_textToDisplay, str.c_str());
  smManager.writeData();
  siInterface.setPersonDetected(true);
  wfManager.getServer()->send ( 200, "text/html", "Ok");
}


void displayData() {
	digitalWrite ( pinLed, LOW );

  char temp[400];

  String message =  "<html>\
    <head>\
      <title>message page</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>";
  message += "<p>";
  message +="<ul>";
  message += "</ul>";
  message += "<form method='get' action='setData'>";
  message += "<label>Text a afficher:</label><input name='texte' length=300 value=\""+String(smManager.m_textToDisplay) +"\"><br>";
  message += "<input type='submit'></form>";
  message += "</body></html>";

  wfManager.getServer()->send ( 200, "text/html", message );

  digitalWrite ( pinLed, HIGH );

}

void startWiFiserver() {
  if (wfManager.begin(IPAddress(MODULE_IP),MODULE_NAME, MODULE_MDNS, MODULE_MDNS_AP)==WL_CONNECTED) {
    wfManager.getServer()->on ( "/", dataSummaryPage );
    wfManager.getServer()->onNotFound ( dataSummaryPage );
  } /*else {
    DEBUGLOG("Not connected");
  }*/

  wfManager.getServer()->on ( "/setting", displayData );
  wfManager.getServer()->on ( "/setData", setData );
  wfManager.getServer()->on ( "/status", dataSummaryJson );
  wfManager.getServer()->on ( "/force", forcePerifericUpdate );

  Serial.println( "HTTP server started" );
  Serial.println(wfManager.toString(STD_TEXT));
}



void setup ( void ) {
  // Iniialise input
  Serial.begin ( 115200 );

  //pinMode(pin_DETECTION,INPUT);
  smManager.readData();
  matrix.begin(displaybuf, WIDTH, HEIGHT);
  DEBUGLOG("");DEBUGLOG(smManager.toString(STD_TEXT));
  startWiFiserver();



#ifdef MCPOC_TELNET
  Debug.begin(MODULE_NAME);
  String helpCmd = "next\n\ron/off\n\rcfgon/cfgoff\n\rperif\n\r";
  Debug.setHelpProjectsCmds(helpCmd);
  Debug.setCallBackProjectCmds(&processCmdRemoteDebug);

#endif

  mtTimer.begin(timerFrequence);
  mtTimer.setCustomMS(10000);
  pPeriferic.retrievePeriphericInfo();

}

boolean previousPresence = true;

void loop ( void ) {

	wfManager.handleClient();

  //matrix.displayScreen();
  /*if (mtTimer.is1SPeriod()) {
    matrix.drawImage(0,0,yingyung,0);
    matrix.drawImage(20,0,yingyung,PA_BLINK_MSEC);


    matrix.drawChar(30,0,0xDB,font4_6, PC_BRIGHT);
    matrix.drawChar(34,0,0xDB,font4_6, PC_SHADOW);
    matrix.drawChar(38,0,0xDB,font4_6, PC_DARK);
    //matrix.drawChar(42,0,0xDB,font4_6, PC_ON_3);
  }
  if (mtTimer.is1MNPeriod()) {
    if (!WiFi.isConnected()) {
      restartESP();
    }
  }
  if (mtTimer.isCustomPeriod()) {
    pPeriferic.retrievePeriphericInfo();
  }
  */




  if (mtTimer.is25MSPeriod()) {
    if (siInterface.isCfgDetected()) {
      mpPages.setPage(CFG_PAGE);
      //DEBUGLOG("cfg");
    }else if (siInterface.isPagechangeDetected() ){
      DEBUGLOG("next");
      mpPages.nextPage();
      DEBUGLOGF("next : %d\n",mpPages.page );
    } else if (siInterface.isPersonDetected() ){
        matrix.switchScreenStatus(SWITCH_ON_SCREEN);
    } else {
      //DEBUGLOG("nobody detected");
      matrix.switchScreenStatus(SWITCH_OFF_SCREEN);
      mpPages.setPage(MESSAGE_PAGE);
    }
  }
  if (matrix.isScreenActivated()) {
    mpPages.displayPage();
    if (mtTimer.is5MNPeriod()){
      pPeriferic.retrievePeriphericInfo();
    }
  } else {
    if (mtTimer.is1MNPeriod()){
      pPeriferic.retrievePeriphericInfo();
    }
  }

if (mtTimer.is1MNPeriod()){
  boolean nowDetection = siInterface.isPersonDetected();

  if (((!previousPresence && nowDetection) || (previousPresence && !nowDetection)) || mtTimer.is30MNPeriod()  ) {
    sfManager.addVariable(PRESENCE_LABEL, String (nowDetection));
    DEBUGLOG(sfManager.toString(STD_TEXT));
    int res = sfManager.sendIoT( smManager.m_privateKey, smManager.m_publicKey);
    DEBUGLOGF("sendIoT : %d\n",res );
    previousPresence = nowDetection;
  }
}

  if (mtTimer.is5MNPeriod()) {
    if (!WiFi.isConnected()) {
      ESP.restart();
    }
    mpPages.setPage(MESSAGE_PAGE);
  }

  mtTimer.clearPeriod();
}
