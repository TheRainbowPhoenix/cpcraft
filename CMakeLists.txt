cmake_minimum_required(VERSION 3.15)
project(fxcraft LANGUAGES C ASM VERSION "05.00.0000")

include(GenerateG3A)
include(Fxconv)

set(SOURCES
  # Original / SDK support files
  src/fxlibc_hal.c
  src/compression.c
  src/controls.c
  src/DMA_Transfer.c
  src/fastBdisp.c
  src/lz4.c
  src/tlsf.c

  # Refactored core
  src/main.c
  src/math_utils.c
  src/delta_time.c
  src/collision.c
  src/block_interaction.c
  src/block_use.c
  src/save.c
  src/misc.c
  src/game.c
  src/render_object.c

  # Rendering
  src/render/drawing.c
  src/render/skybox.c
  src/render/triangles.c
  src/render/text.c
  src/render/projection.c
  src/render/effects.c
  src/render/screen_effects.c

  # World
  src/world/noise.c
  src/world/random.c
  src/world/terrain.c
  src/world/chunks.c
  src/world/redstone.c
  src/world/world_mgmt.c

  # Entities
  src/entities/entity_ai.c
  src/entities/entity_spawn.c

  # UI
  src/ui/hud.c
  src/ui/inventory.c
  src/ui/inventory_mgmt.c
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_fx
  # ...
)
set(ASSETS_cg
  # ...
)

# set(MPM 1)
if(MPM OR "${CMAKE_BUILD_TYPE}" STREQUAL "FastLoad")
  set(ALLOC_OFFSET 0x100000)
else()
  set(ALLOC_OFFSET 0x0)
endif()
configure_file(src/config.h.in include/config.h)
fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(fxcraft
  ${SOURCES}
  ${ASSETS}
  ${ASSETS_${FXSDK_PLATFORM}}
)

target_include_directories(fxcraft PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/data
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_compile_options(fxcraft PRIVATE
  -fdata-sections
  -Ofast
  -fschedule-insns
  -w
  -flto=auto
  -fno-strict-aliasing
  -DNDEBUG
  -Wno-implicit-function-declaration
  -Wno-int-conversion
  -Wno-incompatible-pointer-types
)

set(LINKER "${CMAKE_CURRENT_SOURCE_DIR}/prizm.ld")

if("${CMAKE_BUILD_TYPE}" STREQUAL "FastLoad")
  set(LINKER "${CMAKE_CURRENT_SOURCE_DIR}/fastload.ld")
endif()

target_link_options(fxcraft PRIVATE
  -L ${CMAKE_CURRENT_SOURCE_DIR}
  -T ${LINKER}
  -Wl,-print-memory-usage
  -Wl,-Map=map
  -Wl,-gc-sections
)

if(MPM)
  target_link_libraries(fxcraft PRIVATE -lcw)
else()
  target_link_libraries(fxcraft PRIVATE -lcg)
endif()

target_link_libraries(fxcraft PRIVATE -lc -lgcc)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET fxcraft OUTPUT "Minecraft-fx.g3a"
    NAME "Minecraft-fx"
    ICONS assets/icon-uns.png assets/icon-sel.png
  )
endif()
