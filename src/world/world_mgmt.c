/* src/world/world_mgmt.c - Chunk/entity lifecycle, lighting updates */
#include "chunk_constants.h"
#include "config.h"
#include "engine.h"

void changeBrightnessAllTriangles(int lighChange) {
  for (int i = 0; i < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH; i++) {
    for (int j = 0; j < allObj[i].sizeT; j++)
      allObj[i].brightnes[j] += lighChange;
  }

  if (lighting == true) {
    for (int i = 0;
         i < WIDTH * WIDTH * HEIGHT * TOTALCHUNKWIDTH * TOTALCHUNKWIDTH; i++)
      lightmap[i] += lighChange;
  }
}
void ResetAllChunks() {
  for (int i = 0; i < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH; i++) {
    if (chunk2DActive[i] == true)
      chunk2DReset[i] = true;
  }
}
void clearAllChunks() {
  for (int i = 0; i < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH; i++) {
    if (chunk2DActive[i] == true) {
      tlsf_free(tlsf, allObj[i].verticesIPC);
      tlsf_free(tlsf, allObj[i].triangles);
      tlsf_free(tlsf, allObj[i].color);
      tlsf_free(tlsf, allObj[i].brightnes);
      tlsf_free(tlsf, allObj[i].normal);
      tlsf_free(tlsf, allObj[i].textureSize);

      allObj[i].sizeT = 0;
      allObj[i].sizeV = 0;
      allObj[i].position.x = 0;
      allObj[i].position.y = 0;
      allObj[i].position.z = 0;
      allObj[i].rotation.x = 0;
      allObj[i].rotation.y = 0;
      allObj[i].active = false;

      chunk2DActive[i] = false;
    }
  }
  activeChunks = 0;
  verticesLength = 0;
  triangleLength = 0;
}
void clearAllEntitys() {
  for (int i = 0; i < entityLength; i++) {
    if (entityList[i].active == true) {
      allObj[entityList[i].locationOnAllObj].sizeT = 0;
      allObj[entityList[i].locationOnAllObj].sizeV = 0;
      allObj[entityList[i].locationOnAllObj].position.x = 0;
      allObj[entityList[i].locationOnAllObj].position.y = 0;
      allObj[entityList[i].locationOnAllObj].position.z = 0;
      allObj[entityList[i].locationOnAllObj].rotation.x = 0;
      allObj[entityList[i].locationOnAllObj].rotation.y = 0;
      allObj[entityList[i].locationOnAllObj].active = false;

      if (entityList[i].type == 3) {
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].verticesIPC);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].triangles);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].color);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].brightnes);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].normal);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].textureSize);
      } else {
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].verticesIPC);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].triangles);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].solidColor);
        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].normal);
      }

      entityList[i].active = false;
      entityList[i].extraEntity = 0;
      entityList[i].fallingSince = 0;
      entityList[i].hasFur = false;
      entityList[i].health = 0;
      entityList[i].isFed = false;
      entityList[i].locationOnAllObj = 0;
      entityList[i].moveTimer = 0;
      entityList[i].type = 0;
      entityList[i].state2 = 0;
      entityList[i].state = 0;
    }
  }

  usedEntitys = 0;
}
void loadAllEntitys() {
  for (int i = 0; i < entityLength; i++) {
    if (entityList[i].active == true) {
      if (entityList[i].type == 1)
        makeSheep2Load(entityList[i].position.x, entityList[i].position.y,
                       entityList[i].position.z, i, entityList[i].hasFur);
      if (entityList[i].type == 2)
        makePigLoad(entityList[i].position.x, entityList[i].position.y,
                    entityList[i].position.z, i);
      if (entityList[i].type == 3)
        makeItemLoad(entityList[i].position.x, entityList[i].position.y,
                     entityList[i].position.z, 0, 0, 0, entityList[i].state, i);
    }
  }
}
void loadInChunks() {
  int pChunkX = PPosX / WIDTH;
  int pChunkY = PPosZ / WIDTH;

  for (int i = 0; i < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH; i++) {
    if (abs_int(chunk2DPos[i].x - pChunkX) +
            abs_int(chunk2DPos[i].y - pChunkY) <=
        renderDistance) {
      if (verticesLength * 6 + triangleLength * 19 < 130000 ||
          isOncCG50 == true || renderDistance == 0) {
        generateChunkV2(chunk2DPos[i].x, chunk2DPos[i].y);
        activeChunks++;

        chunk2DActive[i] = true;
      } else
        renderDistance--;
    }
    loadingScreen(i, TOTALCHUNKWIDTH * TOTALCHUNKWIDTH, "Loading spawn area...",
                  21);
  }
}

