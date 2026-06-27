/* src/render/screen_effects.c - Underwater and menu visual effects */
#include "engine.h"


void intToHex(unsigned int num, char *hexStr) {
    const char hexDigits[] = "0123456789ABCDEF";
    int i = 0;

    // Handle zero case
    if (num == 0) {
        hexStr[i++] = '0';
        hexStr[i] = '\0';
        return;
    }

    // Convert the number to hexadecimal, starting from the least significant digit
    while (num != 0) {
        hexStr[i++] = hexDigits[num % 16];
        num /= 16;
    }

    // Null-terminate the string
    hexStr[i] = '\0';

    // Reverse the string to get the correct order
    int len = i;
    for (int j = 0; j < len / 2; ++j) {
        char tmp = hexStr[j];
        hexStr[j] = hexStr[len - j - 1];
        hexStr[len - j - 1] = tmp;
    }
}

//effects
void renderUnderWaterEffect()
{
    for (int y = 0; y < 216; y++)
    {
        for (int x = 0; x < 384; x++)
        {
            int xNew = x;
            int yNew = y;
            int ZbufWidth = 384;

            if(pixelSize == 2)
            {
                xNew = x >> 1;
                yNew = y >> 1;
                ZbufWidth = 192;
            }

            int waterDepthP1 = ZBuffer[xNew + yNew * ZbufWidth] / 200;
            if(waterDepthP1 > 40)
            waterDepthP1 = 40;
            int waterDepth = 40 + waterDepthP1;

            color_t startColor = *(VRAMAddress + x + y * 384);
            color_t endColor16 = combineColors(startColor, 0x42f4, waterDepth);

            *(VRAMAddress + x + y * 384) = endColor16;
        }
    }
}

void renderMenuEffect()
{
    for (int y = 0; y < 216; y++)
    {
        for (int x = 0; x < 384; x++)
        {
            color_t startColor = *(VRAMAddress + x + y * 384);
            color_t endColor16 = combineColors(startColor, 0x8c51, 50);

            *(VRAMAddress + x + y * 384) = endColor16;
        }
    }
}
