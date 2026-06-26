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
│  │  ├─ power.h            — MSTPCR0 layout (un-gate TMU/DMAC clocks)
│  │  ├─ tmu.h              — TMU channel 1 (frame timing)
│  │  ├─ framebuffer.{c,h}  — 160x264 virtual FB + YRAM line pools
│  │  │                       + 2x-upscale CPU blit (the hot path)
│  │  ├─ input.{c,h}        — polled key state with edge detection
│  │  ├─ math_lut.{c,h}     — 8-bit sin LUT + 16.16 fixed-point sin/cos
│  │  ├─ overlay.{c,h}      — tiny 4x6 font + printf for on-screen labels
│  │  └─ texture.{c,h}      — procedural 16x16 textures + textured rect
│  └─ demo/
│     └─ demo.{c,h}         — small demo that validates the engine on hw
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
| A / EXE        | Switch scene (plasma → textured grid → ...)     |
| B / Backspace  | Toggle the on-screen overlay (FPS, tick counts) |
| Menu / Clear   | Quit the demo                                   |

The on-screen overlay shows:

- Scene name (`PLASMA` or `TEXTURED`)
- `R` = render ticks (time spent rendering into the framebuffer)
- `B` = blit/refresh ticks (time spent streaming to the LCD, recorded
  by `fb_present()`)
- `F` = frame counter
- `FPS` = estimated frames per second, computed as
  `TMU_TICKS_PER_SEC / (R + B)`

## What's next

This base is intentionally minimal. The next steps for the CPCraft port
are roughly:

1. **Add `engine/sound.{c,h}`** — wraps the SDK's serial/sound if needed.
2. **Add `engine/chunk.{c,h}`** — a 16x16x16 block storage type, lifted
   from CPCraft's chunk format but with a cleaner API.
3. **Add `engine/raycast.{c,h}`** — a floor/wall caster using the
   16.16 sin/cos tables in `math_lut.h`. CP-Raycaster-Demo's
   `draw3dField()` is a great reference.
4. **Add `engine/player.{c,h}`** — position, velocity, camera angle.
5. **Replace `demo/demo.c`** with a CPCraft main loop that uses the
   above.

Each of those can be added without touching the framebuffer / present
path — that part is done and validated.

## License

Same as the upstream HHK3template (see `LICENSE`).
