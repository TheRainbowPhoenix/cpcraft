#include <sdk/os/lcd.h>
#include "fastBdisp.h"

void DmaWaitNext(void){
}

void DoDMAlcdNonblockStrip(unsigned y1, unsigned y2){
    (void)y1; (void)y2;
    LCD_Refresh();
}

void DoDMAlcdNonblock(void){
    LCD_Refresh();
}

void DoDMAlcdNonblockStripXramStart(unsigned y1, unsigned y2) {
    (void)y1; (void)y2;
}

void DoDMAlcdNonblockStripXram(unsigned y1, unsigned y2) {
    (void)y1; (void)y2;
    LCD_Refresh();
}
