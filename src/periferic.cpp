

#include "periferic.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


Periferic::Periferic(unsigned char pinLed) : BaseManager(pinLed){

}

int  Periferic::sendToVoiceBox(String text){
  IPAddress ip;
  WiFiUDP   Udp;
  ip.fromString(VOICE_IP_PROD);
  DEBUGLOGF("send to Voice:%s\n",text.c_str());
  Udp.beginPacket(ip, 4210);
  //Udp.beginPacket(IPAddress(192,168,0,145), 4210);
  uint8_t res = Udp.write(text.c_str());
  Udp.endPacket();
  return res;
}


String Periferic::collectJson(String urlService) {
  if (!WiFi.isConnected()) return "";
  HTTPClient http;  //Declare an object of class HTTPClient
  DEBUGLOG(urlService);  //Specify request destination
  http.begin(urlService);  //Specify request destination
  DEBUGLOG("http.begin");
  String result;
  int httpCode = http.GET();                                                                  //Send the request
  DEBUGLOGF("http.GET %d",httpCode);
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
  DEBUGLOG("Json:"+jSon + "  key:"+key);
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
  DEBUGLOG(key+":"+sTmp + "  value:"+sTmp);
  return sTmp;
}

String Periferic::getBlock(String jSon, String key) {
  int16_t fromIndex = jSon.indexOf(key);
  DEBUGLOGF("First index %d",fromIndex);
  if (fromIndex < 0) return "";
  int16_t firstparenthesis = jSon.indexOf('{', fromIndex);
  uint8_t nbrParenthese=0;
  String sTmp;
  for (int i=firstparenthesis+1;i++;i<jSon.length()) {
    if (jSon[i] == '{') nbrParenthese++;
    if (jSon[i] == '}') {
      if (nbrParenthese>0)
        nbrParenthese--;
      else {
        sTmp = jSon.substring(firstparenthesis, i+1);
        DEBUGLOG(key+":"+sTmp);
        return sTmp;
      }
    }
  }
/*
  if (firstparenthesis < 0) return "";
  int16_t lastparenthesis = jSon.indexOf('}', firstparenthesis);
  String sTmp = jSon.substring(firstparenthesis, lastparenthesis+1);
*/

  return sTmp;
}

String Periferic::getKeyValue(String jsonMessage, String key) {
  int8_t i = key.indexOf('/');
  //DEBUGLOG("Json:"+jsonMessage + "  key:"+key);
  if (i<0)
    return getValue(jsonMessage, key);
  String blockKey = key.substring(0, i);
  String nextKey = key.substring(i+1);
  DEBUGLOG("blockKey : "+blockKey+"   nextKey:" + nextKey);
  String block = getBlock(jsonMessage, blockKey);
  if (block != "")
    return getKeyValue(block, nextKey);
  return "";
}

String Periferic::getVmcMetoValue(String key) {
  return getKeyValue(m_meteoVMCJson, key);

}
String Periferic::getCurrentValue(String key){
  return getKeyValue(m_currentJson, key);

}
String Periferic::getSonorerValue(String key){
  return getKeyValue(m_snorerJson, key);

}




String Periferic::buildPerifericOutput(String perifericName, uint8_t statusCode) {
  String tt("{\"module\":{");
    tt += "\"nom\":\"" + perifericName +"\"," ;
    tt += "\"status\":\"" + String(statusCode) + "\"}}";
  return tt;
}


void Periferic::retrievePeriphericInfo() {
  //String block;


  //collect saint du Jour
/*  if (m_saintJson=="" ||
      getKeyValue(m_saintJson,"day").toInt() != day() ||
      getKeyValue(m_saintJson,"month").toInt() != month() ) {
      m_saintJson = collectJson("http://fetedujour.fr/api/v2/VOTRECLE/json-normal");
  }*/



  m_snorerJson = "";//collectJson("http://"+String(SNORER_IP_PROD)+"/status");
  if (m_snorerJson.length() == 0) {
    m_snorerJson = buildPerifericOutput(SNORER_NAME, STATUS_PERIPHERIC_DOWN);
  }

  m_currentJson = collectJson("http://"+String(CURRENT_IP_PROD)+"/status");
  //status_current = STATUS_PERIPHERIC_WORKING;
  if (m_currentJson.length() == 0) {
    //m_currentJson = "{\"current-status\":\"failed\"}";
    m_currentJson = buildPerifericOutput(CURRENT_NAME, STATUS_PERIPHERIC_DOWN);
    //status_current = STATUS_PERIPHERIC_DOWN;
  } else {
    float val1 = getKeyValue(m_currentJson,"courant1/value").toFloat();
    m_InstantCurrent1 = String(val1/1000.0,1);
    float val2 = getKeyValue(m_currentJson,"courant2/value").toFloat();
    m_Differential    = String(abs((val1-val2)),0);
    m_InstantCurrent2 = String(val2/1000.0,1);
    m_Temperarture    = getKeyValue(m_currentJson,"temperature/value");
    val1 = getKeyValue(m_currentJson,"courant3/value").toFloat();
    m_InstantCurrent3 = String(val1/1000.0,1);
    m_KWH1 = "";
  }

  m_meteoVMCJson = collectJson("http://"+String(VMC_METEO_IP_PROD)+"/status");
  //status_meteoVmc = STATUS_PERIPHERIC_WORKING;
  if (m_meteoVMCJson.length() == 0) {
    //status_meteoVmc = STATUS_PERIPHERIC_DOWN;
    m_meteoVMCJson = buildPerifericOutput(VMC_METEO_NAME, STATUS_PERIPHERIC_DOWN);
  } else {
    m_ExtTemp       = getKeyValue(m_meteoVMCJson,"EXT/Btemperature/value");
    m_ExtTempMax    = getKeyValue(m_meteoVMCJson,"EXT/Btemperature/maxValue");
    m_ExtTempMin    = getKeyValue(m_meteoVMCJson,"EXT/Btemperature/minValue");
    m_ExtTrendMeteo = getKeyValue(m_meteoVMCJson,"EXT/pression/forcast").toInt();
    m_VMVvts        = getKeyValue(m_meteoVMCJson,"VMC/Vitesse");
    m_VMCtemp       = getKeyValue(m_meteoVMCJson,"VMC/temperature/value");
  }
}
