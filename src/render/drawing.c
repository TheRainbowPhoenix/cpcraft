/* src/render/drawing.c */
/* src/render/drawing.c - Screen clear, sprite copy helpers */
#include "engine.h"

void clear(color_t color)
{
    for (int y = 0; y < resY; y++)
    memset(VRAMAddress + (y * LCD_WIDTH_PX), color, resX*2);
}
void clearAll(color_t color)
{
    memset(VRAMAddress, color, LCD_HEIGHT_PX * LCD_WIDTH_PX * 2);
}
void CopySprite(color_t* sprite, int x, int y, int width, int height, color_t transparentColor) {
    color_t* VRAM = (color_t*)GetVRAMAddress();
    VRAM += LCD_WIDTH_PX*y + x;
    for(int j = y; j < y+height; j++) 
    {
        for(int i = x; i < x+width; i++) 
        {
            
            if(*sprite != transparentColor)
            *VRAM = *sprite;

            VRAM++;
            sprite++;
        }
        VRAM += LCD_WIDTH_PX-width;
    }
}
void CopySpriteHeart(color_t* sprite, int x, int y, int width, int height, color_t transparentColor, int maxX) {
    color_t* VRAM = (color_t*)GetVRAMAddress();
    VRAM += LCD_WIDTH_PX*y + x;
    for(int j = y; j < y+height; j++) 
    {
        for(int i = x; i < x+width; i++) 
        {
            if(*sprite != transparentColor && i < x+maxX)
            *VRAM = *sprite;

            VRAM++;
            sprite++;
        }
        VRAM += LCD_WIDTH_PX-width;
    }
}
void CopySpriteNbitMasked(const unsigned char* data, int x, int y, int width, int height, const color_t* palette, color_t maskColor, unsigned int bitwidth)  
{
    color_t* VRAM = (color_t*) GetVRAMAddress();
    VRAM += (LCD_WIDTH_PX*y + x);
    int offset = 0;
    unsigned char buf;
    for(int j=y; j<y+height; j++)  
    {
        int availbits = 0;
        for(int i=x; i<x+width;  i++)  
        {
            if (!availbits)  
            {
                buf = data[offset++];
                availbits = 8;
            }
            color_t this = ((color_t)buf>>(8-bitwidth));
            color_t color = palette[this];
            if(color != maskColor)
            {
                *VRAM = color;
            }
            VRAM++;
            buf<<=bitwidth;
            availbits-=bitwidth;
        }
        VRAM += (LCD_WIDTH_PX-width);
    }
}
void CopySpriteIcon(color_t* sprite, int x, int y, int width, int height, color_t transparentColor) {
    color_t* VRAM = (color_t*)GetVRAMAddress();
    VRAM += LCD_WIDTH_PX*y + x;
    for(int j = y; j < y+height; j++) 
    {
        for(int i = x; i < x+width; i++) 
        {
            if(*sprite != transparentColor)
            *VRAM = *sprite;

            VRAM++;
            sprite++;
        }
        VRAM += LCD_WIDTH_PX-width;
    }
}

