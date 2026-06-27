/* src/ui/hud.c - Health bar, Minecraft font, hotbar, item details */
#include "engine.h"

color_t performanceColors[10] = {0xf800, 0xfc00, 0xffe0, 0xfc10, 0xfc1f, 0xf81f, 0x841f, 0x041f, 0x001f, 0x07ff};
char performanceName[10][35] = {
    "mapUpdate",
    "collision, fw, time",
    "key inputs",
    "vertice calculation",
    "clear screen",
    "triangle rendering",
    "water effect",
    "reset Zbuffer",
    "make UI",
    "update screen",
};

//UI
void drawHp(int x, int y, int barHeight, int barWidth, int hp, int maxHp)
{
	int hpBarWidth = (hp*(barWidth-1))/maxHp;
	for (int i = 0; i < barHeight; i++)
	{
		for (int j = 0; j < barWidth-1; j++)
		{
            VRAMAddress[(x+j) + (y+i) * 384] = 0x2200;

			color_t color = combineColors(0xe880, 0x07c0, (hp*100)/maxHp);

			if(j < hpBarWidth)
			VRAMAddress[(x+j) + (y+i) * 384] = color;
		}
        VRAMAddress[(x+barWidth-1) + (y+i) * 384] = 0x0000;
        VRAMAddress[(x+barWidth-1) + (y+i+2) * 384] = 0x0000;
	}

    for (int j = 0; j < barWidth; j++)
    {
        VRAMAddress[(x+j) + (y+2) * 384] = 0x0000;
    }
    
}
void drawMinecraftFont(int x1, int x2, int y, bool center, int textLength, char text[], color_t textColor, color_t textColorShadow)
{
    int xLetterPos = 0;
    int xCenter = 0;
    int totalXLength = 0;

    for (int i = 0; i < textLength; i++)
    totalXLength += asciiLength[text[i]];
    
    if(center == true)
    xCenter = (x2-x1-totalXLength)/2;
    else
    xCenter = 0;

    for (int i = 0; i < textLength; i++)
    {
        int letterIndex = text[i];

        int xOnTexture = letterIndex%16; 
        int yOnTexture = letterIndex/16;

        for (int y_ = 0; y_ < 8; y_++)
        {
            for (int x_ = 0; x_ < asciiLength[letterIndex]; x_++)
            {
                if(x_ < 384)
                {
                    int index = (xOnTexture*8+x_) + (yOnTexture*8+y_) * 128;
                    unsigned char currentPixel = asciiMap2[index>>3];
                    if((currentPixel & (1<<(7-index%8))) == 0)
                    VRAMAddress[(x1 + x_ + xLetterPos + 1 + xCenter) + (y + y_ + 1) * 384] = textColorShadow;
                }
            }
        }
        for (int y_ = 0; y_ < 8; y_++)
        {
            for (int x_ = 0; x_ < asciiLength[letterIndex]; x_++)
            {
                if(x_ < 384)
                {
                    int index = (xOnTexture*8+x_) + (yOnTexture*8+y_) * 128;
                    unsigned char currentPixel = asciiMap2[index>>3];
                    if((currentPixel & (1<<(7-index%8))) == 0)
                    VRAMAddress[(x1 + x_ + xLetterPos + xCenter) + (y + y_) * 384] = textColor;
                }
            }
        }
        xLetterPos += asciiLength[letterIndex];
    }
}
void itemDetails(int posX, int posY, char text[])
{
    int totalXLength = 0;
    int textLength = 0;

    for (int i = 0; i < 20; i++)
    {
        if(text[i] != 0)
        textLength++;
    }

    for (int i = 0; i < textLength; i++)
    totalXLength += asciiLength[text[i]];

    for (int y = posY; y < posY+16; y++)
    {
        for (int x = posX; x < posX+totalXLength+8; x++)
        {
            if(x < 384)
            VRAMAddress[x + y * 384] = combineColors(VRAMAddress[x + y * 384], 0x2104, 60);
        }
    }
    
    drawMinecraftFont(posX+4, posX+4, posY+4, false, textLength, text, 0xdefb, 0x39c7);
}

int renderdT = 0;
int renderdV = 0;

int UIstateOld = 0;
int cursorY = 0;
int cursorX = 0;
int cursorStage = 0;
int cursorTypeSelected = 0;
int cursorYExtra = 0;

int cursorXSelected = 0;
int cursorYSelected = 0;

float buttonPressTimer1 = 0;
float buttonPressTimer2 = 0;

int WorldOpenMode = 0; //0=create/load, 1=overwrite, 2=always load-

int returnTotalItemAmount(int type)
{
    int totAmount = 0;
    for (int i = 0; i < 27; i++)
    {
        if(inventoryBlockTypes[i] == type)
        totAmount += inventoryBlockAmount[i];
    }
    
    for (int i = 0; i < 9; i++)
    {
        if(hotbarBlockTypes[i] == type)
        totAmount += hotbarBlockAmount[i];
    }
    
    return totAmount;
}
void makeHealthBar()
{
    int halfHeart = health%2;
    int allheart = health/2;

    for (int i = 0; i < 10; i++)
    {
        CopySprite(heart[0], 60+i*14, 170, 14, 14, 0x0000);

        if(i < allheart)
        CopySpriteHeart(heart[1], 60+i*14, 170, 14, 14, 0x0000, 14);
        if(i == allheart && halfHeart == 1)
        CopySpriteHeart(heart[1], 60+i*14, 170, 14, 14, 0x0000, 8);
    }
    
}
void loadingScreen(int currentPos, int total, char text[], int textLength)
{
    for (int y = 0; y < 216; y++)
    {
        for (int x = 0; x < 384; x++)
        {
            int xOnTexture = x%16;
            int yOnTexture = y%16;
            //VRAMAddress[y*384+x] = textures[3][xOnTexture+yOnTexture*16];
            VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
        }
    }

    drawMinecraftFont(0, 384, 100, true, textLength, text, 0xdefb, 0x39c7);

    int progress = currentPos * 160 / total;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 160; x++)
        {
            if(x <= progress)
            VRAMAddress[(112+x) + (110+y) * 384] = 0x87f0;
            else
            VRAMAddress[(112+x) + (110+y) * 384] = 0x8410;
        }
    }

    Bdisp_PutDisp_DD();
}
void makeHotbar(int selected)
{
    for (int i = 0; i < 9; i++)
    {
        int startPos = 57+30*i;

        CopySprite(hotbarDeselected, startPos, 185, 30, 30, 0x0000);
        if(hotbarBlockTypes[i] != -1)
        {
            //itemIsdcons[allItem[hotbarBlockTypes[i]].iconIndex]
            CopySpriteIcon(&itemIcons2[676 * allItem[hotbarBlockTypes[i]].iconIndex], startPos+2, 187, 26, 26, 0x0000);

            if(hotbarBlockAmount[i] != 1)
            {
                char buffer[4];
                sprintf(buffer, "%d", hotbarBlockAmount[i]);

				drawMinecraftFont(startPos+16, startPos+16, 202, false, countDigits(hotbarBlockAmount[i]), buffer, 0xdefb, 0x39c7);
            }

			if(hotbarBlockHp[i] != allItem[hotbarBlockTypes[i]].breaksIn)
			drawHp(startPos+4, 210, 2, 22, hotbarBlockHp[i], allItem[hotbarBlockTypes[i]].breaksIn);
        }

        if(selected == i)
        {
            CopySprite(hotbarSelected, startPos-1, 184, 32, 32, 0x0000);
        }
    }
}
