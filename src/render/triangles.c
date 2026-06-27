/* src/render/triangles.c - Triangle/line rasterizers (textured, solid, transparent, skybox) */
#include "engine.h"
#include "config.h"


void renderLine(int x0, int x1, int y0, int y1)
{
    int exit;
    int dx = abs_int(x1 - x0);
    int dy = abs_int(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int resTot = resX + resY;

    while((x0 != x1 || y0 != y1) && exit < resTot)
    {
        exit++;
        if(x0 > 0 && x0 < resX && y0 > 0 && y0 < resY)
        *(VRAMAddress + y0 * LCD_WIDTH_PX + x0) = 0x0000;

        int err2 = 2 * err;

        if (err2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (err2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
void renderLinePX2(int x0, int x1, int y0, int y1)
{
    x0 /= 2;
    x1 /= 2;
    y0 /= 2;
    y1 /= 2;

    int exit;
    int dx = abs_int(x1 - x0);
    int dy = abs_int(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int resTot = resX + resY;

    while((x0 != x1 || y0 != y1) && exit < resTot)
    {
        exit++;
        if(x0 > 0 && x0 < resXZBuffer && y0 > 0 && y0 < resYZBuffer)
        {
            *(VRAMAddress + y0*2 * LCD_WIDTH_PX + x0*2 + 0) = 0x0000;
            *(VRAMAddress + y0*2 * LCD_WIDTH_PX + x0*2 + 1) = 0x0000;
            *(VRAMAddress + y0*2 * LCD_WIDTH_PX + x0*2 + LCD_WIDTH_PX) = 0x0000;
            *(VRAMAddress + y0*2 * LCD_WIDTH_PX + x0*2 + LCD_WIDTH_PX+1) = 0x0000;
        }

        int err2 = 2 * err;

        if (err2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (err2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
void renderLineWithWidthZBuffer(int x0, int x1, int y0, int y1, int width, int depth) {
    int exit;
    int dx = abs_int(x1 - x0);
    int dy = abs_int(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int resTot = resX + resY;

    while((x0 != x1 || y0 != y1) && exit < resTot)
    {
        exit++;
        for (int i = -width/2; i <= width/2; i++) {
            for (int j = -width/2; j <= width/2; j++) 
            {
                if(depth <= ZBuffer[(x0+i)/2 + (y0+j)/2 * resX] + 500)
                *(VRAMAddress + (y0+j) * LCD_WIDTH_PX + (x0+i)) = 0x0000;
            }
        }

        int err2 = 2 * err;

        if (err2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (err2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
void renderLineWithWidthZBufferPX2(int x0, int x1, int y0, int y1, int width, int depth) {
    x0 /= 2;
    x1 /= 2;
    y0 /= 2;
    y1 /= 2;
    width /= 2;

    int exit;
    int dx = abs_int(x1 - x0);
    int dy = abs_int(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int resTot = resX + resY;

    while((x0 != x1 || y0 != y1) && exit < resTot)
    {
        exit++;
        for (int i = -width/2; i <= width/2; i++) {
            for (int j = -width/2; j <= width/2; j++) 
            {
                if(depth <= ZBuffer[(x0+i) + (y0+j) * resXZBuffer] + 500)
                {
                    *(VRAMAddress + (y0+j)*2 * LCD_WIDTH_PX + (x0+i)*2 ) = 0x0000;
                    *(VRAMAddress + (y0+j)*2 * LCD_WIDTH_PX + (x0+i)*2 + 1) = 0x0000;
                    *(VRAMAddress + (y0+j)*2 * LCD_WIDTH_PX + (x0+i)*2 + LCD_WIDTH_PX) = 0x0000;
                    *(VRAMAddress + (y0+j)*2 * LCD_WIDTH_PX + (x0+i)*2 + LCD_WIDTH_PX+1) = 0x0000;
                }
            }
        }

        int err2 = 2 * err;

        if (err2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (err2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

//yeah lol does not work
/*
void renderTriangleFast(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth, const int texHeight, const int textureIndex)
{
	if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

	int dxAB = (vertexB.x - vertexA.x) << 16;
    int dyAB = (vertexB.y - vertexA.y) << 8;
    int dxAC = (vertexC.x - vertexA.x) << 16;
    int dyAC = (vertexC.y - vertexA.y) << 8;
    int dxBC = (vertexC.x - vertexB.x) << 16;
    int dyBC = (vertexC.y - vertexB.y) << 8;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

	int dxABTex = (texB.x - texA.x) << 16;
    int dyABTex = (texB.y - texA.y) << 16;
    int dxACTex = (texC.x - texA.x) << 16;
    int dyACTex = (texC.y - texA.y) << 16;
    int dxBCTex = (texC.x - texB.x) << 16;
    int dyBCTex = (texC.y - texB.y) << 16;

    int TexAB_X = (dyAB != 0) ? (dxABTex / dyAB) : 0;
    int TexAC_X = (dyAC != 0) ? (dxACTex / dyAC) : 0;
    int TexBC_X = (dyBC != 0) ? (dxBCTex / dyBC) : 0;

	int TexAB_Y = (dyAB != 0) ? (dyABTex / dyAB) : 0;
    int TexAC_Y = (dyAC != 0) ? (dyACTex / dyAC) : 0;
    int TexBC_Y = (dyBC != 0) ? (dyBCTex / dyBC) : 0;

	int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

	int texWidth8 = (texWidth-1) << 8;
    int texHeight8 = (texHeight-1) << 8;

	if(endA != endB)
	{
		for (int y = endA; y < endB; y += 2)
		{
			int yInTraingle1 = y - vertexA.y;
            int x0 = (((lineAB * yInTraingle1) >> 8) + vertexA.x) >> 1;
            int x1 = (((lineAC * yInTraingle1) >> 8) + vertexA.x) >> 1;

			if (x0 > x1) swap(&x0, &x1);

			yInTraingle1 = (yInTraingle1 > 0) ? yInTraingle1 : 1;

			int texX0_x = TexAB_X*yInTraingle1+(texA.x << 8);
			int texX0_y = TexAB_Y*yInTraingle1+(texA.y << 8);

			int x1Minusx0 = (x1-x0 > 0) ? x1-x0 : 1;
			int diffX = ((TexAC_X*yInTraingle1+(texC.x << 8))-texX0_x)/x1Minusx0;
			int diffY = ((TexAC_Y*yInTraingle1+(texC.y << 8))-texX0_y)/x1Minusx0;

			if(x0 < 0)
			{
				texX0_x += diffX*(-x0);
				texX0_y += diffY*(-x0);
			}

			x0 = (x0 > 0) ? x0 : 0;
			x1 = (x1 < LCD_WIDTH_PX) ? x1 : LCD_WIDTH_PX;

			unsigned short *ZBufferLocation = &ZBuffer[y*LCD_WIDTH_PX_HALF + x0];
			unsigned int *VRAMLcation1 = &VRAMAddress[y * LCD_WIDTH_PX + x0];
			unsigned int *VRAMLcation2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + x0];

			for (int x = x0; x < x1; x++)
			{
				if (depth < *ZBufferLocation)
                {
                    int u = (texX0_x < 0) ? 0 : ((texX0_x > texWidth8) ? texWidth8 : texX0_x);
                    int v = (texX0_y < 0) ? 0 : ((texX0_y > texHeight8) ? texHeight8 : texX0_y);

                    color_t colorFinalP1 = textures[textureIndex][(u >> 8) + (v >> 8) * 16];
                    unsigned int colorFinalP2 = (colorFinalP1 << 16) | colorFinalP1;

                    *VRAMLcation1 = colorFinalP2;
                    *VRAMLcation2 = colorFinalP2;
                    *ZBufferLocation = depth;
                }
                
                ZBufferLocation++;
                VRAMLcation1++;
                VRAMLcation2++;

                texX0_x += diffX;
                texX0_y += diffY;
			}
		}
	}

	if(endB != endC)
	{
		int last = vertexB.y + vertexB.y - vertexA.y;
		
		for (int y = endB; y < endC; y += 2)
		{
			int yInTraingle1 = y - vertexB.y;
            int yInTraingle2 = y - last;
            int x0 = (((lineBC * yInTraingle1) >> 8) + vertexB.x) >> 1;
            int x1 = (((lineAC * yInTraingle2) >> 8) + vertexA.x) >> 1;

			if (x0 > x1) swap(&x0, &x1);

			yInTraingle1 = (yInTraingle1 > 0) ? yInTraingle1 : 1;

			int texX0_x = TexBC_X*yInTraingle1;
			int texX0_y = TexBC_Y*yInTraingle1;

			int x1Minusx0 = (x1-x0 > 0) ? x1-x0 : 1;
			int diffX = ((TexAC_X*yInTraingle2)-texX0_x)/x1Minusx0;
			int diffY = ((TexAC_Y*yInTraingle2)-texX0_y)/x1Minusx0;

			if(x0 < 0)
			{
				texX0_x += diffX*(-x0);
				texX0_y += diffY*(-x0);
			}

			x0 = (x0 > 0) ? x0 : 0;
			x1 = (x1 < LCD_WIDTH_PX) ? x1 : LCD_WIDTH_PX;

			unsigned short *ZBufferLocation = &ZBuffer[y*LCD_WIDTH_PX_HALF + x0];
			unsigned int *VRAMLcation1 = &VRAMAddress[y * LCD_WIDTH_PX + (x0 << 1)];
			unsigned int *VRAMLcation2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (x0 << 1)];

			for (int x = x0; x < x1; x++)
			{
				if (depth < *ZBufferLocation)
                {
                    int u = (texX0_x < 0) ? 0 : ((texX0_x > texWidth8) ? texWidth8 : texX0_x);
                    int v = (texX0_y < 0) ? 0 : ((texX0_y > texHeight8) ? texHeight8 : texX0_y);

                    color_t colorFinalP1 = textures[textureIndex][(u >> 8) + (v >> 8) * 16];
                    unsigned int colorFinalP2 = (colorFinalP1 << 16) | colorFinalP1;

                    *VRAMLcation1 = colorFinalP2;
                    *VRAMLcation2 = colorFinalP2;
                    *ZBufferLocation = depth;
					
					//char txt[50];
					//sprintf(txt, "succes! x0: %d, x1: %d, x: %d, y: %d", x0, x1, x, y);
					//renderTextUpdate(5, 85, 100, txt);
                }
                
                ZBufferLocation++;
                VRAMLcation1++;
                VRAMLcation2++;

                texX0_x += diffX;
                texX0_y += diffY;
			}
		}
		
	}
}
*/

//triangle rendering
//to be worked on
void renderTriangle(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth, int brightness)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    color_t final_color = combineColorsWithBrightness(color, brightness);

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = y * resX;
            int yOnScreen = y * LCD_WIDTH_PX;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    *(VRAMAddress + yOnScreen + x) = final_color;
                    ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = y - vertexB->y + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0);  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1);  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = y * resX;
            int yOnScreen = y * LCD_WIDTH_PX;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    *(VRAMAddress + yOnScreen + x) = final_color;
                    ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}
void renderTrianglePX2(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth, char brightness)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC->y % 2 == 1) ? endC+1 : endC;

    color_t final_color = combineColorsWithBrightness(color, brightness);

    const unsigned int endColor = ((int)final_color << 16) + final_color;
    int LCD_WIDTH_PXH = LCD_WIDTH_PX >> 1;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y+=2)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1)); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1)); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    *(yOnScreen  + x) = endColor;
                    *(yOnScreen2 + x) = endColor;
                    ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y+=2)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = y - vertexB->y + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1));  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1));  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    *(yOnScreen  + x) = endColor;
                    *(yOnScreen2 + x) = endColor;
                    ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}
void renderTriangleNB(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    //endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    //endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    //endC = (vertexC->y % 2 == 1) ? endC+1 : endC;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
                int yInTraingle1 = y - vertexA->y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
                x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
                x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
                
                if (x0 > x1) swap(&x0, &x1);

                int yIndex = y * resX;
                int yOnScreen = y * LCD_WIDTH_PX;
                for (int x = x0; x < x1; x++)
                {
                    int depthIndex = yIndex + x;
                    if (depth < ZBuffer[depthIndex])
                    {
                        *(VRAMAddress + yOnScreen + x) = color;
                        ZBuffer[depthIndex] = depth;
                    }
                }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
                int yInTraingle1 = y - vertexB->y;
                int yInTraingle2 = y - vertexB->y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
                x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0);  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
                x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1);  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
                
                if (x0 > x1) swap(&x0, &x1);

                int yIndex = y * resX;
                int yOnScreen = y * LCD_WIDTH_PX;
                for (int x = x0; x < x1; x++)
                {
                    int depthIndex = yIndex + x;
                    if (depth < ZBuffer[depthIndex])
                    {
                        *(VRAMAddress + yOnScreen + x) = color;
                        ZBuffer[depthIndex] = depth;
                    }
                }
        }
    }
}
void renderTriangleNBPX2(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC->y % 2 == 1) ? endC+1 : endC;

    unsigned int *endColor = 0xE5200040;

    *endColor = ((int)color << 16) + color;
    int LCD_WIDTH_PXH = LCD_WIDTH_PX >> 1;

    //unsigned int *endColor = 0xE5017000;
    //*endColor = ((int)color << 16) + color;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y+=2)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1)); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1)); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    *(yOnScreen  + x) = *endColor;
                    *(yOnScreen2 + x) = *endColor;
                    ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y+=2)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = yInTraingle1 + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1));  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1));  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    *(yOnScreen  + x) = *endColor;
                    *(yOnScreen2 + x) = *endColor;
                    ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}

