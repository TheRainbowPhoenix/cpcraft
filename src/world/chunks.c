/* src/world/chunks.c - Chunk generation, meshing, texture update, lighting */
#include "chunk_constants.h"
#include "config.h"
#include "engine.h"

void loadPerlinCaves(int chunkX, int chunkY) {
  int chunkIndex = chunkX + chunkY * TOTALCHUNKWIDTH;

  if (advancedTerrain == 2) {
    for (int x = 0; x < WIDTH; x++) {
      for (int z = 0; z < WIDTH; z++) {
        for (int y = 0; y < HEIGHT; y++) {
          if (generate_random3D(SEED, 100000, x + chunkX * WIDTH, y,
                                z + chunkY * WIDTH) < 30)
            // if(x+chunkX*WIDTH == 48 && y == 10 && z+chunkY*WIDTH == 48)
            generateWorm((x + chunkX * WIDTH) * 1000, y * 1000,
                         (z + chunkY * WIDTH) * 1000, 1,
                         generate_random3D(SEED + 1, 200, x + chunkX * WIDTH, y,
                                           z + chunkY * WIDTH) +
                             200);
        }
      }
    }
  }
}

void loadPerlin(int chunkX, int chunkY) {
  int chunkIndex = chunkX + chunkY * TOTALCHUNKWIDTH;

  if (blocksRenderd[chunkIndex] == false) {
    if (advancedTerrain == 2) {
      blocksRenderd[chunkIndex] = true;
      for (int x = 0; x < WIDTH; x++) {
        for (int z = 0; z < WIDTH; z++) {
          int maxY = GetPerlin(x + chunkX * WIDTH, z + chunkY * WIDTH);

          for (int y = 0; y < HEIGHT; y++) {
            if (y <= maxY) {
              if (y == maxY) {
                if (maxY >= 20 + TERRAINHEIGHT)
                  blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] =
                      1; // grass
                else {
                  if (trulyRandom2D(SEED, 5, x + chunkX * WIDTH,
                                    z + chunkY * WIDTH) == 1 &&
                      maxY < 18 + TERRAINHEIGHT)
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] =
                        27; // clay
                  else
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] =
                        7; // sand
                }

                if (mobs == true) {
                  if (generate_random2D(SEED, 1024, x + chunkX * WIDTH,
                                        z + chunkY * WIDTH) == 2 &&
                      maxY > 18 + TERRAINHEIGHT)
                    makePig(x + chunkX * WIDTH, y + 1, z + chunkY * WIDTH);
                  if (generate_random2D(SEED, 1024, x + chunkX * WIDTH,
                                        z + chunkY * WIDTH) == 3 &&
                      maxY > 18 + TERRAINHEIGHT)
                    makeSheep2(x + chunkX * WIDTH, y + 1, z + chunkY * WIDTH);
                }

                if (generate_random2D(SEED, 128, x + chunkX * WIDTH,
                                      z + chunkY * WIDTH) == 1 &&
                    maxY > 20 + TERRAINHEIGHT)
                  generateTree(x + chunkX * WIDTH, y + 1, z + chunkY * WIDTH,
                               generate_random2D(SEED, 4, x + chunkX * WIDTH,
                                                 z + chunkY * WIDTH) +
                                   4);

                if (generate_random2D(SEED + 1, 64, x + chunkX * WIDTH,
                                      z + chunkY * WIDTH) == 1 &&
                    maxY > 20 + TERRAINHEIGHT)
                  blocks[chunkIndex][x + z * WIDTH + (y + 1) * WIDTH * WIDTH] =
                      40; // short grass

                if (generate_random2D(SEED + 2, 128, x + chunkX * WIDTH,
                                      z + chunkY * WIDTH) == 1 &&
                    maxY > 20 + TERRAINHEIGHT)
                  blocks[chunkIndex][x + z * WIDTH + (y + 1) * WIDTH * WIDTH] =
                      22; // dandelion
              }

              if (maxY >= 20 + TERRAINHEIGHT) {
                if (y == maxY - 1 || y == maxY - 2)
                  blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] =
                      2; // dirt
              } else {
                if (y == maxY - 1 || y == maxY - 2) {
                  if (trulyRandom2D(SEED, 5, x + chunkX * WIDTH,
                                    z + chunkY * WIDTH) == 1 &&
                      maxY < 18 + TERRAINHEIGHT)
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] =
                        27; // clay
                  else
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] =
                        7; // sand
                }
              }
              if (y < maxY - 2) {
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 3;
                int ROG = generate_random3D(
                    SEED, 100000, x + chunkX * WIDTH, y,
                    z + chunkY * WIDTH); // random ore generation
                if (OrePatches == true) {
                  if (ROG < 150) // coal ore
                    makeOrePatch(x + chunkX * WIDTH, y, z + chunkY * WIDTH, 5,
                                 1, 4, 11);
                  if (ROG > 150 && ROG < 250) // iron ore
                    makeOrePatch(x + chunkX * WIDTH, y, z + chunkY * WIDTH, 4,
                                 1, 6, 12);
                  if (ROG > 250 && ROG < 300 && y < 20) // gold ore
                    makeOrePatch(x + chunkX * WIDTH, y, z + chunkY * WIDTH, 3,
                                 1, 8, 13);
                  if (ROG > 300 && ROG < 340 && y < 25) // emerald ore
                    makeOrePatch(x + chunkX * WIDTH, y, z + chunkY * WIDTH, 3,
                                 1, 20, 15);
                  if (ROG > 340 && ROG < 410 && y < 12) // diamond ore
                    makeOrePatch(x + chunkX * WIDTH, y, z + chunkY * WIDTH, 3,
                                 1, 8, 14);
                  if (ROG > 410 && ROG < 450 && y < 20) // lapiz ore
                    makeOrePatch(x + chunkX * WIDTH, y, z + chunkY * WIDTH, 3,
                                 1, 4, 16);
                } else {
                  if (ROG < 635) // coal ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 11;
                  if (ROG > 635 && ROG < 1203) // iron ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 12;
                  if (ROG > 1203 && ROG < 1205 && y < 20) // gold ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 13;
                  if (ROG > 1205 && ROG < 1210 && y < 25) // emerald ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 15;
                  if (ROG > 1210 && ROG < 1226 && y < 12) // diamond ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 14;
                  if (ROG > 1226 && ROG < 1360 && y < 20) // lapiz ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 16;
                  if (ROG > 1360 && ROG < 1860 && y < 20) // redstone ore
                    blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 53;
                }
              }

              if (y == 0)
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 9;
            }
          }

          if (maxY < 18 + TERRAINHEIGHT) {
            for (int y = maxY + 1; y <= 18; y++) {
              blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 6;
              blockData[chunkIndex * WIDTH * WIDTH * HEIGHT + x + z * WIDTH +
                        y * WIDTH * WIDTH] = 0b10000001;
            }
          }
        }
      }
    }
    if (advancedTerrain == 1) {
      blocksRenderd[chunkIndex] = true;
      for (int x = 0; x < WIDTH; x++) {
        for (int z = 0; z < WIDTH; z++) {
          int maxY = perlin2d((float)(x + chunkX * WIDTH),
                              (float)(z + chunkY * WIDTH), 0.05f, 1) *
                         8 +
                     24;

          for (int y = 0; y < HEIGHT; y++) {
            if (y <= maxY) {
              if (y == maxY) {
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 1;

                if (generate_random2D(SEED, 256, x + chunkX * WIDTH,
                                      z + chunkY * WIDTH) == 0 &&
                    maxY > 18)
                  generateTree(x + chunkX * WIDTH, y + 1, z + chunkY * WIDTH,
                               generate_random2D(SEED, 4, x + chunkX * WIDTH,
                                                 z + chunkY * WIDTH) +
                                   4);
              }
              if (y == maxY - 1 || y == maxY - 2)
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 2;
              if (y < maxY - 2)
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 3;
            }
          }
        }
      }
    }
    if (advancedTerrain == 0) {
      blocksRenderd[chunkIndex] = true;
      for (int x = 0; x < WIDTH; x++) {
        for (int z = 0; z < WIDTH; z++) {
          int maxY = 20;

          for (int y = 0; y < HEIGHT; y++) {
            if (y <= maxY) {
              if (y == 3)
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 1;

              if (y == 0 || y == 1 || y == 2)
                blocks[chunkIndex][x + z * WIDTH + y * WIDTH * WIDTH] = 2;
            }
          }
        }
      }
    }
  }
}

