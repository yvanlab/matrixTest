

#include "MatrixPages.h"
#include "myTimer.h"
#include "context.h"
/*#include "font/font8x8_basic.h"
#include "font/font8x16_basic.h"
#include "font/font4x6_basic.h"*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


MatrixPages::MatrixPages(Periferic *_periferic,unsigned char pinLed) : BaseManager(pinLed){
  //matrix = _matrix;
  periferic = _periferic;
  setPage(MESSAGE_PAGE);
}


void MatrixPages::displayPage(){
  if (page == MESSAGE_PAGE ) {
    displayMessagePage();
  } else if (page == CURRENT_PAGE) {
    displayCurrentPage();
  } else if (page == METEO_PAGE) {
    displayMeteoPage();
  } else if (page == VMC_PAGE) {
    displayVMCPage();
  } else if (page == CFG_PAGE) {
    displayCfgPage();
  } if (page == HOUR_PAGE) {
    displayHourPage();
  } else if (page == STOP_PAGE) {
    displayStopPage();
  }
}


void  MatrixPages::setPage(uint8 num) {
  //DEBUGLOGF("Page %d",num);
  if (num==page) return;
  matrix.clear();
  startTransition = 128;
  page=num;
};

void MatrixPages::nextPage() {
  if (startTransition != 0 /*|| page == CFG_PAGE*/) return;
  //DEBUGLOGF("NPage %d",page);
  uint8_t newpage = (page+1) % 5;
  setPage(newpage);
}

boolean MatrixPages::manageTransition(byte transitionType) {
  if (startTransition == 0) return false;
  if (!mtTimer.isQuickPeriod()) return false;
  startTransition--;
  if (startTransition==0) {
    matrix.clear();
    return true;
  }

  if (transitionType == FROM_CENTER) {
    matrix.setDisplayWindows(startTransition/2, startTransition/8,128-startTransition, 16-startTransition/8);
    matrix.drawSquare(0,0,startTransition/8-1,16-startTransition/8-1,0);
  }else if (transitionType == FROM_RIGHT) {
    matrix.setDisplayWindows(startTransition, 0,128, 16);
  }else   if (transitionType == FROM_LEFT) {
    matrix.setDisplayWindows(startTransition, 0,128, 16);
  }
}


void MatrixPages::displayMessagePage(){
  if (manageTransition(FROM_CENTER) || mtTimer.is1SPeriod()) {
     matrix.drawText(0,0,wfManager.getHourManager()->toDTString(STD_TEXT).c_str(),font4_6, PC_BRIGHT);
     String stemp = String(periferic->extTemp_meteoVmc,1) + "d-" + String (periferic->instant_current/1000,1) + "a";
     matrix.drawText(84,0,stemp.c_str(),font4_6, PC_BRIGHT);

   }
 if (mtTimer.is25MSPeriod()) {
   /*char buffer[50];
   strcpy_P(buffer, (char*)pgm_read_word(&(weatherString[periferic->trendMeteo_meteoVmc])));
   //strcpy(buffer, (char*)pgm_read_word(&(weatherString[periferic->trendMeteo_meteoVmc])));*/

   String weather =  String(periferic->extTemp_meteoVmc,1) + "C - "+ FPSTR (weatherString[periferic->trendMeteo_meteoVmc]);
   //DEBUGLOGF("weather : %s\n",weather.c_str());
   String messageToDisplay = String(smManager.m_textToDisplay) + " - " + weather;
   //DEBUGLOGF("messageToDisplay : %s\n",messageToDisplay.c_str());
   xDecalage--;
   if (xDecalage < (-8*(int)messageToDisplay.length()))
      xDecalage = 128;
   matrix.drawText(xDecalage,6,messageToDisplay.c_str(),font8_8, PC_BRIGHT);
 }
}


void MatrixPages::displayHourPage(){
  if (manageTransition(FROM_RIGHT) || mtTimer.is1SPeriod()) {
     matrix.drawText(0,0,wfManager.getHourManager()->toDTString(STD_TEXT).c_str(),font8_16, PC_BRIGHT);
  }
}

// ************************************************
// ** Current : xxKxh
// ** xxA  xxKwh
// *************************************************

