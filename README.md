This is the fxSDK port of [3D Minecraft by 010ello010](https://www.cemetech.net/forum/viewtopic.php?t=19552). Source code is slightly modified to support compiling for Add-in Push and fx-CG100.

Build this game by `fxsdk build-cg -s`. By default it targets fx-CG50. If you want to target fx-CG100 instead, uncomment the following line in CMakeList.txt:

```cmake
#set(MPM 1)
```

## Structure

```
cpcraft-refactor/
├── include/
│   ├── types.h              — All typedefs (Vector2/3/4, item, entity, block, etc.)
│   ├── globals.h            — All global variables (player pos, render state, etc.)
│   ├── chunk_constants.h    — #defines for chunk dimensions (width, height, etc.)
│   └── engine.h             — Master header with all function declarations
│
├── data/                     — Inline data tables extracted to separate headers
│   ├── sky_colors.h         — Sky color table (60 lines)
│   ├── skybox_colors.h      — Skybox gradient (4,546 lines)
│   ├── item_icons.h         — Item icon bitmaps (2,807 lines)
│   ├── logo_buttons.h       — Minecraft logo + UI buttons (94 lines)
│   ├── ascii_font.h         — ASCII font bitmap (282 lines)
│   ├── ui_elements.h        — Hotbar, hearts, recipe selector (264 lines)
│   ├── palettes.h           — Inventory palettes + colors (364 lines)
│   ├── block_types.h        — Block type definitions (741 lines)
│   ├── items.h              — Item definitions (1,309 lines)
│   ├── blocks.h             — Block properties (814 lines)
│   └── misc_data.h          — Missing icon + swap helper (633 lines)
│
├── src/
│   ├── main.c               — Entry point (57 lines)
│   ├── math_utils.c         — Math: arctan, sqrt, sin/cos, vectors (403 lines)
│   ├── delta_time.c         — Frame timing (20 lines)
│   ├── collision.c          — Player collision (101 lines)
│   ├── block_interaction.c  — Block reach, destroy, place (412 lines)
│   ├── block_use.c          — Chest + useBlock (328 lines)
│   ├── save.c               — Save/load (1,616 lines)
│   ├── misc.c               — Performance, map, entity reach (133 lines)
│   ├── game.c               — start_program + update (116 lines)
│   ├── render_object.c      — Main game loop (1,321 lines)
│   │
│   ├── render/
│   │   ├── drawing.c        — Screen clear, sprite copy (92 lines)
│   │   ├── skybox.c         — Skybox construction (125 lines)
│   │   ├── triangles.c      — Triangle rasterizers (3,625 lines)
│   │   ├── text.c           — Text rendering (33 lines)
│   │   ├── projection.c     — 3D projection, particles (247 lines)
│   │   ├── effects.c        — Crosshair, destruction (113 lines)
│   │   └── screen_effects.c — Underwater, menu effects (70 lines)
│   │
│   ├── world/
│   │   ├── noise.c          — Perlin noise (204 lines)
│   │   ├── random.c         — Random numbers, sort (76 lines)
│   │   ├── terrain.c        — Trees, ores, caves (174 lines)
│   │   ├── chunks.c         — Chunk gen, meshing, lighting (1,236 lines)
│   │   ├── redstone.c       — Redstone updates (1,557 lines)
│   │   └── world_mgmt.c     — Chunk/entity lifecycle (365 lines)
│   │
│   ├── entities/
│   │   ├── entity_ai.c      — Mob AI (1,072 lines)
│   │   └── entity_spawn.c   — Spawn functions (833 lines)
│   │
│   └── ui/
│       ├── hud.c            — Health, font, hotbar (248 lines)
│       ├── inventory.c      — Inventory, crafting, chest (2,476 lines)
│       └── inventory_mgmt.c — Add/remove items (236 lines)
│
├── (original files kept as-is: DMA_Transfer, compression, controls, fastBdisp, lz4, tlsf, etc.)
├── CMakeLists.txt
├── include/ (fx-cg SDK headers)
├── assets/
└── doc/
```

