/* include/engine.h */
/* include/engine.h
 *
 * cpcraft engine — master header.
 *
 * Pull in all type definitions, globals, and data tables.
 * Each .c file includes this to get everything it needs.
 *
 * Module layout:
 *
 *   include/
 *     types.h          - All typedefs (Vector2/3/4, item, entity, block, etc.)
 *     globals.h        - All global variables (player pos, render state, etc.)
 *     engine.h         - This file (master include)
 *
 *   data/
 *     sky_colors.h     - Sky color table (16 brightness levels)
 *     skybox_colors.h  - Skybox gradient (16 x 128 entries)
 *     item_icons.h     - Item icon bitmaps (100 items x 26x26)
 *     logo_buttons.h   - Minecraft logo + UI button bitmaps
 *     ascii_font.h     - ASCII font bitmap (256 chars) + char widths
 *     ui_elements.h    - Hotbar, hearts, recipe selector bitmaps
 *     palettes.h       - Inventory palettes + allColors table
 *     block_types.h    - Block type definitions (19 types: vertices, faces, normals)
 *     items.h          - Item definitions (100 items with name, stack, tool, etc.)
 *     blocks.h         - Block property definitions (73 blocks: destroy time, drops, etc.)
 *     misc_data.h      - Missing icon + swap helper
 *
 *   src/
 *     main.c           - Entry point (main)
 *     math_utils.c     - Math: arctan, sqrt, sin/cos, vectors, distance
 *     delta_time.c     - Frame timing
 *     collision.c      - Player collision (blocks, water, lava, doors)
 *     block_interaction.c - Block reach, destroy, place, highlight
 *     block_use.c      - Chest data + useBlock interactions
 *     save.c           - Save/load (world, chunks, chests, entities, player, settings)
 *     misc.c           - Performance log, map view, entity reach
 *     game.c           - start_program + update (chunk loading)
 *     render_object.c  - Main game loop (renderObject)
 *
 *   src/render/
 *     drawing.c        - Screen clear, sprite copy helpers
 *     skybox.c         - Skybox construction
 *     triangles.c      - Triangle/line rasterizers (textured, solid, transparent)
 *     text.c           - Text rendering
 *     projection.c     - Particle rendering, 3D-to-2D projection, object move/rotate
 *     effects.c        - Crosshair + block destruction rendering
 *     screen_effects.c - Underwater + menu visual effects
 *
 *   src/world/
 *     noise.c          - Perlin noise
 *     random.c         - Random number generators + quicksort
 *     terrain.c        - Tree, ore patch, cave worm generation
 *     chunks.c         - Chunk generation, meshing, texture update, lighting
 *     redstone.c       - Redstone map update logic
 *     world_mgmt.c     - Chunk/entity lifecycle, lighting updates
 *
 *   src/entities/
 *     entity_ai.c      - Entity update logic (sheep, pig, item AI)
 *     entity_spawn.c   - makeItem, makeSheep, makePig, load variants
 *
 *   src/ui/
 *     hud.c            - Health bar, Minecraft font, hotbar, item details
 *     inventory.c      - Full inventory, crafting, chest UI
 *     inventory_mgmt.c - Add/remove items from hotbar and inventory
 */
#pragma once

/* Type definitions */
#include "types.h"

/* Global variables */
#include "globals.h"

/* Data tables */

/* External function declarations (defined in the .c files) */

/* math_utils.c */
float arctan_approx(float y, float x);
float abs_(float x);
int abs_int(int x);
int clamp(int value, int min, int max);
float dotVec3(Vector3 a, Vector3 b);
Vector3 crossProduct(Vector3 a, Vector3 b);
int calculateDistance(int x1, int y1, int x2, int y2);
int calculateDistance3D(int x1, int y1, int z1, int x2, int y2, int z2);
void initializeTables(void);
float fastCosine(float angle);
float fastSine(float angle);
Vector3 CalculateRightVector(float rotationX_, float rotationY_);
Vector3 CalculateForwardVector(float rotationX_, float rotationY_);

/* drawing.c */
void clear(color_t color);
void clearAll(color_t color);
void CopySprite(color_t* sprite, int x, int y, int width, int height, color_t transparentColor);
void CopySpriteHeart(color_t* sprite, int x, int y, int width, int height, color_t transparentColor, int maxX);
void CopySpriteNbitMasked(const unsigned char* data, int x, int y, int width, int height, const color_t* palette, color_t maskColor, unsigned int bitwidth);
void CopySpriteIcon(color_t* sprite, int x, int y, int width, int height, color_t transparentColor);

/* skybox.c */
void makeSkyBox(void);

/* triangles.c */
void renderLine(int x0, int x1, int y0, int y1);
void renderLinePX2(int x0, int x1, int y0, int y1);
void renderTriangleFast(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth, const int texHeight, const int textureIndex);
void renderTriangle(Vector2S* vertexA, Vector2S* vertexB, Vector2S* vertexC, const color_t color, const unsigned short depth, int brightness);
void renderTriangleTexturedNBT(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC, const unsigned short depth, const int texWidth_, const int texHeight_, const color_t *textureAdress);
void renderSkybox(Vector2S vertexA, Vector2S vertexB, Vector2S vertexC, Vector2S texA, Vector2S texB, Vector2S texC);

