

#ifndef myTimer_h
#define myTimer_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include "BaseManager.h"

extern "C" {
#include "user_interface.h"
}



#define PERIOD_QUICK  0b00000001
#define PERIOD_25MS   0b00100000
#define PERIOD_250MS  0b00000010
#define PERIOD_1S     0b00000100
#define PERIOD_1MN    0b00001000
#define PERIOD_5MN    0b00010000
#define PERIOD_30MN   0b01000000
#define PERIOD_CUSTOM 0b10000000


#define timerFrequence  (uint32_t)5 //60000;//ms
#define MOD_25MS (uint32_t)(25/timerFrequence)
#define MOD_250MS (uint32_t)(250/timerFrequence)
#define MOD_1S (uint32_t)(1000/timerFrequence)
#define MOD_1MN (uint32_t)((60*1000)/timerFrequence)
#define MOD_5MN (uint32_t)((5*60*1000)/timerFrequence)
#define MOD_30MN (uint32_t)((30*60*1000)/timerFrequence)



class MyTimer : public BaseManager
{
  public:
    MyTimer(unsigned char pinLed);
    void begin(uint32_t freqMS);

    //static void  timerCallback(void *pArg);

    boolean isQuickPeriod() {return period&PERIOD_QUICK;}
    boolean is25MSPeriod()  {return period&PERIOD_25MS;}
    boolean is250MSPeriod() {return period&PERIOD_250MS;}
    boolean is1SPeriod()    {return period&PERIOD_1S;}
    boolean is1MNPeriod()   {return period&PERIOD_1MN;}
    boolean is5MNPeriod()   {return period&PERIOD_5MN;}
    boolean is30MNPeriod()  {return period&PERIOD_30MN;}
    boolean isCustomPeriod(){return period&PERIOD_CUSTOM;}


    boolean isQuickFrequence()  {return frequence&PERIOD_QUICK;}
    boolean is25MSFrequence()   {return frequence&PERIOD_25MS;}
    boolean is250MSFrequence()  {return frequence&PERIOD_250MS;}
    boolean is1SFrequence()     {return frequence&PERIOD_1S;}
    boolean is1MNFrequence()    {return frequence&PERIOD_1MN;}
    boolean is5MNFrequence()    {return frequence&PERIOD_5MN;}
    boolean is30MNFrequence()   {return frequence&PERIOD_30MN;}
    boolean isCustomFrequence() {return frequence&PERIOD_CUSTOM;}

    void setCustomMS(uint32_t customMS) {MOD_custom = customMS/timerFrequence;};
    void clearPeriod(){period=0;}

    String getClassName(){return "MyTimer";}
    os_timer_t myTimer;
    uint8_t period = 0;
    uint8_t frequence = 0;
    uint32_t periodCPT = 0;
    uint32_t MOD_custom = 0;
  private:



};

extern MyTimer mtTimer;

#endif