void MatrixPages::displayCurrentPage(){
  if (manageTransition(FROM_LEFT) || mtTimer.is1MNPeriod()) {
    String ss = "Current " + String (periferic->kwh_current,1) + "Kwh" ;
    matrix.drawText(0,0,ss.c_str(),font4_6, PC_BRIGHT);
    ss = String (periferic->instant_current/1000,1) + "A" + String (periferic->instant_kwh_current,1)+"Kwh" ;
    matrix.drawText(0,8,ss.c_str(), font8_8, PC_BRIGHT);
  }
}

void MatrixPages::displayVMCPage(){
  if (manageTransition(FROM_LEFT) || mtTimer.is1MNPeriod()) {
    String ss = "VMC " + String(periferic->vtsVMC_meteoVmc);
    matrix.drawText(0,0,ss.c_str(),font4_6, PC_BRIGHT);
    ss = String(periferic->tempVMC_meteoVmc,1) + "deg";
    matrix.drawText(0,8,ss.c_str(), font8_8, PC_BRIGHT);
  }
}

void MatrixPages::displayMeteoPage(){
  if (manageTransition(FROM_RIGHT) || mtTimer.is1MNPeriod()) {
    String ss = "METEO " + String(periferic->trendMeteo_meteoVmc);
    matrix.drawText(0,0,ss.c_str(),font4_6, PC_BRIGHT);
    ss = String(periferic->extTemp_meteoVmc,1) + "deg";
    matrix.drawText(0,6,ss.c_str(),font8_8, PC_BRIGHT);
  }
}

void MatrixPages::displayCfgPage(){
  if (manageTransition(FROM_RIGHT) || mtTimer.is25MSPeriod()) {
    siInterface.readSensor(TIMEOUT_DETECTION_FAR);
    matrix.drawText(0,0,"    ",font8_8, PC_BRIGHT);
    matrix.drawText(0,0,String(siInterface.detection).c_str(),font8_8, PC_BRIGHT);

    int32_t ss =   MAX_DURATION_CFG - (millis() - siInterface.cfgDetectedTime);
    if (ss>0) ss = ss/1000;
    else ss = 0;
    matrix.drawText(120,0,String(ss).c_str(),font4_6, PC_BRIGHT);

    matrix.drawText(0,10,"1s",font4_6, PC_BRIGHT + PA_BLINK_1S);
    matrix.drawText(9,10,"25ms",font4_6, PC_BRIGHT + PA_BLINK_25MS);
    matrix.drawText(26,10,"250ms",font4_6, PC_BRIGHT + PA_BLINK_250MS);

    //matrix.drawText(80,8,"INVERSE",font4_6, PC_BRIGHT + PA_INVERT);
    //matrix.drawImage(65,1,star,0);
    matrix.drawImage(68,1,hamburger,0);
    matrix.drawImage(84,0,yingyung,0);
    matrix.drawImage(101,0,cornes,0);

    matrix.drawChar(30,0,0xDB,font4_6, PC_BRIGHT);
    matrix.drawChar(34,0,0xDB,font4_6, PC_SHADOW);
    matrix.drawChar(38,0,0xDB,font4_6, PC_DARK);

    if (siInterface.isPersonDetected()) {
      matrix.drawText(47,10,"ON",font4_6, PC_BRIGHT);
    } else {
      matrix.drawText(47,10,"Off",font4_6, PC_BRIGHT);
    }
    /*matrix.drawChar(42,0,0xDB,font4_6, PC_ON_3);*/

    /*matrix.drawText(30,0,"COLOR1",font4_6, PC_BRIGHT);
    matrix.drawText(62,0,"COLOR2",font4_6, PC_ON_1);
    matrix.drawText(92,0,"COLOR3",font4_6, PC_ON_2);*/

  }
}

void MatrixPages::displayStopPage(){
  if (manageTransition(FROM_RIGHT) || mtTimer.is25MSPeriod()) {
    siInterface.readSensor(TIMEOUT_DETECTION_FAR);
    //matrix.clear();
    matrix.drawText(0,0,"STUP",font8_16, false);
    //matrix.drawText(40,0,String(millis() - siInterface.cfgDetectedTime).c_str(),font8_16, false);
  }
}