//done
void renderTriangleTexturedNBT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA.x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                //idk why but having it in this exact order saves 68 ticks per 6k 100x100 triangles so 8% faster
                int yIndex = y * resX;
                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress + minXClamp;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            *yOnScreen = textureAdress[(u >> 9) + (v >> 9) * 16];
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB.x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;
                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress + minXClamp;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            *yOnScreen = textureAdress[(u >> 9) + (v >> 9) * 16];
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress, const int brightness)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA.x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                //idk why but having it in this exact order saves 68 ticks per 6k 100x100 triangles so 8% faster
                int yIndex = y * resX;
                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress + minXClamp;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            *yOnScreen = combineColorsWithBrightness(textureAdress[(u >> 9) + (v >> 9) * 16], brightness);
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB.x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;
                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress + minXClamp;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            *yOnScreen = combineColorsWithBrightness(textureAdress[(u >> 9) + (v >> 9) * 16], brightness);
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedNBPX2T(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2S v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2S v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2S(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2S preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2S(preCal, v1, 10);
    int b2 = crossVec2S(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  texWidth_ * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * texHeight_ * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  texWidth_ / d * 512 / 500,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * texHeight_ / d * 512 / 500,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  texWidth_ / d * 512 / 500,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * texHeight_ / d * 512 / 500,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
                int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;
                int yIndex = (y >> 1) * LCD_WIDTH_PX_HALF;

                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            unsigned int colorFinalP2 = (colorFinalP1 << 16) | colorFinalP1;

                            *yOnScreen1 = colorFinalP2;
                            *yOnScreen2 = colorFinalP2;
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
                int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;
                int yIndex = (y >> 1) * LCD_WIDTH_PX_HALF;

                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            unsigned int colorFinalP2 = (colorFinalP1 << 16) | colorFinalP1;

                            *yOnScreen1 = colorFinalP2;
                            *yOnScreen2 = colorFinalP2;
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedPX2T(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress, const int brightness)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;
    int sizeHalf = LCD_WIDTH_PX_HALF;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
                int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            color_t colorFinalP2 = combineColorsWithBrightness(colorFinalP1, brightness);

                            unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                            *yOnScreen1 = colorFinalP3;
                            *yOnScreen2 = colorFinalP3;
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
                int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            color_t colorFinalP2 = combineColorsWithBrightness(colorFinalP1, brightness);

                            unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                            *yOnScreen1 = colorFinalP3;
                            *yOnScreen2 = colorFinalP3;
                            *yOnZbuffer = depth;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}

void renderTriangleTexturedBlackTransNBT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA.x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t endColor = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(endColor != 0x0000)
                        {
                            if (depth < yOnZbuffer[x])
                            {
                                yOnScreen[x] = endColor;
                                yOnZbuffer[x] = depth;
                            }
                        }
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB.x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t endColor = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(endColor != 0x0000)
                        {
                            if (depth < yOnZbuffer[x])
                            {
                                yOnScreen[x] = endColor;
                                yOnZbuffer[x] = depth;
                            }
                        }
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedBlackTransT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress, const int brightness)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA.x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t endColor = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(endColor != 0x0000)
                        {
                            if (depth < yOnZbuffer[x])
                            {
                                yOnScreen[x] = combineColorsWithBrightness(endColor, brightness);
                                yOnZbuffer[x] = depth;
                            }
                        }
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB.x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t endColor = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(endColor != 0x0000)
                        {
                            if (depth < yOnZbuffer[x])
                            {
                                yOnScreen[x] = combineColorsWithBrightness(endColor, brightness);
                                yOnZbuffer[x] = depth;
                            }
                        }
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedBlackTransNBPX2T(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;
    int sizeHalf = LCD_WIDTH_PX_HALF;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
                int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(colorFinalP1 != 0x0000)
                        {
                            if (depth < *yOnZbuffer)
                            {
                                unsigned int colorFinalP2 = (colorFinalP1 << 16) | colorFinalP1;

                                *yOnScreen1 = colorFinalP2;
                                *yOnScreen2 = colorFinalP2;
                                *yOnZbuffer = depth;
                            }
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
                int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(colorFinalP1 != 0x0000)
                        {
                            if (depth < *yOnZbuffer)
                            {
                                unsigned int colorFinalP2 = (colorFinalP1 << 16) | colorFinalP1;

                                *yOnScreen1 = colorFinalP2;
                                *yOnScreen2 = colorFinalP2;
                                *yOnZbuffer = depth;
                            }
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedBlackTransPX2T(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress, const int brightness)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;
    int sizeHalf = LCD_WIDTH_PX_HALF;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
                int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(colorFinalP1 != 0x0000)
                        {
                            if (depth < *yOnZbuffer)
                            {
                                color_t colorFinalP2 = combineColorsWithBrightness(colorFinalP1, brightness);
                                unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                                *yOnScreen1 = colorFinalP3;
                                *yOnScreen2 = colorFinalP3;
                                *yOnZbuffer = depth;
                            }
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
                int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                        int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                        color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];

                        if(colorFinalP1 != 0x0000)
                        {
                            if (depth < *yOnZbuffer)
                            {
                                color_t colorFinalP2 = combineColorsWithBrightness(colorFinalP1, brightness);
                                unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                                *yOnScreen1 = colorFinalP3;
                                *yOnScreen2 = colorFinalP3;
                                *yOnZbuffer = depth;
                            }
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}

void renderTriangleTexturedNBTransT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 1000,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 1000,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA.x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                //idk why but having it in this exact order saves 68 ticks per 6k 100x100 triangles so 8% faster
                int yIndex = y * resX;
                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress + minXClamp;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            color_t colorFinalP2 = combineColors(*yOnScreen, colorFinalP1, 50);

                            *yOnScreen = colorFinalP2;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB.x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA.x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int yIndex = y * resX;
                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress + minXClamp;
                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            color_t colorFinalP2 = combineColors(*yOnScreen, colorFinalP1, 50);

                            *yOnScreen = colorFinalP2;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedNBPX2TransT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;
    int sizeHalf = LCD_WIDTH_PX_HALF;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
                int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;
                int yIndex = (y >> 1) * LCD_WIDTH_PX_HALF;

                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            color_t colorFinalP2 = combineColors(*yOnScreen1, colorFinalP1, 50);
                            const unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                            *yOnScreen1 = colorFinalP3;
                            *yOnScreen2 = colorFinalP3;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
                int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;
                int yIndex = (y >> 1) * LCD_WIDTH_PX_HALF;

                unsigned short* yOnZbuffer = &ZBuffer[yIndex + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = textureAdress[(u >> 9) + (v >> 9) * 16];
                            color_t colorFinalP2 = combineColors(*yOnScreen1, colorFinalP1, 50);
                            const unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                            *yOnScreen1 = colorFinalP3;
                            *yOnScreen2 = colorFinalP3;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}
void renderTriangleTexturedPX2TransT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress, const int brightness)
{
    const int texHeight = texHeight_-1;
    const int texWidth = texWidth_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;
    int sizeHalf = LCD_WIDTH_PX_HALF;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texWidth9 = texWidth << 9;
    int texHeight9 = texHeight << 9;

    Vector2I texCoord = 
    {
        (texA.x * b0 / 10 + texB.x * b1 / 10 + texC.x * b2 / 10) * 1000 / d *  (texWidth_) * 512 / 1000,
        (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * (texHeight_) * 512 / 1000,
    };

    Vector2I texCoord_x = 
    {
        (texA.x * b0_x / 10 + texB.x * b1_x / 10 + texC.x * b2_x / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };
    
    Vector2I texCoord_y = 
    {
        (texA.x * b0_y / 10 + texB.x * b1_y / 10 + texC.x * b2_y / 10) * 1000 *  (texWidth_) / d * 512 / 500,
        (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * (texHeight_) / d * 512 / 500,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA.y;
                int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
                int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = combineColorsWithBrightness(textureAdress[(u >> 9) + (v >> 9) * 16], brightness);
                            color_t colorFinalP2 = combineColors(*yOnScreen1, colorFinalP1, 50);
                            const unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                            *yOnScreen1 = colorFinalP3;
                            *yOnScreen2 = colorFinalP3;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB.y;
                int yInTraingle2 = y - vertexB.y + last;
                int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
                int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = 
                {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0,
                };

                int minXClamp = (x0 > 1) ? x0 : 1;

                unsigned short* yOnZbuffer = &ZBuffer[(y >> 1) * LCD_WIDTH_PX_HALF + minXClamp];
                unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
                unsigned int *yOnScreen2 = &VRAMAddress[(y + 1) * LCD_WIDTH_PX + (minXClamp << 1)];
                
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < LCD_WIDTH_PX_HALF)
                    {
                        if (depth < *yOnZbuffer)
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x > texWidth9) ? texWidth9 : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y > texHeight9) ? texHeight9 : texCoord_i.y);

                            color_t colorFinalP1 = combineColorsWithBrightness(textureAdress[(u >> 9) + (v >> 9) * 16], brightness);
                            color_t colorFinalP2 = combineColors(*yOnScreen1, colorFinalP1, 50);
                            const unsigned int colorFinalP3 = (colorFinalP2 << 16) | colorFinalP2;

                            *yOnScreen1 = colorFinalP3;
                            *yOnScreen2 = colorFinalP3;
                        }
                        
                        yOnZbuffer++;
                        yOnScreen1++;
                        yOnScreen2++;
                    }

                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}

//skybox rendering
void renderSkybox(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC)
{
	int skyBrightnessNow = 15-(skyBrightness-1);
	int texHeight_ = 128;
    const int texHeight = texHeight_-1;

    if (vertexA.y > vertexB.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texA;
        texA = texB;
        texB = temp2;
    }
    if (vertexA.y > vertexC.y) {
        Vector2S temp1 = vertexA;
        vertexA = vertexC;
        vertexC = temp1;

        Vector2S temp2 = texA;
        texA = texC;
        texC = temp2;
    }
    if (vertexB.y > vertexC.y) {
        Vector2S temp1 = vertexC;
        vertexC = vertexB;
        vertexB = temp1;

        Vector2S temp2 = texC;
        texC = texB;
        texB = temp2;
    }

    int dxAB = (vertexB.x - vertexA.x) << 18;
    int dyAB = (vertexB.y - vertexA.y) << 9;
    int dxAC = (vertexC.x - vertexA.x) << 18;
    int dyAC = (vertexC.y - vertexA.y) << 9;
    int dxBC = (vertexC.x - vertexB.x) << 18;
    int dyBC = (vertexC.y - vertexB.y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA.y < 0) ? 0 : ((vertexA.y > resY) ? resY : vertexA.y);
    int endB = (vertexB.y < 0) ? 0 : ((vertexB.y > resY) ? resY : vertexB.y);
    int endC = (vertexC.y < 0) ? 0 : ((vertexC.y > resY) ? resY : vertexC.y);

    int last = vertexB.y - vertexA.y;

    endA = (vertexA.y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB.y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC.y % 2 == 1) ? endC+1 : endC;

    Vector2I v0 = {(vertexB.x - vertexA.x) * 10, (vertexB.y - vertexA.y) * 10};
    Vector2I v1 = {(vertexC.x - vertexA.x) * 10, (vertexC.y - vertexA.y) * 10};

    int minY = clamp(vertexA.y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA.x * 10, minY - vertexA.y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    int texHeight9 = texHeight << 9;

	int texCoord_y = (texA.y * b0 / 10 + texB.y * b1 / 10 + texC.y * b2 / 10) * 1000 / d * texHeight_ * 512 / 1000;
	int texCoord_x_y = (texA.y * b0_x / 10 + texB.y * b1_x / 10 + texC.y * b2_x / 10) * 1000 * texHeight_ / d * 512 / 500;
	int texCoord_y_y = (texA.y * b0_y / 10 + texB.y * b1_y / 10 + texC.y * b2_y / 10) * 1000 * texHeight_ / d * 512 / 500;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y += 2)
        {
            int yInTraingle1 = y - vertexA.y;
            int x0 = (((lineAB * yInTraingle1) >> 9) + vertexA.x) >> 1;
            int x1 = (((lineAC * yInTraingle1) >> 9) + vertexA.x) >> 1;
            
            if (x0 > x1) swap(&x0, &x1);

			int texCoord_i_y = texCoord_y + texCoord_x_y * x0;
            int minXClamp = (x0 > 1) ? x0 : 1;
            unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
            
            for (int x = x0; x < x1; x++)
            {
                if(x > 0 && x < LCD_WIDTH_PX_HALF)
                {
                    int v = (texCoord_i_y < 0) ? 0 : ((texCoord_i_y > texHeight9) ? texHeight9 : texCoord_i_y);
                    unsigned int colorFinal = skyboxColors[skyBrightnessNow][v >> 9];

                    *yOnScreen1 = colorFinal;
                    *(yOnScreen1 + LCD_WIDTH_PX_HALF) = colorFinal;
						
                    yOnScreen1++;

					texCoord_i_y += texCoord_x_y;
                }
            }

            texCoord_y += texCoord_y_y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y += 2)
        {
            int yInTraingle1 = y - vertexB.y;
            int yInTraingle2 = y - vertexB.y + last;
            int x0 = (((lineBC * yInTraingle1) >> 9) + vertexB.x) >> 1;
            int x1 = (((lineAC * yInTraingle2) >> 9) + vertexA.x) >> 1;
            
            if (x0 > x1) swap(&x0, &x1);

            int texCoord_i_y = texCoord_y + texCoord_x_y * x0;
            int minXClamp = (x0 > 1) ? x0 : 1;
            unsigned int *yOnScreen1 = &VRAMAddress[y * LCD_WIDTH_PX + (minXClamp << 1)];
            
            for (int x = x0; x < x1; x++)
            {
                if(x > 0 && x < LCD_WIDTH_PX_HALF)
                {
                    int v = (texCoord_i_y < 0) ? 0 : ((texCoord_i_y > texHeight9) ? texHeight9 : texCoord_i_y);
                    unsigned int colorFinal = skyboxColors[skyBrightnessNow][v >> 9];

                    *yOnScreen1 = colorFinal;
                    *(yOnScreen1 + LCD_WIDTH_PX_HALF) = colorFinal;
						
                    yOnScreen1++;

					texCoord_i_y += texCoord_x_y;
                }
            }

            texCoord_y += texCoord_y_y;
        }
    }
}

//to be worked on
void renderTrianglePX2Trans(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth, char brightness)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC->y % 2 == 1) ? endC+1 : endC;

    color_t endBrightnessColor = combineColorsWithBrightness(color, brightness);

    int LCD_WIDTH_PXH = LCD_WIDTH_PX >> 1;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y+=2)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1)); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1)); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    //int waterDepthP1 = (ZBuffer[depthIndex] - depth) / 500;
                    //if(waterDepthP1 > 40)
                    //waterDepthP1 = 40;
                    //int waterDepth = 40 + waterDepthP1;

                    color_t startColor = *(yOnScreen  + x);
                    color_t endColor16 = combineColors(startColor, endBrightnessColor, 50);
                    const unsigned int endColor32 = ((int)endColor16 << 16) + endColor16;

                    *(yOnScreen  + x) = endColor32;
                    *(yOnScreen2 + x) = endColor32;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y+=2)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = y - vertexB->y + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1));  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1));  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    //int waterDepthP1 = (ZBuffer[depthIndex] - depth) / 500;
                    //if(waterDepthP1 > 40)
                    //waterDepthP1 = 40;
                    //int waterDepth = 40 + waterDepthP1;

                    color_t startColor = *(yOnScreen  + x);
                    color_t endColor16 = combineColors(startColor, endBrightnessColor, 50);
                    const unsigned int endColor32 = ((int)endColor16 << 16) + endColor16;

                    *(yOnScreen  + x) = endColor32;
                    *(yOnScreen2 + x) = endColor32;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}
