/* src/game.c - start_program and update (chunk loading) */
#include "chunk_constants.h"
#include "config.h"
#include "engine.h"

void start_program() {
  loadgameData();
  loadSettings();
  loadAvailableTexturePacks();
  loadTextureAssets();
  loadIconAssets();

  if (isOncCG50 == false)
    pixelSize = 2;

  resXZBuffer = resX / pixelSize;
  resYZBuffer = resY / pixelSize;

  makeStartFolder();

  for (int x = 0; x < 8; x++) // replace later with infinite terrain
  {
    for (int y = 0; y < 8; y++) {
      Vector2S pos;
      pos.x = x;
      pos.y = y;

      chunk2DPos[x + y * 8] = pos;
    }
  }

  deltaTimeCalculate();
  deltaTimeCalculate();
}
void update() {
  updateEntitys();

  if (stopChunkLoading == false) {
    int pChunkX = PPosX / WIDTH;
    int pChunkY = PPosZ / WIDTH;

    currentChunkUpdateTimer += deltaTime;

    while (currentChunkUpdateTimer >
           0.015625f * ((float)currentChunkUpdated + 1)) {
      if (currentChunkUpdated < TOTALCHUNKWIDTH * TOTALCHUNKWIDTH) {
        if (abs_int(chunk2DPos[currentChunkUpdated].x - pChunkX) +
                    abs_int(chunk2DPos[currentChunkUpdated].y - pChunkY) >
                renderDistance ||
            chunk2DReset[currentChunkUpdated] == true) {
          if (chunk2DActive[currentChunkUpdated] == true) {
            tlsf_free(tlsf, allObj[currentChunkUpdated].verticesIPC);
            tlsf_free(tlsf, allObj[currentChunkUpdated].triangles);
            tlsf_free(tlsf, allObj[currentChunkUpdated].color);
            tlsf_free(tlsf, allObj[currentChunkUpdated].brightnes);
            tlsf_free(tlsf, allObj[currentChunkUpdated].normal);
            tlsf_free(tlsf, allObj[currentChunkUpdated].textureSize);

            verticesLength -= allObj[currentChunkUpdated].sizeT;
            triangleLength -= allObj[currentChunkUpdated].sizeV;

            allObj[currentChunkUpdated].sizeT = 0;
            allObj[currentChunkUpdated].sizeV = 0;
            allObj[currentChunkUpdated].position.x = 0;
            allObj[currentChunkUpdated].position.y = 0;
            allObj[currentChunkUpdated].position.z = 0;
            allObj[currentChunkUpdated].rotation.x = 0;
            allObj[currentChunkUpdated].rotation.y = 0;
            allObj[currentChunkUpdated].active = false;

            chunk2DActive[currentChunkUpdated] = false;
            chunk2DReset[currentChunkUpdated] = false;
            activeChunks--;
          }
        }

        if (abs_int(chunk2DPos[currentChunkUpdated].x - pChunkX) +
                abs_int(chunk2DPos[currentChunkUpdated].y - pChunkY) <=
            renderDistance) {
          if (chunk2DActive[currentChunkUpdated] == false) {
            if (verticesLength * 6 + triangleLength * 19 < 130000 ||
                isOncCG50 == true || renderDistance == 0) {
              generateChunkV2(chunk2DPos[currentChunkUpdated].x,
                              chunk2DPos[currentChunkUpdated].y);
              activeChunks++;

              chunk2DActive[currentChunkUpdated] = true;
            } else
              renderDistance--;
          }
        }

        currentChunkUpdated++;
      } else
        break;
    }

    if (currentChunkUpdateTimer > 1.0f) {
      currentChunkUpdated = 0;
      currentChunkUpdateTimer = 0;
    }
  }
}
