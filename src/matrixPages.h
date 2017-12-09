

#ifndef MatrixPages_h
#define MatrixPages_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif



#include "BaseManager.h"
#include "Periferic.h"

#define FROM_CENTER 1
#define FROM_LEFT   2
#define FROM_RIGHT  3


#define MESSAGE_PAGE 0
#define HOUR_PAGE    1
#define CURRENT_PAGE 2
#define VMC_PAGE     3
#define METEO_PAGE   4
#define STOP_PAGE    5
#define CFG_PAGE     6



class MatrixPages : public BaseManager
{
  public:
    MatrixPages(Periferic *_periferic,unsigned char pinLed);
    void displayPage();
    void nextPage();
    void setPage(uint8 num);
    boolean isTransition() {return startTransition!=0;};

    void displayMessagePage();
    void displayCurrentPage();
    void displayVMCPage();
    void displayMeteoPage();
    void displayCfgPage();
    void displayStopPage();
    void displayHourPage();
    String getClassName(){return "MatrixPages";}

    uint8_t page = 0;

  private:
    boolean manageTransition(byte transitionType);
    int16_t xDecalage = 128;
    uint8_t previous_detection = 0;
    int16_t startTransition=0;
    Periferic *periferic;

};


#endif
