
#include <Arduino.h>
#ifdef esp32dev
#include <wifi.h>
#else
#include <ESP8266WebServer.h>

#endif
//#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
#include <esp8266httpclient.h>
//#include "ElectricManager.h"
#include <HourManager.h>

//#include "SparkfunManager.h"
#include "WifiManager.h"
#include "SettingManager.h"
//#include "ioManager.h"
#include "MatrixPages.h"
#include "periferic.h"
#include "SensorInterface.h"

#include "myTimer.h"

#ifdef MCPOC_TELNET
#include "RemoteDebug.h"
#endif


#define MQTT_DEBUG
#define MQTT_ERROR
//#include <Adafruit_MQTT.h>                                 // Adafruit MQTT library
//#include <Adafruit_MQTT_Client.h>                           // Adafruit MQTT library
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

/*extern "C" {
#include "user_interface.h"
}*/


  #define LOG_LABEL  "log"
  /*#define HUMIDITE_VMC_LABEL  "vmcHUM"
  #define TEMPERATURE_VMC_LABEL "vmcTEMP"
  #define VITESSE_VMC_LABEL "vmcVTS"
  #define TEMPERATURE_EXT_LABEL "extTEMP"
  #define TEMPERATUR_EXT_DHT_LABEL "extdhtTEMP"
  #define PRESSION_EXT_LABEL "extPRESS"
  #define HUMIDITE_EXT_LABEL  "extHUM"*/
  #define SWITCHED_ON_LABEL  "switchedOn"




//#endif



LEDMatrix matrix(PIN_A, PIN_B, PIN_C, PIN_D, PIN_OE, PIN_R1, /*PIN_G1, PIN_B1,*/ PIN_STB, PIN_CLK);
uint8_t displaybuf[WIDTH * HEIGHT]; // Display Buffer

ESP8266WebServer        server ( 80 );
ESP8266HTTPUpdateServer httpUpdater;
SettingManager          smManager(pinLed);
HourManager             hrManager(2390,pinLed);
WifiManager             wfManager(pinLed);
//ioManager               sfManager(pinLed);
Periferic               pPeriferic(pinLed);
SensorInterface         siInterface(pinLed, pin_SR04_TRG, pin_SR04_ECHO);//23200); // ST_HW_HC_SR04(TRIG, ECHO)

MatrixPages             mpPages(&pPeriferic,pinLed);
#ifdef MCPOC_TELNET
RemoteDebug             Debug;
#endif

// start of timerCallback

void restartESP() {
  ESP.restart();
}

void timerrestartESP(void *pArg) {
    restartESP();
}

String getJson()
{
  String tt("{\"module\":{");
    tt += "\"nom\":\"" + String(MODULE_NAME) +"\"," ;
    tt += "\"version\":\"" + String(VMC_VERSION) +"\"," ;
    tt += "\"uptime\":\"" + NTP.getUptimeString() +"\"," ;
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"datetime\":{" + hrManager.toDTString(JSON_TEXT) + "},";

    tt += "\"LOG\":[]";
    tt += "}},";
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
  server.send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}
void dataSummaryPage() {
	digitalWrite ( pinLed, LOW );
  server.send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}

void displayCredentialCollection() {
	digitalWrite ( pinLed, LOW );

  char temp[400];

  String message =  "<html>\
    <head>\
      <title>Credentials page</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>";
  message += "<p>";
  message +="<ul>";
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      message += "<li>";
      message +=i + 1;
      message += ": ";
      message += WiFi.SSID(i);
      message += " (";
      message += WiFi.RSSI(i);
      message += ")";
      message += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      message += "</li>";
    }
  message += "</ul>";
  message += "<form method='get' action='set'>";
  message += "<label>SSID:</label><input name='ssid' test length=32 value=\""+String(smManager.m_ssid) +"\"><br>";
  message += "<label>Pass:</label><input name='pass' length=64 value=\""+String(HIDDEN_KEY) +"\"><br>";
  message += "<label>PrivateKey:</label><input name='sparkPrivate' length=64 value=\""+String(HIDDEN_KEY) +"\"><br>";
  message += "<label>PublicKey:</label><input name='sparkPublic' length=64 value=\""+String(smManager.m_publicKey) +"\"><br>";
  message += "<label>Text a afficher:</label><input name='texte' length=150 value=\""+String(smManager.m_textToDisplay) +"\"><br>";
  message += "<input type='submit'></form>";
  message += "</body></html>";

  server.send ( 200, "text/html", message );

  digitalWrite ( pinLed, HIGH );

}