int chunkXMapRedstone = 0;
int chunkZMapRedstone = 0;

int cycle = 0;

// map
void generateChunkV2(int chunkX, int chunkZ) {
  if (chunkX >= 0 && chunkX <= TOTALCHUNKWIDTH && chunkZ >= 0 &&
      chunkZ <= TOTALCHUNKWIDTH) {
    int chunkIndex = chunkX + chunkZ * TOTALCHUNKWIDTH;

    int chunkX12 = chunkX * WIDTH;
    int chunkZ12 = chunkZ * WIDTH;

    int totalVerticeLength = 0;
    int totalTriangleLength = 0;

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        for (int z = 0; z < WIDTH; z++) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;

          int currentBlockIndex = blocks[chunkIndex][blockIndex];
          int currentBlockType = allBlock[currentBlockIndex].blockType;

          if (currentBlockIndex != 0) {
            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++) {
              int blockIndexR =
                  blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;
              int blockRotation =
                  blockData[blockIndexR] &
                  (blockTypes[currentBlockType].diffrentRotationAmount - 1);

              if (blockTypes[currentBlockType].doSideCheck[i] == 1) {
                int x2 = chunkX12 + x +
                         blockTypes[currentBlockType].dir[blockRotation][i].x;
                int y2 =
                    y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                int z2 = chunkZ12 + z +
                         blockTypes[currentBlockType].dir[blockRotation][i].z;

                int chunkX2 = x2 / WIDTH;
                int chunkZ2 = z2 / WIDTH;

                if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH && chunkZ2 >= 0 &&
                    chunkZ2 < TOTALCHUNKWIDTH) {
                  if (y2 >= 0 && y2 < HEIGHT) {
                    int xInChunk2 = x2 % WIDTH;
                    int zInChunk2 = z2 % WIDTH;

                    int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                    int blockIndex2 =
                        xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                    int currentBlockIndex2 = blocks[chunkIndex2][blockIndex2];
                    int currentBlockType2 =
                        allBlock[currentBlockIndex2].blockType;

                    int blockIndexR2 =
                        blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                    int blockRotation2 =
                        blockData[blockIndexR2] &
                        (blockTypes[currentBlockType2].diffrentRotationAmount -
                         1);

                    int blockSideIndex = 0;

                    if (blockTypes[currentBlockType].dir[blockRotation][i].x ==
                        1)
                      blockSideIndex = 0;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .x == -1)
                      blockSideIndex = 1;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == 1)
                      blockSideIndex = 2;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == -1)
                      blockSideIndex = 3;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == 1)
                      blockSideIndex = 4;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == -1)
                      blockSideIndex = 5;

                    if (blockTypes[currentBlockType2]
                                .otherBlocksRenderFace[blockRotation2]
                                                      [blockSideIndex] == 0 ||
                        (blockTypes[currentBlockType2].isTransprent == true &&
                         blockTypes[currentBlockType].isTransprent == false) ||
                        (blockTypes[currentBlockType2].isfluid == true &&
                         blockTypes[currentBlockType].isfluid == false)) {
                      totalVerticeLength += 4;
                      totalTriangleLength++;
                    }
                  }
                }
              } else {
                totalVerticeLength += 4;
                totalTriangleLength++;
              }
            }
          }
        }
      }
    }

    Vector3S *verticesIPCTemp;
    allObj[chunkIndex].triangles =
        tlsf_malloc(tlsf, sizeof(Vector4S) * totalTriangleLength);
    allObj[chunkIndex].normal =
        tlsf_malloc(tlsf, sizeof(Vector3B) * totalTriangleLength);
    allObj[chunkIndex].color =
        tlsf_malloc(tlsf, sizeof(unsigned char) * totalTriangleLength);
    allObj[chunkIndex].brightnes =
        tlsf_malloc(tlsf, sizeof(unsigned char) * totalTriangleLength);
    allObj[chunkIndex].textureSize =
        tlsf_malloc(tlsf, sizeof(unsigned char) * totalTriangleLength);
    verticesIPCTemp = tlsf_malloc(tlsf, sizeof(Vector3S) * totalVerticeLength);

    int currentVerticeLength = 0;
    int currentTriangleLength = 0;

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        for (int z = 0; z < WIDTH; z++) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;

          int currentBlockIndex = blocks[chunkIndex][blockIndex];
          int currentBlockType = allBlock[currentBlockIndex].blockType;

          if (currentBlockIndex != 0) {
            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++) {
              int blockIndexR =
                  blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;
              int blockRotation =
                  blockData[blockIndexR] &
                  (blockTypes[currentBlockType].diffrentRotationAmount - 1);

              if (blockTypes[currentBlockType].doSideCheck[i] == 1) {
                int x2 = chunkX12 + x +
                         blockTypes[currentBlockType].dir[blockRotation][i].x;
                int y2 =
                    y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                int z2 = chunkZ12 + z +
                         blockTypes[currentBlockType].dir[blockRotation][i].z;

                int chunkX2 = x2 / WIDTH;
                int chunkZ2 = z2 / WIDTH;

                if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH && chunkZ2 >= 0 &&
                    chunkZ2 < TOTALCHUNKWIDTH) {
                  if (y2 >= 0 && y2 < HEIGHT) {
                    int xInChunk2 = x2 % WIDTH;
                    int zInChunk2 = z2 % WIDTH;

                    int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                    int blockIndex2 =
                        xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                    int currentBlockIndex2 = blocks[chunkIndex2][blockIndex2];
                    int currentBlockType2 =
                        allBlock[currentBlockIndex2].blockType;

                    int blockIndexR2 =
                        blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                    int blockRotation2 =
                        blockData[blockIndexR2] &
                        (blockTypes[currentBlockType2].diffrentRotationAmount -
                         1);

                    int blockSideIndex = 0;

                    if (blockTypes[currentBlockType].dir[blockRotation][i].x ==
                        1)
                      blockSideIndex = 0;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .x == -1)
                      blockSideIndex = 1;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == 1)
                      blockSideIndex = 2;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == -1)
                      blockSideIndex = 3;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == 1)
                      blockSideIndex = 4;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == -1)
                      blockSideIndex = 5;

                    if (blockTypes[currentBlockType2]
                                .otherBlocksRenderFace[blockRotation2]
                                                      [blockSideIndex] == 0 ||
                        (blockTypes[currentBlockType2].isTransprent == true &&
                         blockTypes[currentBlockType].isTransprent == false) ||
                        (blockTypes[currentBlockType2].isfluid == true &&
                         blockTypes[currentBlockType].isfluid == false)) {
                      allObj[chunkIndex].triangles[currentTriangleLength].x =
                          currentVerticeLength + 0;
                      allObj[chunkIndex].triangles[currentTriangleLength].y =
                          currentVerticeLength + 1;
                      allObj[chunkIndex].triangles[currentTriangleLength].z =
                          currentVerticeLength + 2;
                      allObj[chunkIndex].triangles[currentTriangleLength].w =
                          currentVerticeLength + 3;

                      allObj[chunkIndex].normal[currentTriangleLength] =
                          blockTypes[currentBlockType].normal[blockRotation][i];
                      allObj[chunkIndex].textureSize[currentTriangleLength] =
                          blockTypes[currentBlockType].texureSize[i]; // here

                      if (renderTextured == true)
                        allObj[chunkIndex].color[currentTriangleLength] =
                            allBlock[blocks[chunkIndex][blockIndex]]
                                .textureIndex[i];
                      else
                        allObj[chunkIndex].color[currentTriangleLength] =
                            allBlock[blocks[chunkIndex][blockIndex]]
                                .noTextureColorIndex[(x + y + z) % 2][i];

                      int brightness = 0;

                      if (lighting == false)
                        brightness = skyBrightness;
                      else {
                        int lightIndex =
                            blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                        brightness = lightmap[lightIndex];
                      }

                      // if(blockTypes[currentBlockType].normal[blockRotation][i].x
                      // != 0) brightness -= 2;
                      // if(blockTypes[currentBlockType].normal[blockRotation][i].z
                      // != 0) brightness -= 4;

                      if (brightness < 0)
                        brightness = 0;

                      allObj[chunkIndex].brightnes[currentTriangleLength] =
                          brightness;

                      verticesIPCTemp[currentVerticeLength + 0].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].x]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 0].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].x]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 0].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].x]
                              .z;

                      verticesIPCTemp[currentVerticeLength + 1].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].y]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 1].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].y]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 1].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].y]
                              .z;

                      verticesIPCTemp[currentVerticeLength + 2].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].z]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 2].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].z]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 2].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].z]
                              .z;

                      verticesIPCTemp[currentVerticeLength + 3].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].w]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 3].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].w]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 3].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].w]
                              .z;

                      currentVerticeLength += 4;
                      currentTriangleLength++;
                    }
                  }
                }
              } else {
                allObj[chunkIndex].triangles[currentTriangleLength].x =
                    currentVerticeLength + 0;
                allObj[chunkIndex].triangles[currentTriangleLength].y =
                    currentVerticeLength + 1;
                allObj[chunkIndex].triangles[currentTriangleLength].z =
                    currentVerticeLength + 2;
                allObj[chunkIndex].triangles[currentTriangleLength].w =
                    currentVerticeLength + 3;

                allObj[chunkIndex].normal[currentTriangleLength] =
                    blockTypes[currentBlockType].normal[blockRotation][i];
                allObj[chunkIndex].textureSize[currentTriangleLength] =
                    blockTypes[currentBlockType].texureSize[i]; // here

                if (renderTextured == true) {
                  if (blocks[chunkIndex][blockIndex] ==
                      54) // if redstone wire color based on signal strength
                    allObj[chunkIndex].color[currentTriangleLength] =
                        allBlock[blocks[chunkIndex][blockIndex]]
                            .textureIndex[i] +
                        (blockData[blockIndexR] & 0x0F);
                  else
                    allObj[chunkIndex].color[currentTriangleLength] =
                        allBlock[blocks[chunkIndex][blockIndex]]
                            .textureIndex[i];
                } else
                  allObj[chunkIndex].color[currentTriangleLength] =
                      allBlock[blocks[chunkIndex][blockIndex]]
                          .noTextureColorIndex[(x + y + z) % 2][i];

                int brightness = 0;
                if (lighting == false)
                  brightness = skyBrightness;
                else {
                  int x2 = chunkX12 + x +
                           blockTypes[currentBlockType].dir[blockRotation][i].x;
                  int y2 =
                      y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                  int z2 = chunkZ12 + z +
                           blockTypes[currentBlockType].dir[blockRotation][i].z;

                  int chunkX2 = x2 / WIDTH;
                  int chunkZ2 = z2 / WIDTH;

                  if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH &&
                      chunkZ2 >= 0 && chunkZ2 < TOTALCHUNKWIDTH) {
                    if (y2 >= 0 && y2 < HEIGHT) {
                      if (lighting == false)
                        brightness = skyBrightness;
                      else {
                        int xInChunk2 = x2 % WIDTH;
                        int zInChunk2 = z2 % WIDTH;

                        int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                        int blockIndex2 =
                            xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                        int lightIndex =
                            blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;

                        brightness = lightmap[lightIndex];
                      }
                    }
                  }
                }

                // if(blockTypes[currentBlockType].normal[blockRotation][i].x !=
                // 0) brightness -= 2;
                // if(blockTypes[currentBlockType].normal[blockRotation][i].z !=
                // 0) brightness -= 4;

                if (brightness < 0)
                  brightness = 0;

                allObj[chunkIndex].brightnes[currentTriangleLength] =
                    brightness;

                verticesIPCTemp[currentVerticeLength + 0].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].x]
                        .x;
                verticesIPCTemp[currentVerticeLength + 0].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].x]
                        .y;
                verticesIPCTemp[currentVerticeLength + 0].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].x]
                        .z;

                verticesIPCTemp[currentVerticeLength + 1].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].y]
                        .x;
                verticesIPCTemp[currentVerticeLength + 1].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].y]
                        .y;
                verticesIPCTemp[currentVerticeLength + 1].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].y]
                        .z;

                verticesIPCTemp[currentVerticeLength + 2].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].z]
                        .x;
                verticesIPCTemp[currentVerticeLength + 2].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].z]
                        .y;
                verticesIPCTemp[currentVerticeLength + 2].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].z]
                        .z;

                verticesIPCTemp[currentVerticeLength + 3].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].w]
                        .x;
                verticesIPCTemp[currentVerticeLength + 3].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].w]
                        .y;
                verticesIPCTemp[currentVerticeLength + 3].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].w]
                        .z;

                currentVerticeLength += 4;
                currentTriangleLength++;
              }
            }
          }
        }
      }
    }

    if (compressVertices == true) {
      Vector3S positionTable[totalVerticeLength];
      char usedTable[totalVerticeLength];
      unsigned short oldToNew[totalVerticeLength];

      int newVerticeLength = 0;

      for (int i = 0; i < totalVerticeLength; i++)
        usedTable[i] = -1;

      for (int i = 0; i < totalVerticeLength; i++) {
        bool makeNewIndex = true;
        for (int j = 0; j < totalVerticeLength; j++) {
          if (usedTable[j] == -1)
            break;

          if (positionTable[j].x == verticesIPCTemp[i].x &&
              positionTable[j].y == verticesIPCTemp[i].y &&
              positionTable[j].z == verticesIPCTemp[i].z) {
            oldToNew[i] = j;
            makeNewIndex = false;
            break;
          }
        }

        if (makeNewIndex == true) {
          positionTable[newVerticeLength] = verticesIPCTemp[i];
          usedTable[newVerticeLength] = 1;
          oldToNew[i] = newVerticeLength;
          verticesIPCTemp[newVerticeLength] = verticesIPCTemp[i];
          newVerticeLength++;
        }
      }

      allObj[chunkIndex].verticesIPC =
          tlsf_malloc(tlsf, sizeof(Vector3S) * newVerticeLength);

      for (int i = 0; i < newVerticeLength; i++)
        allObj[chunkIndex].verticesIPC[i] = verticesIPCTemp[i];

      for (int i = 0; i < totalTriangleLength; i++) {
        allObj[chunkIndex].triangles[i].x =
            oldToNew[allObj[chunkIndex].triangles[i].x];
        allObj[chunkIndex].triangles[i].y =
            oldToNew[allObj[chunkIndex].triangles[i].y];
        allObj[chunkIndex].triangles[i].z =
            oldToNew[allObj[chunkIndex].triangles[i].z];
        allObj[chunkIndex].triangles[i].w =
            oldToNew[allObj[chunkIndex].triangles[i].w];
      }

      allObj[chunkIndex].sizeV = newVerticeLength;
      allObj[chunkIndex].sizeT = totalTriangleLength;
      allObj[chunkIndex].active = true;
    } else {
      allObj[chunkIndex].verticesIPC =
          tlsf_malloc(tlsf, sizeof(Vector3S) * totalVerticeLength);

      for (int i = 0; i < totalVerticeLength; i++)
        allObj[chunkIndex].verticesIPC[i] = verticesIPCTemp[i];

      allObj[chunkIndex].sizeT = totalTriangleLength;
      allObj[chunkIndex].sizeV = totalVerticeLength;
      allObj[chunkIndex].active = true;
    }

    verticesLength = 0;
    triangleLength = 0;

    for (int i = 0; i < objLength; i++) {
      if (allObj[i].active == true) {
        verticesLength += allObj[i].sizeV;
        triangleLength += allObj[i].sizeT;
      }
    }

    tlsf_free(tlsf, verticesIPCTemp);
  }
}
void updateChunkV2(int chunkX, int chunkZ) {
  if (chunkX >= 0 && chunkX <= TOTALCHUNKWIDTH && chunkZ >= 0 &&
      chunkZ <= TOTALCHUNKWIDTH) {
    int chunkIndex = chunkX + chunkZ * TOTALCHUNKWIDTH;

    tlsf_free(tlsf, allObj[chunkIndex].verticesIPC);
    tlsf_free(tlsf, allObj[chunkIndex].triangles);
    tlsf_free(tlsf, allObj[chunkIndex].color);
    tlsf_free(tlsf, allObj[chunkIndex].brightnes);
    tlsf_free(tlsf, allObj[chunkIndex].normal);
    tlsf_free(tlsf, allObj[chunkIndex].textureSize);

    allObj[chunkIndex].sizeT = 0;
    allObj[chunkIndex].sizeV = 0;
    allObj[chunkIndex].position.x = 0;
    allObj[chunkIndex].position.y = 0;
    allObj[chunkIndex].position.z = 0;
    allObj[chunkIndex].rotation.x = 0;
    allObj[chunkIndex].rotation.y = 0;
    allObj[chunkIndex].active = false;

    int chunkX12 = chunkX * WIDTH;
    int chunkZ12 = chunkZ * WIDTH;

    int totalVerticeLength = 0;
    int totalTriangleLength = 0;

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        for (int z = 0; z < WIDTH; z++) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;

          int currentBlockIndex = blocks[chunkIndex][blockIndex];
          int currentBlockType = allBlock[currentBlockIndex].blockType;

          if (currentBlockIndex != 0) {
            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++) {
              int blockIndexR =
                  blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;
              int blockRotation =
                  blockData[blockIndexR] &
                  (blockTypes[currentBlockType].diffrentRotationAmount - 1);

              if (blockTypes[currentBlockType].doSideCheck[i] == 1) {
                int x2 = chunkX12 + x +
                         blockTypes[currentBlockType].dir[blockRotation][i].x;
                int y2 =
                    y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                int z2 = chunkZ12 + z +
                         blockTypes[currentBlockType].dir[blockRotation][i].z;

                int chunkX2 = x2 / WIDTH;
                int chunkZ2 = z2 / WIDTH;

                if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH && chunkZ2 >= 0 &&
                    chunkZ2 < TOTALCHUNKWIDTH) {
                  if (y2 >= 0 && y2 < HEIGHT) {
                    int xInChunk2 = x2 % WIDTH;
                    int zInChunk2 = z2 % WIDTH;

                    int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                    int blockIndex2 =
                        xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                    int currentBlockIndex2 = blocks[chunkIndex2][blockIndex2];
                    int currentBlockType2 =
                        allBlock[currentBlockIndex2].blockType;

                    int blockIndexR2 =
                        blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                    int blockRotation2 =
                        blockData[blockIndexR2] &
                        (blockTypes[currentBlockType2].diffrentRotationAmount -
                         1);

                    int blockSideIndex = 0;

                    if (blockTypes[currentBlockType].dir[blockRotation][i].x ==
                        1)
                      blockSideIndex = 0;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .x == -1)
                      blockSideIndex = 1;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == 1)
                      blockSideIndex = 2;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == -1)
                      blockSideIndex = 3;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == 1)
                      blockSideIndex = 4;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == -1)
                      blockSideIndex = 5;

                    if (blockTypes[currentBlockType2]
                                .otherBlocksRenderFace[blockRotation2]
                                                      [blockSideIndex] == 0 ||
                        (blockTypes[currentBlockType2].isTransprent == true &&
                         blockTypes[currentBlockType].isTransprent == false) ||
                        (blockTypes[currentBlockType2].isfluid == true &&
                         blockTypes[currentBlockType].isfluid == false)) {
                      totalVerticeLength += 4;
                      totalTriangleLength++;
                    }
                  }
                }
              } else {
                totalVerticeLength += 4;
                totalTriangleLength++;
              }
            }
          }
        }
      }
    }

    Vector3S *verticesIPCTemp;
    allObj[chunkIndex].triangles =
        tlsf_malloc(tlsf, sizeof(Vector4S) * totalTriangleLength);
    allObj[chunkIndex].normal =
        tlsf_malloc(tlsf, sizeof(Vector3B) * totalTriangleLength);
    allObj[chunkIndex].color =
        tlsf_malloc(tlsf, sizeof(unsigned char) * totalTriangleLength);
    allObj[chunkIndex].brightnes =
        tlsf_malloc(tlsf, sizeof(unsigned char) * totalTriangleLength);
    allObj[chunkIndex].textureSize =
        tlsf_malloc(tlsf, sizeof(unsigned char) * totalTriangleLength);
    verticesIPCTemp = tlsf_malloc(tlsf, sizeof(Vector3S) * totalVerticeLength);

    int currentVerticeLength = 0;
    int currentTriangleLength = 0;

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        for (int z = 0; z < WIDTH; z++) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;

          int currentBlockIndex = blocks[chunkIndex][blockIndex];
          int currentBlockType = allBlock[currentBlockIndex].blockType;

          if (currentBlockIndex != 0) {
            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++) {
              int blockIndexR =
                  blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;
              int blockRotation =
                  blockData[blockIndexR] &
                  (blockTypes[currentBlockType].diffrentRotationAmount - 1);

              if (blockTypes[currentBlockType].doSideCheck[i] == 1) {
                int x2 = chunkX12 + x +
                         blockTypes[currentBlockType].dir[blockRotation][i].x;
                int y2 =
                    y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                int z2 = chunkZ12 + z +
                         blockTypes[currentBlockType].dir[blockRotation][i].z;

                int chunkX2 = x2 / WIDTH;
                int chunkZ2 = z2 / WIDTH;

                if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH && chunkZ2 >= 0 &&
                    chunkZ2 < TOTALCHUNKWIDTH) {
                  if (y2 >= 0 && y2 < HEIGHT) {
                    int xInChunk2 = x2 % WIDTH;
                    int zInChunk2 = z2 % WIDTH;

                    int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                    int blockIndex2 =
                        xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                    int currentBlockIndex2 = blocks[chunkIndex2][blockIndex2];
                    int currentBlockType2 =
                        allBlock[currentBlockIndex2].blockType;

                    int blockIndexR2 =
                        blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                    int blockRotation2 =
                        blockData[blockIndexR2] &
                        (blockTypes[currentBlockType2].diffrentRotationAmount -
                         1);

                    int blockSideIndex = 0;

                    if (blockTypes[currentBlockType].dir[blockRotation][i].x ==
                        1)
                      blockSideIndex = 0;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .x == -1)
                      blockSideIndex = 1;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == 1)
                      blockSideIndex = 2;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == -1)
                      blockSideIndex = 3;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == 1)
                      blockSideIndex = 4;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == -1)
                      blockSideIndex = 5;

                    if (blockTypes[currentBlockType2]
                                .otherBlocksRenderFace[blockRotation2]
                                                      [blockSideIndex] == 0 ||
                        (blockTypes[currentBlockType2].isTransprent == true &&
                         blockTypes[currentBlockType].isTransprent == false) ||
                        (blockTypes[currentBlockType2].isfluid == true &&
                         blockTypes[currentBlockType].isfluid == false)) {
                      allObj[chunkIndex].triangles[currentTriangleLength].x =
                          currentVerticeLength + 0;
                      allObj[chunkIndex].triangles[currentTriangleLength].y =
                          currentVerticeLength + 1;
                      allObj[chunkIndex].triangles[currentTriangleLength].z =
                          currentVerticeLength + 2;
                      allObj[chunkIndex].triangles[currentTriangleLength].w =
                          currentVerticeLength + 3;

                      allObj[chunkIndex].normal[currentTriangleLength] =
                          blockTypes[currentBlockType].normal[blockRotation][i];
                      allObj[chunkIndex].textureSize[currentTriangleLength] =
                          blockTypes[currentBlockType].texureSize[i]; // here

                      if (renderTextured == true)
                        allObj[chunkIndex].color[currentTriangleLength] =
                            allBlock[blocks[chunkIndex][blockIndex]]
                                .textureIndex[i];
                      else
                        allObj[chunkIndex].color[currentTriangleLength] =
                            allBlock[blocks[chunkIndex][blockIndex]]
                                .noTextureColorIndex[(x + y + z) % 2][i];

                      int brightness = 0;

                      if (lighting == false)
                        brightness = skyBrightness;
                      else {
                        int lightIndex =
                            blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                        brightness = lightmap[lightIndex];
                      }

                      // if(blockTypes[currentBlockType].normal[blockRotation][i].x
                      // != 0) brightness -= 2;
                      // if(blockTypes[currentBlockType].normal[blockRotation][i].z
                      // != 0) brightness -= 4;

                      if (brightness < 0)
                        brightness = 0;

                      allObj[chunkIndex].brightnes[currentTriangleLength] =
                          brightness;

                      verticesIPCTemp[currentVerticeLength + 0].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].x]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 0].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].x]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 0].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].x]
                              .z;

                      verticesIPCTemp[currentVerticeLength + 1].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].y]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 1].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].y]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 1].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].y]
                              .z;

                      verticesIPCTemp[currentVerticeLength + 2].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].z]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 2].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].z]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 2].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].z]
                              .z;

                      verticesIPCTemp[currentVerticeLength + 3].x =
                          (x + chunkX * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].w]
                              .x;
                      verticesIPCTemp[currentVerticeLength + 3].y =
                          y * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].w]
                              .y;
                      verticesIPCTemp[currentVerticeLength + 3].z =
                          (z + chunkZ * WIDTH) * 10 +
                          blockTypes[currentBlockType]
                              .vertices[blockRotation]
                                       [blockTypes[currentBlockType].faces[i].w]
                              .z;

                      currentVerticeLength += 4;
                      currentTriangleLength++;
                    }
                  }
                }
              } else {
                allObj[chunkIndex].triangles[currentTriangleLength].x =
                    currentVerticeLength + 0;
                allObj[chunkIndex].triangles[currentTriangleLength].y =
                    currentVerticeLength + 1;
                allObj[chunkIndex].triangles[currentTriangleLength].z =
                    currentVerticeLength + 2;
                allObj[chunkIndex].triangles[currentTriangleLength].w =
                    currentVerticeLength + 3;

                allObj[chunkIndex].normal[currentTriangleLength] =
                    blockTypes[currentBlockType].normal[blockRotation][i];
                allObj[chunkIndex].textureSize[currentTriangleLength] =
                    blockTypes[currentBlockType].texureSize[i]; // here

                if (renderTextured == true) {
                  if (blocks[chunkIndex][blockIndex] ==
                      54) // if redstone wire color based on signal strength
                    allObj[chunkIndex].color[currentTriangleLength] =
                        allBlock[blocks[chunkIndex][blockIndex]]
                            .textureIndex[i] +
                        (blockData[blockIndexR] & 0x0F);
                  else
                    allObj[chunkIndex].color[currentTriangleLength] =
                        allBlock[blocks[chunkIndex][blockIndex]]
                            .textureIndex[i];
                } else
                  allObj[chunkIndex].color[currentTriangleLength] =
                      allBlock[blocks[chunkIndex][blockIndex]]
                          .noTextureColorIndex[(x + y + z) % 2][i];

                int brightness = 0;
                if (lighting == false)
                  brightness = skyBrightness;
                else {
                  int x2 = chunkX12 + x +
                           blockTypes[currentBlockType].dir[blockRotation][i].x;
                  int y2 =
                      y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                  int z2 = chunkZ12 + z +
                           blockTypes[currentBlockType].dir[blockRotation][i].z;

                  int chunkX2 = x2 / WIDTH;
                  int chunkZ2 = z2 / WIDTH;

                  if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH &&
                      chunkZ2 >= 0 && chunkZ2 < TOTALCHUNKWIDTH) {
                    if (y2 >= 0 && y2 < HEIGHT) {
                      if (lighting == false)
                        brightness = skyBrightness;
                      else {
                        int xInChunk2 = x2 % WIDTH;
                        int zInChunk2 = z2 % WIDTH;

                        int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                        int blockIndex2 =
                            xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                        int lightIndex =
                            blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;

                        brightness = lightmap[lightIndex];
                      }
                    }
                  }
                }

                // if(blockTypes[currentBlockType].normal[blockRotation][i].x !=
                // 0) brightness -= 2;
                // if(blockTypes[currentBlockType].normal[blockRotation][i].z !=
                // 0) brightness -= 4;

                if (brightness < 0)
                  brightness = 0;

                allObj[chunkIndex].brightnes[currentTriangleLength] =
                    brightness;

                verticesIPCTemp[currentVerticeLength + 0].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].x]
                        .x;
                verticesIPCTemp[currentVerticeLength + 0].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].x]
                        .y;
                verticesIPCTemp[currentVerticeLength + 0].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].x]
                        .z;

                verticesIPCTemp[currentVerticeLength + 1].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].y]
                        .x;
                verticesIPCTemp[currentVerticeLength + 1].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].y]
                        .y;
                verticesIPCTemp[currentVerticeLength + 1].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].y]
                        .z;

                verticesIPCTemp[currentVerticeLength + 2].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].z]
                        .x;
                verticesIPCTemp[currentVerticeLength + 2].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].z]
                        .y;
                verticesIPCTemp[currentVerticeLength + 2].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].z]
                        .z;

                verticesIPCTemp[currentVerticeLength + 3].x =
                    (x + chunkX * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].w]
                        .x;
                verticesIPCTemp[currentVerticeLength + 3].y =
                    y * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].w]
                        .y;
                verticesIPCTemp[currentVerticeLength + 3].z =
                    (z + chunkZ * WIDTH) * 10 +
                    blockTypes[currentBlockType]
                        .vertices[blockRotation]
                                 [blockTypes[currentBlockType].faces[i].w]
                        .z;

                currentVerticeLength += 4;
                currentTriangleLength++;
              }
            }
          }
        }
      }
    }

    if (compressVertices == true) {
      Vector3S positionTable[totalVerticeLength];
      char usedTable[totalVerticeLength];
      unsigned short oldToNew[totalVerticeLength];

      int newVerticeLength = 0;

      for (int i = 0; i < totalVerticeLength; i++)
        usedTable[i] = -1;

      for (int i = 0; i < totalVerticeLength; i++) {
        bool makeNewIndex = true;
        for (int j = 0; j < totalVerticeLength; j++) {
          if (usedTable[j] == -1)
            break;

          if (positionTable[j].x == verticesIPCTemp[i].x &&
              positionTable[j].y == verticesIPCTemp[i].y &&
              positionTable[j].z == verticesIPCTemp[i].z) {
            oldToNew[i] = j;
            makeNewIndex = false;
            break;
          }
        }

        if (makeNewIndex == true) {
          positionTable[newVerticeLength] = verticesIPCTemp[i];
          usedTable[newVerticeLength] = 1;
          oldToNew[i] = newVerticeLength;
          verticesIPCTemp[newVerticeLength] = verticesIPCTemp[i];
          newVerticeLength++;
        }
      }

      allObj[chunkIndex].verticesIPC =
          tlsf_malloc(tlsf, sizeof(Vector3S) * newVerticeLength);

      for (int i = 0; i < newVerticeLength; i++)
        allObj[chunkIndex].verticesIPC[i] = verticesIPCTemp[i];

      for (int i = 0; i < totalTriangleLength; i++) {
        allObj[chunkIndex].triangles[i].x =
            oldToNew[allObj[chunkIndex].triangles[i].x];
        allObj[chunkIndex].triangles[i].y =
            oldToNew[allObj[chunkIndex].triangles[i].y];
        allObj[chunkIndex].triangles[i].z =
            oldToNew[allObj[chunkIndex].triangles[i].z];
        allObj[chunkIndex].triangles[i].w =
            oldToNew[allObj[chunkIndex].triangles[i].w];
      }

      allObj[chunkIndex].sizeV = newVerticeLength;
      allObj[chunkIndex].sizeT = totalTriangleLength;
      allObj[chunkIndex].active = true;
    } else {
      allObj[chunkIndex].verticesIPC =
          tlsf_malloc(tlsf, sizeof(Vector3S) * totalVerticeLength);

      for (int i = 0; i < totalVerticeLength; i++)
        allObj[chunkIndex].verticesIPC[i] = verticesIPCTemp[i];

      allObj[chunkIndex].sizeT = totalTriangleLength;
      allObj[chunkIndex].sizeV = totalVerticeLength;
      allObj[chunkIndex].active = true;
    }

    verticesLength = 0;
    triangleLength = 0;

    for (int i = 0; i < objLength; i++) {
      if (allObj[i].active == true) {
        verticesLength += allObj[i].sizeV;
        triangleLength += allObj[i].sizeT;
      }
    }

    tlsf_free(tlsf, verticesIPCTemp);
  }
}
void updateChunkTexturesV2(int chunkX, int chunkZ) {
  if (chunkX >= 0 && chunkX <= TOTALCHUNKWIDTH && chunkZ >= 0 &&
      chunkZ <= TOTALCHUNKWIDTH) {
    int chunkIndex = chunkX + chunkZ * TOTALCHUNKWIDTH;

    int totalTriangleLength = allObj[chunkIndex].sizeT;
    int currentTriangleLength = 0;

    int chunkX12 = chunkX * WIDTH;
    int chunkZ12 = chunkZ * WIDTH;

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        for (int z = 0; z < WIDTH; z++) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;

          int currentBlockIndex = blocks[chunkIndex][blockIndex];
          int currentBlockType = allBlock[currentBlockIndex].blockType;

          if (currentBlockIndex != 0) {
            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++) {
              int blockIndexR =
                  blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;
              int blockRotation =
                  blockData[blockIndexR] &
                  (blockTypes[currentBlockType].diffrentRotationAmount - 1);

              if (blockTypes[currentBlockType].doSideCheck[i] == 1) {
                int x2 = chunkX12 + x +
                         blockTypes[currentBlockType].dir[blockRotation][i].x;
                int y2 =
                    y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                int z2 = chunkZ12 + z +
                         blockTypes[currentBlockType].dir[blockRotation][i].z;

                int chunkX2 = x2 / WIDTH;
                int chunkZ2 = z2 / WIDTH;

                if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH && chunkZ2 >= 0 &&
                    chunkZ2 < TOTALCHUNKWIDTH) {
                  if (y2 >= 0 && y2 < HEIGHT) {
                    int xInChunk2 = x2 % WIDTH;
                    int zInChunk2 = z2 % WIDTH;

                    int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                    int blockIndex2 =
                        xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                    int currentBlockIndex2 = blocks[chunkIndex2][blockIndex2];
                    int currentBlockType2 =
                        allBlock[currentBlockIndex2].blockType;

                    int blockIndexR2 =
                        blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                    int blockRotation2 =
                        blockData[blockIndexR2] &
                        (blockTypes[currentBlockType2].diffrentRotationAmount -
                         1);

                    int blockSideIndex = 0;

                    if (blockTypes[currentBlockType].dir[blockRotation][i].x ==
                        1)
                      blockSideIndex = 0;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .x == -1)
                      blockSideIndex = 1;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == 1)
                      blockSideIndex = 2;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == -1)
                      blockSideIndex = 3;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == 1)
                      blockSideIndex = 4;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == -1)
                      blockSideIndex = 5;

                    if (blockTypes[currentBlockType2]
                                .otherBlocksRenderFace[blockRotation2]
                                                      [blockSideIndex] == 0 ||
                        (blockTypes[currentBlockType2].isTransprent == true &&
                         blockTypes[currentBlockType].isTransprent == false) ||
                        (blockTypes[currentBlockType2].isfluid == true &&
                         blockTypes[currentBlockType].isfluid == false)) {
                      if (renderTextured == true)
                        allObj[chunkIndex].color[currentTriangleLength] =
                            allBlock[blocks[chunkIndex][blockIndex]]
                                .textureIndex[i];
                      else
                        allObj[chunkIndex].color[currentTriangleLength] =
                            allBlock[blocks[chunkIndex][blockIndex]]
                                .noTextureColorIndex[(x + y + z) % 2][i];

                      currentTriangleLength++;
                    }
                  }
                }
              } else {
                if (renderTextured == true) {
                  if (blocks[chunkIndex][blockIndex] ==
                      54) // if redstone wire color based on signal strength
                    allObj[chunkIndex].color[currentTriangleLength] =
                        allBlock[blocks[chunkIndex][blockIndex]]
                            .textureIndex[i] +
                        (blockData[blockIndexR] & 0x0F);
                  else
                    allObj[chunkIndex].color[currentTriangleLength] =
                        allBlock[blocks[chunkIndex][blockIndex]]
                            .textureIndex[i];
                } else
                  allObj[chunkIndex].color[currentTriangleLength] =
                      allBlock[blocks[chunkIndex][blockIndex]]
                          .noTextureColorIndex[(x + y + z) % 2][i];

                currentTriangleLength++;
              }
            }
          }
        }
      }
    }
  }
}
void updateChunkLightFacesV2(int chunkX, int chunkZ) {
  if (chunkX >= 0 && chunkX <= TOTALCHUNKWIDTH && chunkZ >= 0 &&
      chunkZ <= TOTALCHUNKWIDTH) {
    int chunkIndex = chunkX + chunkZ * TOTALCHUNKWIDTH;

    int chunkX12 = chunkX * WIDTH;
    int chunkZ12 = chunkZ * WIDTH;

    int currentVerticeLength = 0;
    int currentTriangleLength = 0;

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        for (int z = 0; z < WIDTH; z++) {
          int blockIndex = x + z * WIDTH + y * WIDTH * WIDTH;

          int currentBlockIndex = blocks[chunkIndex][blockIndex];
          int currentBlockType = allBlock[currentBlockIndex].blockType;

          if (currentBlockIndex != 0) {
            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++) {
              int blockIndexR =
                  blockIndex + chunkIndex * WIDTH * WIDTH * HEIGHT;
              int blockRotation =
                  blockData[blockIndexR] &
                  (blockTypes[currentBlockType].diffrentRotationAmount - 1);

              if (blockTypes[currentBlockType].doSideCheck[i] == 1) {
                int x2 = chunkX12 + x +
                         blockTypes[currentBlockType].dir[blockRotation][i].x;
                int y2 =
                    y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                int z2 = chunkZ12 + z +
                         blockTypes[currentBlockType].dir[blockRotation][i].z;

                int chunkX2 = x2 / WIDTH;
                int chunkZ2 = z2 / WIDTH;

                if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH && chunkZ2 >= 0 &&
                    chunkZ2 < TOTALCHUNKWIDTH) {
                  if (y2 >= 0 && y2 < HEIGHT) {
                    int xInChunk2 = x2 % WIDTH;
                    int zInChunk2 = z2 % WIDTH;

                    int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                    int blockIndex2 =
                        xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                    int currentBlockIndex2 = blocks[chunkIndex2][blockIndex2];
                    int currentBlockType2 =
                        allBlock[currentBlockIndex2].blockType;

                    int blockIndexR2 =
                        blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                    int blockRotation2 =
                        blockData[blockIndexR2] &
                        (blockTypes[currentBlockType2].diffrentRotationAmount -
                         1);

                    int blockSideIndex = 0;

                    if (blockTypes[currentBlockType].dir[blockRotation][i].x ==
                        1)
                      blockSideIndex = 0;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .x == -1)
                      blockSideIndex = 1;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == 1)
                      blockSideIndex = 2;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .y == -1)
                      blockSideIndex = 3;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == 1)
                      blockSideIndex = 4;
                    else if (blockTypes[currentBlockType]
                                 .dir[blockRotation][i]
                                 .z == -1)
                      blockSideIndex = 5;

                    if (blockTypes[currentBlockType2]
                                .otherBlocksRenderFace[blockRotation2]
                                                      [blockSideIndex] == 0 ||
                        (blockTypes[currentBlockType2].isTransprent == true &&
                         blockTypes[currentBlockType].isTransprent == false) ||
                        (blockTypes[currentBlockType2].isfluid == true &&
                         blockTypes[currentBlockType].isfluid == false)) {
                      int brightness = 0;

                      if (lighting == false)
                        brightness = skyBrightness;
                      else {
                        int lightIndex =
                            blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;
                        brightness = lightmap[lightIndex];
                      }

                      // if(blockTypes[currentBlockType].normal[blockRotation][i].x
                      // != 0) brightness -= 2;
                      // if(blockTypes[currentBlockType].normal[blockRotation][i].z
                      // != 0) brightness -= 4;

                      if (brightness < 0)
                        brightness = 0;

                      allObj[chunkIndex].brightnes[currentTriangleLength] =
                          brightness;

                      currentVerticeLength += 4;
                      currentTriangleLength++;
                    }
                  }
                }
              } else {
                int brightness = 0;
                if (lighting == false)
                  brightness = skyBrightness;
                else {
                  int x2 = chunkX12 + x +
                           blockTypes[currentBlockType].dir[blockRotation][i].x;
                  int y2 =
                      y + blockTypes[currentBlockType].dir[blockRotation][i].y;
                  int z2 = chunkZ12 + z +
                           blockTypes[currentBlockType].dir[blockRotation][i].z;

                  int chunkX2 = x2 / WIDTH;
                  int chunkZ2 = z2 / WIDTH;

                  if (chunkX2 >= 0 && chunkX2 < TOTALCHUNKWIDTH &&
                      chunkZ2 >= 0 && chunkZ2 < TOTALCHUNKWIDTH) {
                    if (y2 >= 0 && y2 < HEIGHT) {
                      if (lighting == false)
                        brightness = skyBrightness;
                      else {
                        int xInChunk2 = x2 % WIDTH;
                        int zInChunk2 = z2 % WIDTH;

                        int chunkIndex2 = chunkX2 + chunkZ2 * TOTALCHUNKWIDTH;
                        int blockIndex2 =
                            xInChunk2 + zInChunk2 * WIDTH + y2 * WIDTH * WIDTH;

                        int lightIndex =
                            blockIndex2 + chunkIndex2 * WIDTH * WIDTH * HEIGHT;

                        brightness = lightmap[lightIndex];
                      }
                    }
                  }
                }

                // if(blockTypes[currentBlockType].normal[blockRotation][i].x !=
                // 0) brightness -= 2;
                // if(blockTypes[currentBlockType].normal[blockRotation][i].z !=
                // 0) brightness -= 4;

                if (brightness < 0)
                  brightness = 0;

                allObj[chunkIndex].brightnes[currentTriangleLength] =
                    brightness;

                currentVerticeLength += 4;
                currentTriangleLength++;
              }
            }
          }
        }
      }
    }
  }
}
