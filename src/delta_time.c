/* src/delta_time.c */
/* src/delta_time.c - Frame timing */
#include "engine.h"

void deltaTimeCalculate()
{
    int deltaTimeOld = deltaTime;

    const int time2 = time1;
    time1 = RTC_GetTicks();
    deltaTime = time1 - time2;
    deltaTime /= 128;

    if(deltaTime > 1)
    deltaTime = deltaTimeOld;

    deltaTimeNoSlow = deltaTime;
    if(slow == true)
    deltaTime /= 10;
}