void setCredential(){
  String str = server.arg("ssid");
  if (str.length()>0)
    strcpy(smManager.m_ssid, str.c_str());
  str = server.arg("pass");
  if (str.length()>0 && str != HIDDEN_KEY)
    strcpy(smManager.m_password,str.c_str());
  str = server.arg("sparkPrivate");
  if (str.length()>0 && str != HIDDEN_KEY)
      strcpy(smManager.m_privateKey,str.c_str());
  str = server.arg("sparkPublic");
  if (str.length()>0)
      strcpy(smManager.m_publicKey,str.c_str());
  str = server.arg("texte");
  if (str.length()>0)
    strcpy(smManager.m_textToDisplay, str.c_str());
  smManager.writeData();
  server.send ( 200, "text/html", "data recorded.restart board");
}

void clearMemory(){
  smManager.clearData();
  server.send ( 200, "text/html", "ok");
}


void startWiFiserver() {
  if (wfManager.connectSSID(smManager.m_ssid,smManager.m_password,IPAddress(MODULE_IP), MODULE_MDNS )==WL_CONNECTED) {
    DEBUGLOG("connected");
    server.on ( "/", dataSummaryPage );
    server.onNotFound ( dataSummaryPage );
    hrManager.begin("pool.ntp.org", 1, true);
  } else {
    DEBUGLOG("Not connected");
    /*matrix.drawText(0,0,"No internet",font4_6, false);
    matrix.drawText(0,7,"192.0.4.1",font4_6, false);*/
    wfManager.connectAP(MODULE_MDNS_AP);
    server.on ( "/", displayCredentialCollection );
    server.onNotFound ( displayCredentialCollection );
  }
  WiFi.hostname(MODULE_NAME);
  server.on ( "/clear", clearMemory );
  server.on ( "/restart", restartESP );
  server.on ( "/set", setCredential );
  server.on ( "/credential", displayCredentialCollection );
  server.on ( "/status", dataSummaryJson );
  httpUpdater.setup(&server, ((const char *)"/firmware"), MODULE_UPDATE_LOGIN, MODULE_UPDATE_PASS);

  server.begin();
  #ifdef MCPOC_TELNET
  MDNS.addService("telnet", "tcp", 23); // Telnet server RemoteDebug
  #endif

  Serial.println( "HTTP server started" );
  Serial.println(wfManager.toString(STD_TEXT));
}

void setup ( void ) {
  // Iniialise input
  Serial.begin ( 115200 );
  //pinMode(pin_DETECTION,INPUT);
  smManager.readData();
  matrix.begin(displaybuf, WIDTH, HEIGHT);
  //DEBUGLOG("");DEBUGLOG(smManager.toString(STD_TEXT));
  startWiFiserver();

#ifdef MCPOC_TELNET
  Debug.begin(MODULE_NAME);
#endif

  mtTimer.begin(timerFrequence);
  mtTimer.setCustomMS(10000);
  pPeriferic.retrievePeriphericInfo();



  /*uint32_t realSize = ESP.getFlashChipRealSize();
      uint32_t ideSize = ESP.getFlashChipSize();
      FlashMode_t ideMode = ESP.getFlashChipMode();

      Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
      Serial.printf("Flash real size: %u\n\n", realSize);

      Serial.printf("Flash ide  size: %u\n", ideSize);
      Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
      Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

      if(ideSize != realSize) {
          Serial.println("Flash Chip configuration wrong!\n");
      } else {
          Serial.println("Flash Chip configuration ok.\n");
      }
*/

}


void loop ( void ) {

	server.handleClient();
  #ifdef MCPOC_TELNET
  Debug.handle();
  #endif
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
  }*/


  if (mtTimer.is1MNPeriod()){
    //DEBUGLOG(millis()-startTransition);
    pPeriferic.retrievePeriphericInfo();
    //mpPages.nextPage();
  }

  if (mtTimer.is25MSPeriod()) {
    if (siInterface.isCfgDetected()) {
      mpPages.setPage(CFG_PAGE);
      DEBUGLOG("cfg");
    }else if (siInterface.isPagechangeDetected() ){
      DEBUGLOG("next");
      mpPages.nextPage();
      DEBUGLOGF("next : %d\n",mpPages.page );
    } else if (siInterface.isPersonDetected() ){
        matrix.switchScreenStatus(SWITCH_ON_SCREEN);
    } else {
      DEBUGLOG("nobody detected");
      matrix.switchScreenStatus(SWITCH_OFF_SCREEN);
      mpPages.setPage(MESSAGE_PAGE);
    }
  }
  if (matrix.isScreenActivated())
    mpPages.displayPage();

  if (mtTimer.is5MNPeriod()) {
    if (!WiFi.isConnected()) {
      restartESP();
    }
    mpPages.setPage(MESSAGE_PAGE);
  }

  mtTimer.clearPeriod();
}
