/* src/world/terrain.c - Tree, ore patch, cave worm generation */
#include "chunk_constants.h"
#include "config.h"
#include "engine.h"

void generateTree(int x, int y, int z, int hight) {

  for (int yOnTree = hight - 2; yOnTree <= hight - 1; yOnTree++) {
    if (y + yOnTree < HEIGHT) {
      for (int xOnTree = -2; xOnTree <= 2; xOnTree++) {
        int chunkX = (x + xOnTree) / WIDTH;
        for (int zOnTree = -2; zOnTree <= 2; zOnTree++) {
          int chunkZ = (z + zOnTree) / WIDTH;
          int currentChunk = chunkX + chunkZ * TOTALCHUNKWIDTH;

          int blockIndex = ((x + xOnTree) % WIDTH) +
                           ((z + zOnTree) % WIDTH) * WIDTH +
                           (y + yOnTree) * WIDTH * WIDTH;
          if (blocks[currentChunk][blockIndex] == 0)
            blocks[currentChunk][blockIndex] = 4;
        }
      }
    }
  }

  for (int yOnTree = hight; yOnTree <= hight + 1; yOnTree++) {
    if (y + yOnTree < HEIGHT) {
      for (int xOnTree = -1; xOnTree <= 1; xOnTree++) {
        int chunkX = (x + xOnTree) / WIDTH;
        for (int zOnTree = -1; zOnTree <= 1; zOnTree++) {
          int chunkZ = (z + zOnTree) / WIDTH;
          int currentChunk = chunkX + chunkZ * TOTALCHUNKWIDTH;

          int blockIndex = ((x + xOnTree) % WIDTH) +
                           ((z + zOnTree) % WIDTH) * WIDTH +
                           (y + yOnTree) * WIDTH * WIDTH;
          if (blocks[currentChunk][blockIndex] == 0)
            blocks[currentChunk][blockIndex] = 4;
        }
      }
    }
  }

  for (int yOnTree = 0; yOnTree <= hight; yOnTree++) {
    if (y + yOnTree < HEIGHT) {
      int chunkX = x / WIDTH;
      int chunkZ = z / WIDTH;
      int currentChunk = chunkX + chunkZ * TOTALCHUNKWIDTH;

      int blockIndex =
          (x % WIDTH) + (z % WIDTH) * WIDTH + (y + yOnTree) * WIDTH * WIDTH;
      blocks[currentChunk][blockIndex] = 5;
    }
  }
}
void makeOrePatch(int xMain, int yMain, int zMain, int size, int chance,
                  int lowerReturns, int blockType) {
  int sizeHalf = size / 2;
  for (int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      for (int z = 0; z < size; z++) {
        int xH = x - sizeHalf + xMain;
        int yH = y - sizeHalf + yMain;
        int zH = z - sizeHalf + zMain;

        int distance =
            calculateDistance3D(xMain, yMain, zMain, xH, yH, zH) * lowerReturns;
        if (generate_random3D(SEED, chance + distance, xH, yH, zH) == 0) {
          int chunkX = xH / WIDTH;
          int chunkZ = zH / WIDTH;

          int xInChunk = xH % WIDTH;
          int zInChunk = zH % WIDTH;

          blocks[chunkX + chunkZ * TOTALCHUNKWIDTH]
                [xInChunk + zInChunk * WIDTH + yH * WIDTH * WIDTH] = blockType;
        }
      }
    }
  }
}
void generateWorm(int x_, int y_, int z_, int distancePerStep, int length) {
  Vector3I current;
  current.x = x_;
  current.y = y_;
  current.z = z_;
  int breakAll = 0;

  for (int i = 0; i < length; i++) {
    // Generate random direction using Perlin noise
    int dx =
        Perlin3D(current.x / 12, current.y / 12, current.z / 12) * 2 - 1000;
    int dy = Perlin3D(current.x / 12 + 10000, current.y / 12 + 10000,
                      current.z / 12 + 10000) *
                 2 -
             1000;
    int dz = Perlin3D(current.x / 12 + 10000, current.y / 12 + 10000,
                      current.z / 12 + 10000) *
                 2 -
             1000;

    // Normalize direction
    int len = integer_sqrt(dx * dx + dy * dy + dz * dz);
    if (len == 0)
      len = 1;
    dx = dx * 1000 / len;
    dy = dy * 250 / len;
    dz = dz * 1000 / len;

    // Move to the next point
    current.x += dx * distancePerStep;
    current.y += dy * distancePerStep * 4;
    current.z += dz * distancePerStep;

    Vector3I currentWorld = {current.x / 1000, current.y / 1000,
                             current.z / 1000};

    // Check boundaries
    int sizeHalf = (4 + generate_random3D(SEED, 5, currentWorld.x,
                                          currentWorld.y, currentWorld.z)) /
                   2;

    if (generate_random3D(SEED, 30, currentWorld.x, currentWorld.y,
                          currentWorld.z) == 1)
      sizeHalf = (4 + generate_random3D(SEED, 5, currentWorld.x, currentWorld.y,
                                        currentWorld.z)) /
                 2;

    for (int x = currentWorld.x - sizeHalf; x <= currentWorld.x + sizeHalf;
         x++) {
      for (int z = currentWorld.z - sizeHalf; z <= currentWorld.z + sizeHalf;
           z++) {
        int maxY = GetPerlin(x, z);

        for (int y = currentWorld.y - sizeHalf; y <= currentWorld.y + sizeHalf;
             y++) {
          int distance = calculateDistance3D(currentWorld.x, currentWorld.y,
                                             currentWorld.z, x, y, z);

          if (distance < sizeHalf) {
            if (x > 0 && x < WIDTH * TOTALCHUNKWIDTH && y > 0 &&
                y <= maxY + 2 && z > 0 && z < WIDTH * TOTALCHUNKWIDTH) {
              int chunkX = x / WIDTH;
              int chunkZ = z / WIDTH;

              int xInChunk = x % WIDTH;
              int zInChunk = z % WIDTH;

              if (y > 6)
                blocks[chunkX + chunkZ * TOTALCHUNKWIDTH]
                      [xInChunk + zInChunk * WIDTH + y * WIDTH * WIDTH] = 0;
              else {
                blocks[chunkX + chunkZ * TOTALCHUNKWIDTH]
                      [xInChunk + zInChunk * WIDTH + y * WIDTH * WIDTH] = 24;
                blockData[(chunkX + chunkZ * TOTALCHUNKWIDTH) * WIDTH * WIDTH *
                              HEIGHT +
                          xInChunk + zInChunk * WIDTH + y * WIDTH * WIDTH] =
                    0b10000001;
              }
            } else
              breakAll = 1;
          }
        }
      }
    }
    if (breakAll == 1)
      break;
  }
}
