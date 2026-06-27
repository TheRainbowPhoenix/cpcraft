/* src/block_interaction.c - Block reach, destroy, place, highlight */
#include "chunk_constants.h"
#include "config.h"
#include "engine.h"

void updateBlockReach() {
  Vector3I forw = {forward.x * 1000, -forward.y * 1000, -forward.z * 1000};
  Vector3I pPos = {PPosX * 10000, PPosY * 10000, PPosZ * 10000};

  Vector3I lastPointRounded;

  removeBlock.x = -1;
  removeBlock.y = -1;
  removeBlock.z = -1;

  addBlock.x = -1;
  addBlock.y = -1;
  addBlock.z = -1;

  for (int i = 0; i < 50; i += 1) {
    int pointX = (forw.x * i + pPos.x) / 10000;
    int pointY = (forw.y * i + pPos.y) / 10000;
    int pointZ = (forw.z * i + pPos.z) / 10000;

    if ((forw.y * i + pPos.y) / 100 - pointY * 100 > 50)
      isTopOfBlock = true;
    else
      isTopOfBlock = false;

    if (pointX != lastPointRounded.x || pointY != lastPointRounded.y ||
        pointZ != lastPointRounded.z) {
      int pointInChunkX = pointX % WIDTH;
      int pointInChunkY = pointY;
      int pointInChunkZ = pointZ % WIDTH;

      if (pointY < HEIGHT) {
        int chunkIndex = (pointX / WIDTH) + (pointZ / WIDTH) * TOTALCHUNKWIDTH;

        int blockIndex =
            blocks[chunkIndex][pointInChunkX + pointInChunkZ * WIDTH +
                               pointInChunkY * WIDTH * WIDTH];
        if (blockIndex != 0 && allBlock[blockIndex].blockType != 2) {
          removeBlock.x = pointX;
          removeBlock.y = pointY;
          removeBlock.z = pointZ;

          addBlock = lastPointRounded;

          break;
        }
      }

      lastPointRounded.x = pointX;
      lastPointRounded.y = pointY;
      lastPointRounded.z = pointZ;
    }
  }
}
void destroyBlock() {
  updateBlockReach();

  if (removeBlock.x != -1) {
    int chunkX = removeBlock.x / WIDTH;
    int chunkY = removeBlock.z / WIDTH;
    int currentChunk_ = chunkX + chunkY * TOTALCHUNKWIDTH;

    int blockIndex = (removeBlock.x % WIDTH) + (removeBlock.z % WIDTH) * WIDTH +
                     (removeBlock.y) * WIDTH * WIDTH;

    if (blocks[currentChunk_][blockIndex] != 9) {
      if (survival == true) {
        int dropType = allBlock[blocks[currentChunk_][blockIndex]].drops;
        int toolType = allBlock[blocks[currentChunk_][blockIndex]].toolToBeak;
        int toolTypeLevel =
            allBlock[blocks[currentChunk_][blockIndex]].toolLevelToGetItem;

        int realBlockSelected = 0;
        if (blockSelected > -1)
          realBlockSelected = blockSelected;

        int toolTypeSelected = allItem[realBlockSelected].toolType;
        int toolTypeLevelSelected = allItem[realBlockSelected].toolLevel;

        int cont = 0;

        if (toolTypeLevel == 0)
          cont = 1;

        if (toolType == toolTypeSelected) {
          if (toolTypeLevelSelected >= toolTypeLevel)
            cont = 1;
        }

        if (allItem[realBlockSelected].breaksIn > 0) {
          if (hotbarBlockHp[currentSlot] == 0) {
            hotbarBlockAmount[currentSlot] = 0;
            hotbarBlockTypes[currentSlot] = -1;

            blockSelected = -1;

            return; // tool is brokey
          }

          hotbarBlockHp[currentSlot]--;
        }

        if (dropType == -1 || toolType == -1)
          cont = 0;

        if (cont == 1) {
          makeItem((int)(removeBlock.x * 10) + 4, (int)(removeBlock.y * 10) + 4,
                   (int)(removeBlock.z * 10) + 4, 0, 0, 0, dropType);
          if (dropType == 66) // if wheat drop 2 seed
          {
            makeItem((int)(removeBlock.x * 10) + 4,
                     (int)(removeBlock.y * 10) + 4,
                     (int)(removeBlock.z * 10) + 4, 0, 0, 0, 65);
            makeItem((int)(removeBlock.x * 10) + 4,
                     (int)(removeBlock.y * 10) + 4,
                     (int)(removeBlock.z * 10) + 4, 0, 0, 0, 65);
          }
        }
      }

      if (blocks[currentChunk_][blockIndex] == 49 ||
          blocks[currentChunk_][blockIndex] == 59) {
        blocks[currentChunk_][blockIndex + WIDTH * WIDTH] = 0;
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT +
                  (blockIndex + WIDTH * WIDTH)] = 0;
      }
      if (blocks[currentChunk_][blockIndex] == 50 ||
          blocks[currentChunk_][blockIndex] == 60) {
        blocks[currentChunk_][blockIndex - WIDTH * WIDTH] = 0;
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT +
                  (blockIndex - WIDTH * WIDTH)] = 0;
      }
      if (allBlock[blocks[currentChunk_][blockIndex]].blockType == 13) {
        if (blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] ==
            0) {
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex -
                    WIDTH] = 0;
          blocks[currentChunk_][blockIndex - WIDTH] = 0;
        }
        if (blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] ==
            1) {
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex - 1] =
              0;
          blocks[currentChunk_][blockIndex - 1] = 0;
        }
        if (blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] ==
            2) {
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex +
                    WIDTH] = 0;
          blocks[currentChunk_][blockIndex + WIDTH] = 0;
        }
        if (blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] ==
            3) {
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex + 1] =
              0;
          blocks[currentChunk_][blockIndex + 1] = 0;
        }
      }

      if (blocks[currentChunk_][blockIndex] == 55)
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex +
                  WIDTH * WIDTH] =
            (blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex +
                       WIDTH * WIDTH] &
             0b01111111); // remove redstone torch signal

      if (blocks[currentChunk_][blockIndex] == 54 &&
          blockTypes[blocks[currentChunk_][blockIndex - WIDTH * WIDTH]]
                  .canTransferRedstoneSignal == true)
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex -
                  WIDTH * WIDTH] =
            (blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex -
                       WIDTH * WIDTH] &
             0b10111111); // remove redstone wire signal

      if (blocks[currentChunk_][blockIndex] == 58) {
        int rotation =
            blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex];
        Vector3I sidesRepeater[4] = {
            {1, 0, 0}, {0, 0, -1}, {-1, 0, 0}, {0, 0, 1}};

        int newX = removeBlock.x - sidesRepeater[rotation].x;
        int newY = removeBlock.y - sidesRepeater[rotation].y;
        int newZ = removeBlock.z - sidesRepeater[rotation].z;

        int chunkIndexNew = (newX / WIDTH) + (newZ / WIDTH) * TOTALCHUNKWIDTH;
        int blockIndexNew =
            (newX % WIDTH) + (newZ % WIDTH) * WIDTH + newY * WIDTH * WIDTH;
        int blockDataIndex =
            chunkIndexNew * WIDTH * WIDTH * HEIGHT + blockIndexNew;

        if (blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType]
                .canTransferRedstoneSignal == true)
          blockData[blockDataIndex] = (blockData[blockDataIndex] &
                                       0b01111111); // remove repeater signal
      }

      if (blocks[currentChunk_][blockIndex] == 63 ||
          allBlock[blocks[currentChunk_][blockIndex]].blockType == 18) {
        int rotation =
            ((blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] &
              0b00001110) >>
             1);
        Vector3I allSidesLeverButton[6] = {{-1, 0, 0}, {0, 0, -1}, {1, 0, 0},
                                           {0, 0, 1},  {0, -1, 0}, {0, 1, 0}};

        int newX = removeBlock.x + allSidesLeverButton[rotation].x;
        int newY = removeBlock.y + allSidesLeverButton[rotation].y;
        int newZ = removeBlock.z + allSidesLeverButton[rotation].z;

        int chunkIndexNew = (newX / WIDTH) + (newZ / WIDTH) * TOTALCHUNKWIDTH;
        int blockIndexNew =
            (newX % WIDTH) + (newZ % WIDTH) * WIDTH + newY * WIDTH * WIDTH;
        int blockDataIndex =
            chunkIndexNew * WIDTH * WIDTH * HEIGHT + blockIndexNew;

        if (blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType]
                .canTransferRedstoneSignal == true)
          blockData[blockDataIndex] = (blockData[blockDataIndex] &
                                       0b01111111); // remove repeater signal
      }

      blocks[currentChunk_][blockIndex] = 0;
      blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 0;

      if (lighting == true)
        updateLighting2(addBlock.x, addBlock.z);

      updateChunkV2(chunkX, chunkY);
    }
  }
}
void placeBlock(int itemType) {
  if (itemType != -1) {
    if (addBlock.x != -1 && addBlock.y < HEIGHT) {
      int chunkX = addBlock.x / WIDTH;
      int chunkY = addBlock.z / WIDTH;
      int currentChunk_ = chunkX + chunkY * TOTALCHUNKWIDTH;

      int blockIndex = (addBlock.x % WIDTH) + (addBlock.z % WIDTH) * WIDTH +
                       (addBlock.y) * WIDTH * WIDTH;
      int blockIndex1 = (addBlock.x % WIDTH) + (addBlock.z % WIDTH) * WIDTH +
                        (addBlock.y + 1) * WIDTH * WIDTH;

      if (itemType == 77 || itemType == 83) // check of door can be placed
      {
        if (blocks[currentChunk_][blockIndex1] != 0 &&
            blocks[currentChunk_][blockIndex1] != 6 &&
            blocks[currentChunk_][blockIndex1] != 24)
          return;
      }
      if (itemType == 78) // check if bed can be placed
      {
        Vector3I newPos = addBlock;

        if (rotationY < 45 || rotationY > 315)
          newPos.z--;
        if (rotationY > 45 && rotationY < 135)
          newPos.x++;
        if (rotationY > 135 && rotationY < 225)
          newPos.z++;
        if (rotationY > 225 && rotationY < 315)
          newPos.x--;

        int TchunkX = newPos.x / WIDTH;
        int TchunkY = newPos.z / WIDTH;
        int TcurrentChunk_ = chunkX + chunkY * TOTALCHUNKWIDTH;

        int TblockIndex = (newPos.x % WIDTH) + (newPos.z % WIDTH) * WIDTH +
                          (newPos.y) * WIDTH * WIDTH;

        if (blocks[TcurrentChunk_][TblockIndex] != 0 &&
            blocks[TcurrentChunk_][TblockIndex] != 6 &&
            blocks[TcurrentChunk_][TblockIndex] != 24)
          return;
      }

      if (survival == true) {
        hotbarBlockAmount[currentSlot]--;
        if (hotbarBlockAmount[currentSlot] == 0)
          hotbarBlockTypes[currentSlot] = -1;
      }

      if (itemType == 78) // rotate bed
      {
        blocks[currentChunk_][blockIndex] = allItem[itemType].blockId + 1;

        if (rotationY < 45 || rotationY > 315)
          blocks[currentChunk_][blockIndex - WIDTH] = allItem[itemType].blockId;
        if (rotationY > 45 && rotationY < 135)
          blocks[currentChunk_][blockIndex + 1] = allItem[itemType].blockId;
        if (rotationY > 135 && rotationY < 225)
          blocks[currentChunk_][blockIndex + WIDTH] = allItem[itemType].blockId;
        if (rotationY > 225 && rotationY < 315)
          blocks[currentChunk_][blockIndex - 1] = allItem[itemType].blockId;
      } else
        blocks[currentChunk_][blockIndex] = allItem[itemType].blockId;

      if (allBlock[allItem[itemType].blockId].blockType ==
          2) // set water and lava hight to a standart of 1 instead of full
             // block
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
            0b10000001;

      if (allBlock[allItem[itemType].blockId].blockType == 8) //
      {
        if (removeBlock.x - addBlock.x != 0)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 2;
        if (removeBlock.y - addBlock.y != 0)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 0;
        if (removeBlock.z - addBlock.z != 0)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 1;
      }
      if (allBlock[allItem[itemType].blockId].blockType == 9 ||
          allBlock[allItem[itemType].blockId].blockType == 15) //
      {
        if (rotationY > 45 && rotationY < 135)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 2;
        if (rotationY > 225 && rotationY < 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 0;
        if (rotationY < 45 || rotationY > 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 3;
        if (rotationY > 135 && rotationY < 225)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 1;
      }
      if (allBlock[allItem[itemType].blockId].blockType == 10) {
        int blockRotation = 0;

        if (rotationY < 45 || rotationY > 315)
          blockRotation = 0;
        if (rotationY > 45 && rotationY < 135)
          blockRotation = 3;
        if (rotationY > 135 && rotationY < 225)
          blockRotation = 2;
        if (rotationY > 225 && rotationY < 315)
          blockRotation = 1;

        if ((isTopOfBlock == true && removeBlock.y - addBlock.y == 0) ||
            removeBlock.y - addBlock.y > 0)
          blockRotation += 4;

        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
            blockRotation;
      }
      if (allBlock[allItem[itemType].blockId].blockType == 11) {
        if ((isTopOfBlock == true && removeBlock.y - addBlock.y == 0) ||
            removeBlock.y - addBlock.y > 0)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 1;
      }
      if (allBlock[allItem[itemType].blockId].blockType == 12) {
        blocks[currentChunk_][blockIndex] = allItem[itemType].blockId;
        blocks[currentChunk_][blockIndex1] = allItem[itemType].blockId + 1;

        if (rotationY < 45 || rotationY > 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000010;
        if (rotationY > 45 && rotationY < 135)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000001;
        if (rotationY > 135 && rotationY < 225)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000000;
        if (rotationY > 225 && rotationY < 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000011;

        if (rotationY < 45 || rotationY > 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex1] =
              0b00000010;
        if (rotationY > 45 && rotationY < 135)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex1] =
              0b00000001;
        if (rotationY > 135 && rotationY < 225)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex1] =
              0b00000000;
        if (rotationY > 225 && rotationY < 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex1] =
              0b00000011;
      }
      if (allBlock[allItem[itemType].blockId].blockType == 13) {
        if (rotationY < 45 || rotationY > 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 0;
        if (rotationY > 45 && rotationY < 135)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 3;
        if (rotationY > 135 && rotationY < 225)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 2;
        if (rotationY > 225 && rotationY < 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] = 1;

        if (rotationY < 45 || rotationY > 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex -
                    WIDTH] = 2;
        if (rotationY > 45 && rotationY < 135)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex + 1] =
              1;
        if (rotationY > 135 && rotationY < 225)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex +
                    WIDTH] = 0;
        if (rotationY > 225 && rotationY < 315)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex - 1] =
              3;
      }
      if (allBlock[allItem[itemType].blockId].blockType == 17 ||
          allBlock[allItem[itemType].blockId].blockType == 18) {
        Vector3I rotation;
        rotation.x = addBlock.x - removeBlock.x;
        rotation.y = addBlock.y - removeBlock.y;
        rotation.z = addBlock.z - removeBlock.z;

        if (rotation.x == 1)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000000;
        if (rotation.z == 1)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000010;
        if (rotation.x == -1)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000100;
        if (rotation.z == -1)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00000110;
        if (rotation.y == 1)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00001000;
        if (rotation.y == -1)
          blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
              0b00001010;
      }
      if (allItem[itemType].blockId == 70)
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
            0b10000000;
      if (allItem[itemType].blockId == 55)
        blockData[currentChunk_ * WIDTH * WIDTH * HEIGHT + blockIndex] =
            0b10000000;

      updateChunkV2(chunkX, chunkY);

      if (lighting == true) {
        updateLighting2(addBlock.x, addBlock.z);

        updateAllChunkLighting();
      }

      updateItemSlected();
    }
  }
}
void highlightBlock(Vector3I RB) {
  screenPoint SP[8]; // screen positions
  SP[0] =
      pointToScreenCorrdinates((RB.x) * 1000, (RB.y + 1) * 1000, (RB.z) * 1000);
  SP[1] = pointToScreenCorrdinates((RB.x + 1) * 1000, (RB.y + 1) * 1000,
                                   (RB.z) * 1000);
  SP[2] = pointToScreenCorrdinates((RB.x) * 1000, (RB.y + 1) * 1000,
                                   (RB.z + 1) * 1000);
  SP[3] = pointToScreenCorrdinates((RB.x + 1) * 1000, (RB.y + 1) * 1000,
                                   (RB.z + 1) * 1000);
  SP[4] = pointToScreenCorrdinates((RB.x) * 1000, (RB.y) * 1000, (RB.z) * 1000);
  SP[5] =
      pointToScreenCorrdinates((RB.x + 1) * 1000, (RB.y) * 1000, (RB.z) * 1000);
  SP[6] =
      pointToScreenCorrdinates((RB.x) * 1000, (RB.y) * 1000, (RB.z + 1) * 1000);
  SP[7] = pointToScreenCorrdinates((RB.x + 1) * 1000, (RB.y) * 1000,
                                   (RB.z + 1) * 1000);

  bool continue_ = true;

  for (int i = 0; i < 8; i++) {
    if (SP[i].x < 0 || SP[i].x > resX)
      continue_ = false;
    if (SP[i].y < 0 || SP[i].y > resY)
      continue_ = false;
  }

  if (continue_ == true) {
    if (pixelSize == 1) {
      // top
      renderLine(SP[0].x, SP[1].x, SP[0].y, SP[1].y);
      renderLine(SP[1].x, SP[3].x, SP[1].y, SP[3].y);
      renderLine(SP[2].x, SP[3].x, SP[2].y, SP[3].y);
      renderLine(SP[0].x, SP[2].x, SP[0].y, SP[2].y);

      // sides
      renderLine(SP[0].x, SP[4].x, SP[0].y, SP[4].y);
      renderLine(SP[1].x, SP[5].x, SP[1].y, SP[5].y);
      renderLine(SP[2].x, SP[6].x, SP[2].y, SP[6].y);
      renderLine(SP[3].x, SP[7].x, SP[3].y, SP[7].y);

      // bottom
      renderLine(SP[4].x, SP[5].x, SP[4].y, SP[5].y);
      renderLine(SP[5].x, SP[7].x, SP[5].y, SP[7].y);
      renderLine(SP[6].x, SP[7].x, SP[6].y, SP[7].y);
      renderLine(SP[4].x, SP[6].x, SP[4].y, SP[6].y);
    } else {
      // top
      renderLinePX2(SP[0].x, SP[1].x, SP[0].y, SP[1].y);
      renderLinePX2(SP[1].x, SP[3].x, SP[1].y, SP[3].y);
      renderLinePX2(SP[2].x, SP[3].x, SP[2].y, SP[3].y);
      renderLinePX2(SP[0].x, SP[2].x, SP[0].y, SP[2].y);

      // sides
      renderLinePX2(SP[0].x, SP[4].x, SP[0].y, SP[4].y);
      renderLinePX2(SP[1].x, SP[5].x, SP[1].y, SP[5].y);
      renderLinePX2(SP[2].x, SP[6].x, SP[2].y, SP[6].y);
      renderLinePX2(SP[3].x, SP[7].x, SP[3].y, SP[7].y);

      // bottom
      renderLinePX2(SP[4].x, SP[5].x, SP[4].y, SP[5].y);
      renderLinePX2(SP[5].x, SP[7].x, SP[5].y, SP[7].y);
      renderLinePX2(SP[6].x, SP[7].x, SP[6].y, SP[7].y);
      renderLinePX2(SP[4].x, SP[6].x, SP[4].y, SP[6].y);
    }
  }
}
void renderCorsAir(int renderBlockOutline) {
  updateBlockReach();

  if (removeBlock.x != -1 && renderBlockOutline == true)
    highlightBlock(removeBlock);

  int startX = resX / 2 - 1;
  int startY = resY / 2 - 1;
  for (int x = startX; x < startX + 2; x++) {
    for (int y = startY - 3; y < startY + 5; y++)
      *(VRAMAddress + (y * LCD_WIDTH_PX) + x) = 0xd6ba;
  }

  for (int x = startX - 3; x < startX + 5; x++) {
    for (int y = startY; y < startY + 2; y++)
      *(VRAMAddress + (y * LCD_WIDTH_PX) + x) = 0xd6ba;
  }
}
