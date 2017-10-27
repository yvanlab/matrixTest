

#include "periferic.h"
#include <esp8266httpclient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


Periferic::Periferic(unsigned char pinLed) : BaseManager(pinLed){

}

String Periferic::collectJson(String ipAddress) {
    if (!WiFi.isConnected()) return "";
    HTTPClient http;  //Declare an object of class HTTPClient
    DEBUGLOG("http://"+ipAddress+"/status");  //Specify request destination
    http.begin("http://"+ipAddress+"/status");  //Specify request destination
    String result;
    int httpCode = http.GET();                                                                  //Send the request

    if (httpCode > 0) { //Check the returning code
      result = http.getString();   //Get the request response payload
    }
    http.end();   //Close connection
    //DEBUGLOG(httpCode);DEBUGLOG(result);
    return result;
}
/*
{
  "module": {
    "nom": "current",
    "version": "1.02",
    "uptime": "7 days 13:58:51",
    "build_date": "Aug 19 2017 17:34:27"
  },
  "LOG": [
    {
      "nom": "WifiManager",
      "code": "3",
      "description": "ssid"
    }],
  "current": {
        "current": "4740",
        "KWH": "1501.63",
        "lastKWT": "19.28"
    }
}
*/
String Periferic::getValue(String jSon, String key) {
  int16_t fromIndex = jSon.indexOf(key);
  //DEBUGLOGF("First index %d",fromIndex);
  if (fromIndex < 0) return "";
  int16_t lastIndex = jSon.indexOf(',', fromIndex);
  if (lastIndex < 0) lastIndex = jSon.indexOf('}', fromIndex);
  if (lastIndex < 0) return "";
  //DEBUGLOGF("second index %d",lastIndex);
  String sTmp = jSon.substring(fromIndex, lastIndex);
  //DEBUGLOG(sTmp);
  //retrive ""
  lastIndex = sTmp.lastIndexOf("\"");
  fromIndex = sTmp.indexOf("\"",key.length()+1);
  sTmp = sTmp.substring(fromIndex+1,lastIndex );
  //DEBUGLOG(key+":"+sTmp);
  return sTmp;
}

String Periferic::getBlock(String jSon, String key) {
  int16_t fromIndex = jSon.indexOf(key);
  //DEBUGLOGF("First index %d",fromIndex);
  if (fromIndex < 0) return "";
  int16_t firstparenthesis = jSon.indexOf('{', fromIndex);
  if (firstparenthesis < 0) return "";
  int16_t lastparenthesis = jSon.indexOf('}', firstparenthesis);
  String sTmp = jSon.substring(firstparenthesis, lastparenthesis+1);
  //DEBUGLOG(key+":"+sTmp);
  return sTmp;
}

void Periferic::retrievePeriphericInfo() {
  String block;
  String result = collectJson(CURRENT_IP_PROD);

  status_current = STATUS_PERIPHERIC_WORKING;
  if (result.length() == 0) {
    status_current = STATUS_PERIPHERIC_DOWN;
  } else {
    block = getBlock(result,"courant");
    instant_current       = getValue(block,"current").toFloat();
    kwh_current           = getValue(block,"lastKWT").toFloat();
    instant_kwh_current   = getValue(block,"KWH").toFloat();
    //lastMesureDate_current = getValue(block,"KWH");
  }

  result = collectJson(VMC_METEO_IP_PROD);
  status_meteoVmc = STATUS_PERIPHERIC_WORKING;
  if (result.length() == 0) {
    status_meteoVmc = STATUS_PERIPHERIC_DOWN;
  } else {
    block = getBlock(result,"VMC");
    vtsVMC_meteoVmc     = getValue(block,"Vitesse").toInt(); //00,50,100
    tempVMC_meteoVmc    =  getValue(block,"dhtTemp").toFloat();

    block = getBlock(result,"EXT");
    extTemp_meteoVmc    = getValue(block,"bmpTemp").toFloat();
    trendMeteo_meteoVmc = getValue(block,"bmpPress").toInt();
    //lastMesureDate__meteoVmc = getValue(result,"KWH");
  }
}
