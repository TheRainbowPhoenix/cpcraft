/* src/engine/ui.c */
/*
 * cpcraft-port — engine/ui.c
 *
 * HUD drawing.
 */
#include "ui.h"
#include "framebuffer.h"
#include "overlay.h"
#include "world.h"

void ui_draw_crosshair(void)
{
    /* The crosshair is a small "+" at the center of the framebuffer.
     * We draw it in white (0xFFFF) — works against most backgrounds. */
    const int cx = FB_W / 2;
    const int cy = FB_H / 2;
    const uint16_t color = 0xFFFF;

    /* Horizontal stroke. */
    for (int x = cx - 3; x <= cx + 3; x++)
        fb_pixel(x, cy, color);
    /* Vertical stroke. */
    for (int y = cy - 3; y <= cy + 3; y++)
        fb_pixel(cx, y, color);
}

/* Convert a block ID to a representative color for the hotbar slot. */
static uint16_t block_color(uint8_t id)
{
    switch (id)
    {
        case BLK_STONE:  return 0x8410;  /* gray */
        case BLK_GRASS:  return 0x07E0;  /* green */
        case BLK_DIRT:   return 0xC000 | 0x06 << 5 | 0x04;  /* brown-ish */
        case BLK_WOOD:   return 0x8400 | 0x32 << 5 | 0x08;  /* dark brown */
        case BLK_LEAVES: return 0x0400 | 0x3E << 5 | 0x00;  /* dark green */
        case BLK_SAND:   return 0xFFE0;  /* yellow */
        case BLK_WATER:  return 0x001F;  /* blue */
        case BLK_COBBLE: return 0x4208;  /* dark gray */
        case BLK_PLANK:  return 0xC000 | 0x52 << 5 | 0x08;  /* light brown */
        case BLK_GLASS:  return 0xBDF7;  /* light cyan */
        default:         return 0x0000;  /* black for empty */
    }
}

void ui_draw_hotbar(int selected, const uint8_t slots[9])
{
    /* 9 slots at the bottom of the screen. Each slot is 14px wide; total
     * hotbar width = 9 * 14 = 126 px, centered on the 160-wide screen
     * (origin_x = 17). Slot height = 14 px, bottom margin = 4 px. */
    const int slot_w = 14;
    const int slot_h = 14;
    const int total_w = 9 * slot_w;
    const int origin_x = (FB_W - total_w) / 2;
    const int origin_y = FB_H - slot_h - 4;

    for (int i = 0; i < 9; i++)
    {
        int x0 = origin_x + i * slot_w;
        int y0 = origin_y;

        /* Slot background: white if selected, dark gray otherwise. */
        uint16_t bg = (i == selected) ? 0xFFFF : 0x4210;
        fb_rect(x0, y0, slot_w, slot_h, bg);

        /* Inner color: the block's color, inset by 2px. */
        uint16_t fg = block_color(slots[i]);
        fb_rect(x0 + 2, y0 + 2, slot_w - 4, slot_h - 4, fg);
    }
}

void ui_draw_debug(int fps, uint32_t render_ticks, uint32_t blit_ticks)
{
    /* Top-left text overlay on a black background bar. */
    fb_rect(0, 0, FB_W, 14, 0x0000);

    if (fps > 0)
        overlay_printf(1, 4, 0xFFFF, "FPS %2d", fps);
    overlay_printf(40, 4, 0xFFFF, "R%5lu", (unsigned long)render_ticks);
    overlay_printf(85, 4, 0xFFFF, "B%5lu", (unsigned long)blit_ticks);
}
