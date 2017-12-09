

#include "myTimer.h"
#include "context.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


MyTimer mtTimer(0);

MyTimer::MyTimer(unsigned char pinLed) : BaseManager(pinLed){

}

/*String MyTimer::toString(boolean bJson = false){
  return "Period[" + String(period) + "]";
}*/
void  timerCallback(void *pArg) {
     mtTimer.periodCPT++;

     //DEBUGLOG(mtTimer.periodCPT);
     mtTimer.period |= PERIOD_QUICK;
     if ((mtTimer.periodCPT % MOD_25MS) == 0)    {mtTimer.period |= PERIOD_25MS ;mtTimer.frequence ^= PERIOD_25MS;}
     if ((mtTimer.periodCPT % MOD_250MS) == 0)   {mtTimer.period |= PERIOD_250MS;mtTimer.frequence ^= PERIOD_250MS;}
     if ((mtTimer.periodCPT % MOD_1S) == 0)      {mtTimer.period |= PERIOD_1S   ;mtTimer.frequence ^= PERIOD_1S;}
     if ((mtTimer.periodCPT % MOD_1MN) == 0)     {mtTimer.period |= PERIOD_1MN  ;mtTimer.frequence ^= PERIOD_1MN;}
     if ((mtTimer.periodCPT % MOD_5MN) == 0)     {mtTimer.period |= PERIOD_5MN  ;mtTimer.frequence ^= PERIOD_5MN;}
     if ((mtTimer.periodCPT % MOD_30MN) == 0)    {mtTimer.period |= PERIOD_30MN ;mtTimer.frequence ^= PERIOD_30MN;}
     if ((mtTimer.periodCPT % mtTimer.MOD_custom) == 0)  {mtTimer.period |= PERIOD_CUSTOM;mtTimer.frequence ^= PERIOD_CUSTOM;}
     matrix.displayScreen();
} // End of timerCallback

void MyTimer::begin(uint32_t freqMS){
  os_timer_setfn(&myTimer,timerCallback, NULL);
  os_timer_arm(&myTimer, freqMS, true);
}
