/* src/render/text.c - Text rendering functions */
#include "engine.h"


void renderText(int x, int y, const char *text)
{
    int x2 = x;
    int y2 = y;
    PrintMiniMini(&x2, &y2, text, (1 << 6) | (1 << 1), 0x0000, 0);
}
void renderTextColord(int x, int y, const char *text, color_t color)
{
    int x2 = x;
    int y2 = y;
    PrintMiniMini(&x2, &y2, text, (1 << 6) | (1 << 1), color, 0);
}
void renderTextLargeColord(int x, int y, const char *text, color_t color, color_t backGroundColor)
{
    int x2 = x;
    int y2 = y;
    PrintMini(&x2, &y2, text, 0x40, 0xFFFFFFFF, 0, 0, color, backGroundColor, 1, 0);
}
void renderTextUpdate(int x, int y, int stop, const char *text)
{
	int x2 = x;
    int y2 = y;
    PrintMiniMini(&x2, &y2, text, (1 << 6) | (1 << 1), 0x0000, 0);
	Bdisp_PutDisp_DD();
	if(stop != 0)
	OS_InnerWait_ms(stop);
}

//particle rendering
