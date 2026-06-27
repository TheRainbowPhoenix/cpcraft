/* src/main.c - Entry point */
#include "engine.h"
#include "config.h"
#include "misc_data.h"

int main() 
{
    tlsf = tlsf_create_with_pool((char*)(0x8C400000 + ALLOC_OFFSET), 1024*1024);

    VRAMAddress = (unsigned short*)GetVRAMAddress();

    //ZBuffer = (unsigned short *)((uintptr_t)GetSecondaryVRAMAddress() & ~1);
    ZBuffer = 0x8C200000 + ALLOC_OFFSET;
	
	Bdisp_EnableColor(1);
	Bdisp_AllClr_VRAM();

	for (int i = 0; i < 384 * 216; i++)
    ZBuffer[i] = 65535;

    //int start = RTC_GetTicks();

    //for (int j = 0; j < 50000; j++)
    //renderTriangleTexturedNBPX2T((Vector2I){5, 5}, (Vector2I){55, 5}, (Vector2I){55, 55}, (Vector2I){0, 0}, (Vector2I){10, 0}, (Vector2I){10, 10}, 50000-j, 16, 16, &textures[0]);

    //int end = RTC_GetTicks();     
    //char txt[50];         
    //sprintf(txt, "%d", end-start);
    //renderTextUpdate(5, 60, 1000, txt);

    renderTextUpdate(10, resY-15, 200, "By 010ello010");

    for (int i = 0; i < partiLength; i++)
    allparticles[i].mode = false;

    for (int i = 0; i < resXZBuffer * resYZBuffer; i++)
    ZBuffer[i] = 65535;

    if(GetVRAMAddress() == 0xAC000000)
    {
        isOncCG50 = true;
        
        renderObject();
    }
    else
    {
        renderText(10, 10, "You are on fx-cg10/fx-cg20.");
        renderText(10, 25, "CG10/CG20 does not have phython heap and this version uses phython heap.");
        renderText(10, 40, "Please download the verison that was designed for your device");
        
        Bdisp_PutDisp_DD();
        OS_InnerWait_ms(2000);
    }
    //renderObject();

    return 0;
}

