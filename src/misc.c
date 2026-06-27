/* src/misc.c */
/* src/misc.c - Performance log, map view, entity reach */
#include "engine.h"
#include "config.h"
#include "chunk_constants.h"


void updatePerformanceLog()
{
    performanceTime += deltaTimeNoSlow;

    if(performanceTime > 2)
    {
        for (int i = 0; i < 10; i++)
        {
            Cperformace[i] = performace[i]/2;
            performace[i] = 0;
        }

        performanceTime = 0;
    }

    int totalTicks = 0;
    
    for (int i = 0; i < 10; i++)
    {
        for (int x = totalTicks; x < totalTicks+Cperformace[i]; x++)
        {
            for (int y = 0; y < 8; y++)
            VRAMAddress[y * 384 + x] = performanceColors[i];
        }

        for (int y = 10; y < 18; y++)
        {
            for (int x = 0; x < 8; x++)
            VRAMAddress[(y + i*10) * 384 + x] = performanceColors[i];
        }

        int textLength = 0;
        for (int j = 0; j < 35; j++)
        {
            if(performanceName[i][j] != 0)
            textLength++;
        }

        drawMinecraftFont(10, 10, i*10+10, false, textLength, performanceName[i], 0xdefb, 0x39c7);

        char text_[10];
        sprintf(text_, "%d", Cperformace[i]);
        drawMinecraftFont(108, 108, i*10+10, false, countDigits(Cperformace[i]), text_, 0xdefb, 0x39c7);

        totalTicks += Cperformace[i];
    }
}

void viewMap()
{
    color_t grassC = 0x4404;
    color_t sandC = 0xd653;
    color_t waterC = 0x42f4;

    color_t *screenLoc = VRAMAddress;

    for (int x = 0; x < 384; x++)
    {
        for (int z = 0; z < 216; z++)
        {
            int perlinHeight = Perlin3D(x * 10, 0, z * 10);

            int r5 = (perlinHeight * 31) / 1000;
            int g6 = (perlinHeight * 63) / 1000;
            int b5 = (perlinHeight * 31) / 1000;

            VRAMAddress[x + z*384] = (r5 << 11) | (g6 << 5) | b5;
            //screenLoc++;
        }
    }
    Bdisp_PutDisp_DD();
    OS_InnerWait_ms(2000);
}

//update Entitys
void updateEntityReach()
{
    Vector3I forw = {forward.x * 100, -forward.y * 100, -forward.z * 100};
    Vector3I pPos = {PPosX * 1000, PPosY * 1000, PPosZ * 1000};

    Vector3I lastPointRounded;

    entityInFrontIndex = -1;

    for (int i = 0; i < entityLength; i++)
    {
        if(entityInFrontIndex == -1 && entityList[i].active == true)
        {
            Vector3 entPos = allObj[entityList[i].locationOnAllObj].position;

            for (int j = 0; j < 50; j += 1)
            {
                int pointX = (forw.x * j + pPos.x) / 1000;
                int pointY = (forw.y * j + pPos.y) / 1000;
                int pointZ = (forw.z * j + pPos.z) / 1000;

                if(pointX == (int)entPos.x && pointY == (int)entPos.y && pointZ == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
                if(pointX == (int)entPos.x && pointY+1 == (int)entPos.y && pointZ == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
				if(pointX == (int)entPos.x && pointY-1 == (int)entPos.y && pointZ == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
				if(pointX+1 == (int)entPos.x && pointY == (int)entPos.y && pointZ == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
				if(pointX-1 == (int)entPos.x && pointY == (int)entPos.y && pointZ == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
				if(pointX == (int)entPos.x && pointY == (int)entPos.y && pointZ+1 == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
				if(pointX == (int)entPos.x && pointY == (int)entPos.y && pointZ-1 == (int)entPos.z){
                	entityInFrontIndex = i;
					break;
				}
            }
        }
    }
}