// lighing
void updateAllChunkLighting() {
  for (int i = 0; i < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH; i++) {
    if (chunk2DActive[i] == true)
      updateChunkLightFacesV2(chunk2DPos[i].x, chunk2DPos[i].y);
  }
}
void resetLightMapAll() {
  memset(lightmap, 0,
         WIDTH * WIDTH * HEIGHT * TOTALCHUNKWIDTH * TOTALCHUNKWIDTH);
}
void loadLighting2() {
  for (int chunkIndex = 0; chunkIndex < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH;
       chunkIndex++) {
    for (int x = 0; x < WIDTH; x++) {
      for (int z = 0; z < WIDTH; z++) {
        int hasLight = 1;
        for (int y = HEIGHT - 1; y >= 0; y--) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;
          int lightIndex = blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;

          if (hasLight == 0)
            lightmap[lightIndex] = 1;
          else {
            bool lightGetsThrough =
                blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType]
                    .lightGetsThroughP1;
            bool lightGetsThroughHalf =
                blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType]
                    .lightGetsThroughP1Half;

            if (lightGetsThrough == true)
              lightmap[lightIndex] = skyBrightness;
            else {
              hasLight = 0;

              if (lightGetsThroughHalf == false)
                lightmap[lightIndex] = 1;
              else
                lightmap[lightIndex] = skyBrightness;
            }
          }

          if (allBlock[blocks[chunkIndex][blockIndex]].brightness != 0)
            lightmap[lightIndex] =
                allBlock[blocks[chunkIndex][blockIndex]].brightness;
        }
      }
    }
  }

  Vector3I sides[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                       {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
  for (int i = 0; i < skyBrightness - 1; i++) {
    int iM = 16 - i;
    for (int chunkX = 0; chunkX < TOTALCHUNKWIDTH; chunkX++) {
      for (int chunkZ = 0; chunkZ < TOTALCHUNKWIDTH; chunkZ++) {
        int chunkIndex = chunkX + chunkZ * TOTALCHUNKWIDTH;
        int blockIndex = 0;

        int lightChunkIndex = chunkIndex * WIDTH * WIDTH * HEIGHT;

        for (int y = 0; y < HEIGHT; y++) {
          for (int z = 0; z < WIDTH; z++) {
            for (int x = 0; x < WIDTH; x++) {
              int lightIndex = blockIndex + lightChunkIndex;

              if (lightmap[lightIndex] == iM) {
                if (blockTypes[allBlock[blocks[chunkIndex][blockIndex]]
                                   .blockType]
                        .lightGetsThroughP2 == true) {
                  int xTotal = x + chunkX * WIDTH;
                  int zTotal = z + chunkZ * WIDTH;

                  for (int j = 0; j < 6; j++) {
                    int newX = xTotal + sides[j].x;
                    int newY = y + sides[j].y;
                    int newZ = zTotal + sides[j].z;

                    if (newX > 0 && newX < WIDTH * TOTALCHUNKWIDTH &&
                        newZ > 0 && newZ < WIDTH * TOTALCHUNKWIDTH &&
                        newY > 0 && newY < HEIGHT) {
                      int newChunkX = newX / WIDTH;
                      int newChunkZ = newZ / WIDTH;

                      int newBlockX = newX % WIDTH;
                      int newBlockz = newZ % WIDTH;

                      int newBlockIndex =
                          newBlockX + newBlockz * WIDTH + newY * WIDTH * WIDTH;
                      int newChunkIndex =
                          newChunkX + newChunkZ * TOTALCHUNKWIDTH;

                      int newLightIndex = newBlockIndex + newChunkIndex *
                                                              WIDTH * WIDTH *
                                                              HEIGHT;

                      if (lightmap[newLightIndex] < (iM - 1)) {
                        if (blockTypes[allBlock[blocks[newChunkIndex]
                                                      [newBlockIndex]]
                                           .blockType]
                                    .lightGetsThroughP2 == true ||
                            blockTypes[allBlock[blocks[newChunkIndex]
                                                      [newBlockIndex]]
                                           .blockType]
                                    .lightGetsThroughP1Half == true)
                          lightmap[newLightIndex] = iM - 1;
                      }
                    }
                  }
                }
              }
              blockIndex++;
            }
          }
        }
      }
    }
  }
}
void updateChunkLighting2(int chunkX, int chunkZ) {
  int chunkIndex = chunkX + chunkZ * TOTALCHUNKWIDTH;

  for (int x = 0; x < WIDTH; x++) {
    for (int z = 0; z < WIDTH; z++) {
      int hasLight = 1;
      for (int y = HEIGHT - 1; y >= 0; y--) {
        int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;
        int lightIndex = blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;

        if (hasLight == 0)
          lightmap[lightIndex] = 1;
        else {
          bool lightGetsThrough =
              blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType]
                  .lightGetsThroughP1;
          bool lightGetsThroughHalf =
              blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType]
                  .lightGetsThroughP1Half;

          if (lightGetsThrough == true)
            lightmap[lightIndex] = skyBrightness;
          else {
            hasLight = 0;

            if (lightGetsThroughHalf == false)
              lightmap[lightIndex] = 1;
            else
              lightmap[lightIndex] = skyBrightness;
          }
        }

        if (allBlock[blocks[chunkIndex][blockIndex]].brightness != 0)
          lightmap[lightIndex] =
              allBlock[blocks[chunkIndex][blockIndex]].brightness;
      }
    }
  }

  Vector3I sides[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                       {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
  for (int i = 0; i < skyBrightness - 1; i++) {
    int iM = 16 - i;
    int blockIndex = 0;
    int lightChunkIndex = chunkIndex * WIDTH * WIDTH * HEIGHT;

    for (int y = 0; y < HEIGHT; y++) {
      for (int z = 0; z < WIDTH; z++) {
        for (int x = 0; x < WIDTH; x++) {
          int lightIndex = blockIndex + lightChunkIndex;

          if (lightmap[lightIndex] == iM) {
            if (blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType]
                    .lightGetsThroughP2 == true) {
              int xTotal = x + chunkX * WIDTH;
              int zTotal = z + chunkZ * WIDTH;

              for (int j = 0; j < 6; j++) {
                int newX = xTotal + sides[j].x;
                int newY = y + sides[j].y;
                int newZ = zTotal + sides[j].z;

                if (newX > 0 && newX < WIDTH * TOTALCHUNKWIDTH && newZ > 0 &&
                    newZ < WIDTH * TOTALCHUNKWIDTH && newY > 0 &&
                    newY < HEIGHT) {
                  int newChunkX = newX / WIDTH;
                  int newChunkZ = newZ / WIDTH;

                  int newBlockX = newX % WIDTH;
                  int newBlockz = newZ % WIDTH;

                  int newBlockIndex =
                      newBlockX + newBlockz * WIDTH + newY * WIDTH * WIDTH;
                  int newChunkIndex = newChunkX + newChunkZ * TOTALCHUNKWIDTH;

                  int newLightIndex =
                      newBlockIndex + newChunkIndex * WIDTH * WIDTH * HEIGHT;

                  if (lightmap[newLightIndex] < (iM - 1)) {
                    if (blockTypes
                                [allBlock[blocks[newChunkIndex][newBlockIndex]]
                                     .blockType]
                                    .lightGetsThroughP2 == true ||
                        blockTypes
                                [allBlock[blocks[newChunkIndex][newBlockIndex]]
                                     .blockType]
                                    .lightGetsThroughP1Half == true)
                      lightmap[newLightIndex] = iM - 1;
                  }
                }
              }
            }
          }
          blockIndex++;
        }
      }
    }
  }
}
void updateLighting2(int posX, int posZ) {
  for (int chunkX = 0; chunkX < TOTALCHUNKWIDTH; chunkX++) {
    for (int chunkZ = 0; chunkZ < TOTALCHUNKWIDTH; chunkZ++) {
      int x = chunkX * WIDTH;
      int z = chunkZ * WIDTH;

      if (abs_int(x - posX) + abs_int(z - posZ) < 16)
        updateChunkLighting2(chunkX, chunkZ);
    }
  }
}