/* text.c */
void renderText(int x, int y, const char *text);
void renderTextColord(int x, int y, const char *text, color_t color);
void renderTextLargeColord(int x, int y, const char *text, color_t color, color_t backGroundColor);
void renderTextUpdate(int x, int y, int stop, const char *text);

/* projection.c */
void renderParticle(float size, int mode, color_t color, screenPoint screenPixel, int fallof);
screenPoint pointToScreenCorrdinates(int xIn, int yIn, int zIn);
void moveObject(int index, float plusX, float plusY, float plusZ);
void addRotateObject(int indexGet, int indexSet, Vector3I center, float pitch, float yaw);
void setRotateObject(int indexGet, int indexSet, Vector3I center, float pitch, float yaw);

/* noise.c */
int noise2I(int x, int y);
int noise2dI(int x, int y);
int getHeight(int x, int z);
int GetPerlin(int x, int z);

/* random.c */
unsigned int generate_random(unsigned int seed, int range);
unsigned int generate_random2D(unsigned int seed, int range, int x, int y);
unsigned int generate_random3D(unsigned int seed, int range, int x, int y, int z);
void Quick_SortInt(int *arr, int left, int right);

/* delta_time.c */
void deltaTimeCalculate(void);

/* save.c */
void saveWorldData(int world);
void loadWorldData(int world);
void savegameData(void);
void loadgameData(void);
void saveSettings(void);
void loadSettings(void);
void saveChunk(int index, int world, bool exists);
void loadChunk(int index, int world);

/* collision.c */
bool checkCollision(float positionFX, float positionFY, float positionFZ);
bool checkIfInWater(float positionFX, float positionFY, float positionFZ);
bool checkIfInLava(float positionFX, float positionFY, float positionFZ);

/* block_interaction.c */
void updateBlockReach(void);
void destroyBlock(void);
void placeBlock(int itemType);
void highlightBlock(Vector3I RB);

/* block_use.c */
void createChestData(int x, int y, int z);
void useBlock(void);

/* misc.c */
void updatePerformanceLog(void);
void viewMap(void);
void updateEntityReach(void);

/* game.c */
void start_program(void);
void update(void);

/* render_object.c */
void renderObject(void);

/* world/terrain.c */
void generateTree(int x, int y, int z, int hight);
void makeOrePatch(int xMain, int yMain, int zMain, int size, int chance, int lowerReturns, int blockType);
void generateWorm(int x_, int y_, int z_, int distancePerStep, int length);

/* world/chunks.c */
void generateChunkV2(int chunkX, int chunkZ);
void updateChunkV2(int chunkX, int chunkZ);
void updateChunkTexturesV2(int chunkX, int chunkZ);
void updateChunkLightFacesV2(int chunkX, int chunkZ);

/* world/redstone.c */
void updateMap(void);

/* world/world_mgmt.c */
void ResetAllChunks(void);
void clearAllChunks(void);
void clearAllEntitys(void);
void loadAllEntitys(void);
void loadInChunks(void);
void updateAllChunkLighting(void);
void loadLighting2(void);

/* render/effects.c */
void renderCorsAir(int renderBlockOutline);
void renderBlockDestruction(float timeDone, float totalTime);

/* render/screen_effects.c */
void renderUnderWaterEffect(void);
void renderMenuEffect(void);

/* ui/hud.c */
void drawHp(int x, int y, int barHeight, int barWidth, int hp, int maxHp);
void drawMinecraftFont(int x1, int x2, int y, bool center, int textLength, char text[], color_t textColor, color_t textColorShadow);
void itemDetails(int posX, int posY, char text[]);
void makeHealthBar(void);
void makeHotbar(int selected);

/* ui/inventory.c */
void makeUI(void);
void swapItems(int cursorX1, int cursorY1, int cursorX2, int cursorY2);
void swapItemsChest(int cursorX1, int cursorY1, int cursorX2, int cursorY2);

/* ui/inventory_mgmt.c */
void addItemToHotbar(int itemIndex, int amount);
void addItemToInventory(int itemIndex, int amount);
void addItemToHotbarInventory(int itemIndex, int amount);
void removeItemFromHotbar(int itemIndex, int amount);
void removeItemFromInventory(int itemIndex, int amount);
void removeItemFromInventoryHotbar(int itemIndex, int amount);
void updateItemSlected(void);

/* entities/entity_ai.c */
void updateEntitys(void);

/* entities/entity_spawn.c */
void makeItem(int posX, int posY, int posZ, float velocityX, float velocityY, float velocityZ, int itemIndex);
void makeSheep2(int posX, int posY, int posZ);
void makePig(int posX, int posY, int posZ);
void makeSheep2Load(int posX, int posY, int posZ, int slot, bool hasFur);
void makeItemLoad(int posX, int posY, int posZ, float velocityX, float velocityY, float velocityZ, int itemIndex, int slot);
void makePigLoad(int posX, int posY, int posZ, int slot);

/* utils */
color_t combineColors(color_t color1, color_t color2, int strength);
color_t combineColorsWithBrightness(color_t color, int brightness);
