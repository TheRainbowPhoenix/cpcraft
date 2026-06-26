# cpcraft-port

A fast-rendering engine base for the Casio ClassPad, built with the
[Hollyhock-3 SDK](https://github.com/ClasspadDev/hollyhock-3), and intended
as the foundation for porting
[CPCraft](https://github.com/TheRainbowPhoenix/cpcraft) (a tiny Minecraft)
to the ClassPad.

This is a clean rebuild of CPCraft's renderer. The original CPCraft has a
~30 000-line `engine.c` that mixes every concern together; this port keeps
the engine strictly modular so CPCraft features can be added one at a time
on top of a clean base.

## What's in this folder

This folder was forked from the
[HHK3template-main](https://github.com/search?q=HHK3template) project and
the engine was added under `src/engine/`. The original template's
`Makefile` was kept (with a few flag tweaks) so the build flow is the same
as for any HHK3 app.

```
cpcraft-port/
├─ Makefile            — HHK3-style Makefile (build with `make`, test with `make test`)
├─ src/
│  ├─ main.cpp         — entry point. Calls engine_init() then demo_run().
│  ├─ engine/          — the engine itself
│  │  ├─ engine.{c,h}       — one-stop init
│  │  ├─ builtins.h         — likely/unlikely, ENGINE_INLINE
│  │  ├─ math_types.h       — Vec2i/Vec3i/Vec4b integer vector types
│  │  ├─ power.h            — MSTPCR0 layout (un-gate TMU/DMAC clocks)
│  │  ├─ tmu.h              — TMU channel 1 (frame timing)
│  │  ├─ framebuffer.{c,h}  — 160x264 virtual FB + direct VRAM→LCD blit
│  │  │                       (no YRAM, no ocbwb — just CPU writes)
│  │  ├─ input.{c,h}        — GetInput()-based key polling with edge detection
│  │  ├─ math_lut.{c,h}     — 8-bit sin LUT + 16.16 fixed-point sin/cos
│  │  │                       (integer-only, no math.h, no FPU)
│  │  ├─ overlay.{c,h}      — tiny 4x6 font + printf for on-screen labels
│  │  ├─ texture.{c,h}      — procedural 16x16 textures + textured rect
│  │  ├─ world.{c,h}        — voxel world storage + terrain generation
│  │  │                       (64×32×64 blocks, hash-based heightmap + trees)
│  │  ├─ player.{c,h}       — player position/rotation/velocity + AABB collision
│  │  ├─ camera.{c,h}       — 3D-to-2D projection (yaw+pitch+perspective divide)
│  │  ├─ rasterizer.{c,h}   — z-buffered affine-textured quad rasterizer
│  │  ├─ block_render.{c,h} — walk world, draw exposed cube faces
│  │  └─ ui.{c,h}           — crosshair, hotbar, debug overlay
│  └─ demo/
│     └─ demo.{c,h}         — CPCraft prototype main loop
└─ test/
   ├─ native_test.c         — host-side test harness (run with `make test`)
   └─ sdk-stub/             — stub SDK headers for the host test harness
```

## Why this design

QBos07's
[Render-Display-Benchmark](https://github.com/QBos07/Render-Display-Benchmark)
measured every reasonable way to push pixels to the ClassPad LCD. The
headline result:

| Mode                                 | Ticks / full frame |
| ------------------------------------ | ------------------ |
| Pixel+Line + YRAM buffer + 2x upscale | **~140 000**       |
| Line       + YRAM buffer + 2x upscale | ~199 000           |
| Full       + YRAM buffer + 2x upscale | ~197 000           |
| Full       + plain CPU                | ~470 000           |
| Full       + DMAC                     | ~724 000           |

Three non-obvious lessons that drive this engine's design:

1. **DMA is SLOWER than CPU on this LCD.** The LCD data port sits on a
   slow peripheral bus; the CPU can stream writes through the store
   queue and pipeline them, while the DMAC stalls between every
   transfer. So we DON'T use DMAC for blitting, period.

2. **A per-line YRAM buffer wins.** YRAM (`.oc_mem.y` section, ~8 KiB on
   the ClassPad) is on a fast bus that doesn't compete with VRAM reads.
   Rendering one scanline into YRAM, then streaming it to the LCD, lets
   the CPU pipeline the next line's render with the previous line's
   refresh. With two pools we double-buffer the render/refresh so
   there's no stall between lines.

3. **2x upscaling is essentially free AND cuts render cost 4x.** We
   render into a 160x264 framebuffer; the blit doubles each pixel both
   horizontally and vertically when streaming to the LCD. The doubling
   loop is tiny (two `*lcd_data_port = pixel` writes per source pixel)
   and is dominated by the LCD's own write latency, so the extra write
   is hidden. Meanwhile the renderer only computes 1/4 as many pixels.

Net result: a 160x264 RGB565 framebuffer with double-buffered YRAM line
pools and a 2x-upscale blit gives us ~140k ticks/frame, leaving plenty of
headroom for actual game logic on a 14.5 MHz tick clock.

## How to build

### In the devcontainer (recommended)

1. Open the folder in VS Code with the HHK3 devcontainer.
2. `make -j` (or `Ctrl+Shift+B`).

The devcontainer provides `sh4a_nofpueb-elf-gcc` and the SDK at `/sdk/`.

### Manually

You need the HHK3 SDK built and `SDK_DIR` pointing at it:

```sh
export SDK_DIR=/path/to/hollyhock-3/sdk
make -j
```

This produces `dist/CPCraft.hh3` — copy that to the root of the calculator
(in USB storage mode) and run it from the launcher.

## How to run the host-side tests

The engine comes with a host-side test harness that stubs out the SDK
functions and verifies the framebuffer, texture, overlay, math, and input
modules work correctly. The blit routine itself can't run on host because
it talks to the SH-4A's TMU at fixed addresses, but every other code path
is exercised.

To run the tests (requires `gcc`):

```sh
make test
```

Stub SDK headers are bundled under `test/sdk-stub/` so this works out of
the box. The harness is in `test/native_test.c` — add new tests there.

## Demo controls

Once the .hh3 is running on hardware:

| Key            | Action                                          |
| -------------- | ----------------------------------------------- |
| D-pad          | Move the camera offset (scene 1 only)           |
| EXE            | Switch scene (plasma → textured grid → ...)     |
| Backspace      | Toggle the on-screen overlay (FPS, tick counts) |
| Shift + Clear  | Quit the demo (same combo as CP-Raycaster-Demo) |

The on-screen overlay shows:

- Scene name (`PLASMA` or `TEXTURED`)
- `R` = render ticks (time spent rendering into the framebuffer)
- `B` = blit/refresh ticks (time spent streaming to the LCD, recorded
  by `fb_present()`)
- `F` = frame counter
- `FPS` = estimated frames per second, computed as
  `TMU_TICKS_PER_SEC / (R + B)`

### Input system

The engine uses the ClassPad's `GetInput()` event API (not
`Input_GetKeyState()`), matching the pattern from CP-Raycaster-Demo.
This is more reliable for detecting held keys and allows the
Shift+Clear exit combo.

See `src/engine/input.h` for the full API. The key enum uses `EK_`
prefix (e.g. `EK_UP`, `EK_SHIFT`, `EK_CLEAR`) to avoid colliding with
the SDK's own `KEY_*` bitmask constants.

## Simulator

The simulator (`simulator/`) lets you run the engine on your PC without
flashing to the ClassPad. It implements the LCD protocol in software
(an ILI9341-style GRAM with `LCD_SetDrawingBounds` / `LCD_SendCommand`
/ pixel writes) and renders to an SDL2 window.

```sh
make sim       # build native simulator (Linux/macOS/Windows)
make sim-run   # build and run
make sim-web   # build web version (requires emcc)
make sim-clean # clean simulator build artifacts
```

The simulator's SDK stubs live in `simulator/include/sdk/`. They define
the same function pointers as the real SDK, but the implementations
live in `simulator/src/simulator.cpp` and talk to SDL2 instead of
hardware.

Keyboard mapping (host → ClassPad):

| Host key        | ClassPad key   |
| --------------- | -------------- |
| Arrow keys      | D-pad          |
| Enter / Space   | EXE            |
| Backspace       | Backspace      |
| Escape          | Power/Clear    |
| Left/Right Shift| Shift          |
| 0-9             | Number keys    |
| X / Y / Z       | X / Y / Z keys |

The GitHub workflow at `.github/workflows/simulator.yml` builds the
simulator for Linux, Windows, and Web on every push. The web version
is deployed to GitHub Pages.

## Current state — CPCraft prototype

The engine now has a full voxel-world renderer. Walking around the
generated world works:

- **World**: 64×32×64 blocks, hash-based heightmap terrain with grass
  tops, dirt just below, stone deeper down. Trees are sprinkled in
  (wood trunk + leaves canopy).
- **Player**: float position, yaw/pitch, AABB collision against the
  world. Gravity + jump.
- **Camera**: 3D-to-2D projection with 70° FOV, perspective divide.
- **Renderer**: walks every block, finds exposed faces (neighbor is
  air/transparent), backface-culls, projects the 4 corners, and
  rasterizes as a textured quad through the z-buffer.
- **HUD**: crosshair at screen center, 9-slot hotbar at the bottom,
  debug overlay (FPS + render/blit ticks) at the top.

### Demo controls (minimal for prototype)

| Key             | Action                          |
| --------------- | ------------------------------- |
| D-pad up/down   | Look up/down (pitch)            |
| D-pad left/right| Turn left/right (yaw)           |
| EXE (hold)      | Walk forward                    |
| Backspace (hold)| Walk backward                  |
| EXE + Backspace | Jump (both held)                |
| Shift + Clear   | Quit (same combo as CP-Raycaster) |

The control scheme is intentionally minimal — the input module only
tracks D-pad + EXE + Backspace + Shift + Clear. Number keys (1-9) for
hotbar selection and block place/break are TODOs for the next iteration.

## What's next

The prototype is feature-complete enough to validate the rendering
pipeline. Next steps for a fuller CPCraft port:

1. **Extend `input.h`** to track number keys (1-9), OPTN, EXP, VARS —
   needed for hotbar selection, block place/break, and jump on
   separate keys.
2. **Add block place/break** using the `raycast_voxel()` function
   already in `demo.c` (it's currently unused — wire it up to EXE
   for break and Shift for place).
3. **Chunk-based world storage** — replace the flat 64×32×64 array
   with CPCraft's chunked storage so we can have infinite terrain.
4. **Meshing + frustum culling** — the current renderer walks every
   block in the world every frame. For a 64×32×64 world that's
   131 072 iterations; fine for a prototype, too slow for real
   gameplay. Chunk meshing (build a vertex list per chunk, cull
   chunks outside the view frustum) would bring this down to ~1000
   visible faces per frame.
5. **Texture loading** — replace the procedural textures with actual
   Minecraft-format textures from CPCraft's `assets/` folder.
6. **Save/load** — persist the world + player position to flash.
7. **Mobs** — port CPCraft's `updateEntitys()` and sheep/pig spawning.
8. **Inventory + crafting** — port CPCraft's `makeUI()` and
   crafting recipe system.

## License

Same as the upstream HHK3template (see `LICENSE`).