void renderTriangleNBPX2Trans(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    endC = (vertexC->y % 2 == 1) ? endC+1 : endC;
    
    int LCD_WIDTH_PXH = LCD_WIDTH_PX >> 1;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y+=2)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1)); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1)); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    //int waterDepthP1 = (ZBuffer[depthIndex] - depth) / 500;
                    //if(waterDepthP1 > 40)
                    //waterDepthP1 = 40;
                    //int waterDepth = 40 + waterDepthP1;

                    color_t startColor = *(yOnScreen  + x);
                    color_t endColor16 = combineColors(startColor, color, 50);
                    const unsigned int endColor32 = ((int)endColor16 << 16) + endColor16;

                    *(yOnScreen  + x) = endColor32;
                    *(yOnScreen2 + x) = endColor32;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y+=2)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = y - vertexB->y + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? (resX >> 1) : (x0 >> 1));  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? (resX >> 1) : (x1 >> 1));  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = (y >> 1) * resXZBuffer;
            unsigned int *yOnScreen  = y * LCD_WIDTH_PX + VRAMAddress;
            unsigned int *yOnScreen2 = yOnScreen + LCD_WIDTH_PXH;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    //int waterDepthP1 = (ZBuffer[depthIndex] - depth) / 500;
                    //if(waterDepthP1 > 40)
                    //waterDepthP1 = 40;
                    //int waterDepth = 40 + waterDepthP1;

                    color_t startColor = *(yOnScreen  + x);
                    color_t endColor16 = combineColors(startColor, color, 50);
                    const unsigned int endColor32 = ((int)endColor16 << 16) + endColor16;

                    *(yOnScreen  + x) = endColor32;
                    *(yOnScreen2 + x) = endColor32;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}

