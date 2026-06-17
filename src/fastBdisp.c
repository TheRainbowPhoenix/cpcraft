#include <sdk/os/lcd.h>
#include "fastBdisp.h"

// Hollyhock v3 uses a simple vram pointer and LCD_Refresh()
// We'll keep the function names for compatibility but use SDK calls.

void DmaWaitNext(void){
    // No-op in Hollyhock as LCD_Refresh is usually synchronous or handled by OS
}

void DoDMAlcdNonblockStrip(unsigned y1,unsigned y2){
    // In Hollyhock, we don't have easy partial refresh like this that maps to the old Prizm one
    // So we just refresh the whole screen for now.
    LCD_Refresh();
}

void DoDMAlcdNonblock(void){
    LCD_Refresh();
}

void DoDMAlcdNonblockStripXramStart(unsigned y1,unsigned y2)
{
    // No-op
}

void DoDMAlcdNonblockStripXram(unsigned y1,unsigned y2)
{
    // No-op or LCD_Refresh() if it's supposed to actually show something
    LCD_Refresh();
}
