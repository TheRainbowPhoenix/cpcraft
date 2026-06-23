#include <type_traits>
#include <sdk/os/file.h>
#include <sdk/os/debug.h>
#include <sdk/os/lcd.h>
#include <sdk/os/string.h>
#include <sdk/calc/calc.h>
#include <cstring>
#include <cstdlib>

std::remove_pointer_t<decltype(vram)> (*vram_bak)[width * height];

void calcInit(void)
{
    vram_bak = (decltype(vram_bak))malloc(sizeof(*vram_bak));
    
    if (!vram_bak)
    {
        return;
    }
    
    memcpy(vram_bak, vram, sizeof(*vram_bak));
    memset(vram, 0, sizeof(*vram_bak));
}

void calcExit(void) 
{
    memcpy(vram, vram_bak, sizeof(*vram_bak));
    LCD_Refresh();
    free(vram_bak);
}