void renderTriangleTrans(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth, int brightness)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    //endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    //endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    //endC = (vertexC->y % 2 == 1) ? endC+1 : endC;
    color_t final_color = combineColorsWithBrightness(color, brightness);

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = y * resX;
            int yOnScreen = y * LCD_WIDTH_PX;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    color_t startColor = *(VRAMAddress + yOnScreen + x);
                    color_t endColor16 = combineColors(startColor, final_color, 50);

                    *(VRAMAddress + yOnScreen + x) = endColor16;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = y - vertexB->y + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0);  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1);  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = y * resX;
            int yOnScreen = y * LCD_WIDTH_PX;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    color_t startColor = *(VRAMAddress + yOnScreen + x);
                    color_t endColor16 = combineColors(startColor, final_color, 50);

                    *(VRAMAddress + yOnScreen + x) = endColor16;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}
void renderTriangleNBTrans(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth)
{
    if (vertexA->y > vertexB->y) swap(&vertexA, &vertexB);
    if (vertexA->y > vertexC->y) swap(&vertexA, &vertexC);
    if (vertexB->y > vertexC->y) swap(&vertexB, &vertexC);
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;

    //endA = (vertexA->y % 2 == 1) ? endA+1 : endA;
    //endB = (vertexB->y % 2 == 1) ? endB+1 : endB;
    //endC = (vertexC->y % 2 == 1) ? endC+1 : endC;

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            int yInTraingle1 = y - vertexA->y;
            int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
            int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0); //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1); //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = y * resX;
            int yOnScreen = y * LCD_WIDTH_PX;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    color_t startColor = *(VRAMAddress + yOnScreen + x);
                    color_t endColor16 = combineColors(startColor, color, 50);

                    *(VRAMAddress + yOnScreen + x) = endColor16;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            int yInTraingle1 = y - vertexB->y;
            int yInTraingle2 = y - vertexB->y + last;
            int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
            int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
            x0 = (x0 < 0) ? 0 : ((x0 > resX) ? resX : x0);  //is faster then if(x0 > resX) x0 = resX;    else if(x0 < 0) x0 = 0;
            x1 = (x1 < 0) ? 0 : ((x1 > resX) ? resX : x1);  //is faster then if(x1 > resX) x1 = resX;    else if(x1 < 0) x1 = 0;
            
            if (x0 > x1) swap(&x0, &x1);

            int yIndex = y * resX;
            int yOnScreen = y * LCD_WIDTH_PX;
            for (int x = x0; x < x1; x++)
            {
                int depthIndex = yIndex + x;
                if (depth < ZBuffer[depthIndex])
                {
                    color_t startColor = *(VRAMAddress + yOnScreen + x);
                    color_t endColor16 = combineColors(startColor, color, 50);

                    *(VRAMAddress + yOnScreen + x) = endColor16;
                    //ZBuffer[depthIndex] = depth;
                }
            }
        }
    }
}
void renderTriangleTexturedTrans(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, Vector2S *texA, Vector2S *texB, Vector2S *texC, const unsigned short depth, const int texWidth, const int texHeight, const color_t *textureAdress, const int brightness)
{
    if (vertexA->y > vertexB->y) {
        swap(&vertexA, &vertexB);
        swap(&texA, &texB);
    }
    if (vertexA->y > vertexC->y) {
        swap(&vertexA, &vertexC);
        swap(&texA, &texC);
    }
    if (vertexB->y > vertexC->y) {
        swap(&vertexB, &vertexC);
        swap(&texB, &texC);
    }
    
    int dxAB = (vertexB->x - vertexA->x) << 18;
    int dyAB = (vertexB->y - vertexA->y) << 9;
    int dxAC = (vertexC->x - vertexA->x) << 18;
    int dyAC = (vertexC->y - vertexA->y) << 9;
    int dxBC = (vertexC->x - vertexB->x) << 18;
    int dyBC = (vertexC->y - vertexB->y) << 9;

    int lineAB = (dyAB != 0) ? (dxAB / dyAB) : 0;
    int lineAC = (dyAC != 0) ? (dxAC / dyAC) : 0;
    int lineBC = (dyBC != 0) ? (dxBC / dyBC) : 0;

    // Calculate start and end points
    int endA = (vertexA->y < 0) ? 0 : ((vertexA->y > resY) ? resY : vertexA->y);
    int endB = (vertexB->y < 0) ? 0 : ((vertexB->y > resY) ? resY : vertexB->y);
    int endC = (vertexC->y < 0) ? 0 : ((vertexC->y > resY) ? resY : vertexC->y);

    int last = vertexB->y - vertexA->y;
    int resX_minus_1 = resX - 1;

    Vector2I v0 = {(vertexB->x - vertexA->x) * 10, (vertexB->y - vertexA->y) * 10};
    Vector2I v1 = {(vertexC->x - vertexA->x) * 10, (vertexC->y - vertexA->y) * 10};

    int minY = clamp(vertexA->y, 0, resY-1) * 10;

    int d = crossVec2I(v0, v1, 10);
    if(d == 0) return;
    int b1_x = v1.y;
    int b1_y = -v1.x;
    int b2_x = -v0.y;
    int b2_y = v0.x;
    int b0_x = 0 - b1_x - b2_x;
    int b0_y = 0 - b1_y - b2_y;
    Vector2I preCal = {0 - vertexA->x * 10, minY - vertexA->y * 10};
    int b1 = crossVec2I(preCal, v1, 10);
    int b2 = crossVec2I(v0, preCal, 10);
    int b0 = d - b1 - b2;

    Vector2I texCoord = {
        (texA->x * b0 / 10 + texB->x * b1 / 10 + texC->x * b2 / 10) * 1000 / d *  (texWidth) * 512 / 1000,
        (texA->y * b0 / 10 + texB->y * b1 / 10 + texC->y * b2 / 10) * 1000 / d * (texHeight) * 512 / 1000,
    };
    Vector2I texCoord_x = {
        (texA->x * b0_x / 10 + texB->x * b1_x / 10 + texC->x * b2_x / 10) * 1000 *  (texWidth) / d * 512 / 1000,
        (texA->y * b0_x / 10 + texB->y * b1_x / 10 + texC->y * b2_x / 10) * 1000 * (texHeight) / d * 512 / 1000,
    };
    Vector2I texCoord_y = {
        (texA->x * b0_y / 10 + texB->x * b1_y / 10 + texC->x * b2_y / 10) * 1000 *  (texWidth) / d * 512 / 1000,
        (texA->y * b0_y / 10 + texB->y * b1_y / 10 + texC->y * b2_y / 10) * 1000 * (texHeight) / d * 512 / 1000,
    };

    if(endA != endB)
    {
        for (int y = endA; y < endB; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexA->y;
                int x0 = ((lineAB * yInTraingle1) >> 9) + vertexA->x;
                int x1 = ((lineAC * yInTraingle1) >> 9) + vertexA->x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0
                };

                int yIndex = y * resX;
                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress;
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < resX)
                    {
                        if (depth < ZBuffer[yIndex + x])
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x >= texWidth << 9) ? texWidth : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y >= texHeight << 9) ? texHeight : texCoord_i.y);

                            color_t startColor = *(yOnScreen  + x);
                            color_t endColor16 = combineColors(startColor, combineColorsWithBrightness(textureAdress[(u >> 9) + (v >> 9) * 16], brightness), 50);

                            *(yOnScreen + x) = endColor16;

                            //ZBuffer[yIndex + x] = depth;
                        }
                    }
                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }
            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
    if(endB != endC)
    {
        for (int y = endB; y < endC; y++)
        {
            if(y > 0 && y < resY)
            {
                int yInTraingle1 = y - vertexB->y;
                int yInTraingle2 = y - vertexB->y + last;
                int x0 = ((lineBC * yInTraingle1) >> 9) + vertexB->x;
                int x1 = ((lineAC * yInTraingle2) >> 9) + vertexA->x;
                
                if (x0 > x1) swap(&x0, &x1);

                Vector2I texCoord_i = {
                    texCoord.x + texCoord_x.x * x0,
                    texCoord.y + texCoord_x.y * x0
                };

                int yIndex = y * resX;
                unsigned short* yOnScreen = y * LCD_WIDTH_PX + VRAMAddress;
                for (int x = x0; x < x1; x++)
                {
                    if(x > 0 && x < resX)
                    {
                        if (depth < ZBuffer[yIndex + x])
                        {
                            int u = (texCoord_i.x < 0) ? 0 : ((texCoord_i.x >= texWidth << 9) ? texWidth : texCoord_i.x);
                            int v = (texCoord_i.y < 0) ? 0 : ((texCoord_i.y >= texHeight << 9) ? texHeight : texCoord_i.y);

                            color_t startColor = *(yOnScreen  + x);
                            color_t endColor16 = combineColors(startColor, combineColorsWithBrightness(textureAdress[(u >> 9) + (v >> 9) * 16], brightness), 50);
                            
                            *(yOnScreen + x) = endColor16;
                            
                            //ZBuffer[yIndex + x] = depth;
                        }
                    }
                    texCoord_i.x += texCoord_x.x;
                    texCoord_i.y += texCoord_x.y;
                }
            }

            texCoord.x += texCoord_y.x;
            texCoord.y += texCoord_y.y;
        }
    }
}

//text rendering
