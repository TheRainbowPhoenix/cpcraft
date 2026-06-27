/* src/block_use.c - Chest data management and block use interactions */
#include "engine.h"
#include "chunk_constants.h"


void createChestData(int x, int y, int z)
{
    int chestIndex = -1;
    for (int i = 0; i < chestAmount; i++)
    {
        if(allchest[i].used == false)
        {
            chestIndex = i;
            break;
        }
    }

    if(chestIndex != -1) //chest does not exist exit
    {
        for (int i = 0; i < 27; i++)
        {
            allchest[chestIndex].chestBlockTypes[i] = -1;
            allchest[chestIndex].chestBlockAmount[i] = 0;
        }

        allchest[chestIndex].used = true;

        allchest[chestIndex].position.x = x;
        allchest[chestIndex].position.y = y;
        allchest[chestIndex].position.z = z;
    }
}
//add a function to remove chests later

int currentChunkUpdated = 0;
float currentChunkUpdateTimer = 0;

void useBlock()
{
    updateBlockReach();

    if(removeBlock.x != -1)
    {
        int chunkX = removeBlock.x / width;
        int chunkY = removeBlock.z / width;
        int chunkIndex = chunkX + chunkY * totalChunkWidth;
        int blockIndex = (removeBlock.x % width) + (removeBlock.z % width) * width + (removeBlock.y) * width * width;
        int blockType = blocks[chunkIndex][blockIndex];

        if(blockType == 19) //open crafting table
        {
            isInCraftingTable = true;
            UIState = 16;
            return;
        }
        if(blockType == 18 && blockSelected == -1) //explode the tnt
        {
            makeExplosion(10, removeBlock.x, removeBlock.y, removeBlock.z);
            return;
        }
        if(blockType == 20) //cook item in furnace
        {
            int con = -1;
            for (int i = 0; i < furnaceRecipeAmount; i++)
            {
                if(furnaceRecipes[i].inputItem == blockSelected)
                {
                    con = i;
                    break;
                }
            }
            if(con != -1)
            {
                hotbarBlockAmount[currentSlot]--;
                if(hotbarBlockAmount[currentSlot] == 0)
                {
                    hotbarBlockAmount[currentSlot] = 0;
                    hotbarBlockTypes[currentSlot] = -1;
                    updateItemSlected();
                }
                makeItem((int)(removeBlock.x * 10) + 4, (int)(removeBlock.y * 10) + 14, (int)(removeBlock.z * 10) + 4, 0, 0, 0, furnaceRecipes[con].outputItem);
            }
            return;
        }
        if(blockType == 26) //open chest
        {
            UIState = 18;
            return;
        }
        if(blockType == 1 || blockType == 2) //use hoe to make soil
        {
            if(allItem[blockSelected].toolType == 5)
            {
                blocks[chunkIndex][blockIndex] = 30;
                updateChunkV2(chunkX, chunkY);

                if(survival == true)
                {
			        hotbarBlockHp[currentSlot]--;
                    
                    if(hotbarBlockHp[currentSlot] == 0)
			        {
			        	hotbarBlockAmount[currentSlot] = 0;
			        	hotbarBlockTypes[currentSlot] = -1;

                    	blockSelected = -1;

			        	return; //tool is brokey 
			        }
                }
                return;
            }
        }
		if(allBlock[blockType].blockType == 17) //lever
        {
			if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000001) == 0b00000000)
			blockData[chunkIndex*width*width*height + blockIndex]++;
			else
			blockData[chunkIndex*width*width*height + blockIndex]--;

			updateChunkV2(chunkX, chunkY);

            return;
        }
		if(allBlock[blockType].blockType == 18) //botton presses
        {
			if(blockType == 64)
			blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11001110)|0b00110001); //leave on for 4 redstone ticks
			if(blockType == 65)
			blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11001110)|0b00100001); //leave on for 3 redstone ticks

			updateChunkV2(chunkX, chunkY);

            return;
        }

        if(blockType == 49) //open via top door
        {
            if(((blockData[chunkIndex*width*width*height + blockIndex]&0b00000100)>>2) == 0)
            {
                if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000011) == 3)
                blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11111100));
                else
                blockData[chunkIndex*width*width*height + blockIndex]++;

                if((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b00000011) == 3)
                blockData[chunkIndex*width*width*height + blockIndex + width*width] = (blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111100);
                else
                blockData[chunkIndex*width*width*height + blockIndex + width*width]++;

                blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11111011) | 0b00000100);
                blockData[chunkIndex*width*width*height + blockIndex + width*width] = ((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111011) | 0b00000100);
            }
            else if(((blockData[chunkIndex*width*width*height + blockIndex]&0b00000100)>>2) == 1)
            {
                if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000011) == 0)
                blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11111100) | 3);
                else
                blockData[chunkIndex*width*width*height + blockIndex]--;

                if((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b00000011) == 0)
                blockData[chunkIndex*width*width*height + blockIndex + width*width] = ((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111100) | 3);
                else
                blockData[chunkIndex*width*width*height + blockIndex + width*width]--;

                blockData[chunkIndex*width*width*height + blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b11111011);
                blockData[chunkIndex*width*width*height + blockIndex + width*width] = ((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111011));
            }

            updateChunkV2(chunkX, chunkY);
            return;
        }
        if(blockType == 50) //open via bottom door
        {
            if(((blockData[chunkIndex*width*width*height + blockIndex]&0b00000100)>>2) == 0)
            {
                if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000011) == 3)
                blockData[chunkIndex*width*width*height + blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b11111100);
                else
                blockData[chunkIndex*width*width*height + blockIndex]++;

                if((blockData[chunkIndex*width*width*height + blockIndex - width*width]&0b00000011) == 3)
                blockData[chunkIndex*width*width*height + blockIndex - width*width] = (blockData[chunkIndex*width*width*height + blockIndex - width*width]&0b11111100);
                else
                blockData[chunkIndex*width*width*height + blockIndex - width*width]++;

                blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11111011) | 0b00000100);
                blockData[chunkIndex*width*width*height + blockIndex - width*width] = ((blockData[chunkIndex*width*width*height + blockIndex - width*width]&0b11111011) | 0b00000100);
            }
            else if(((blockData[chunkIndex*width*width*height + blockIndex]&0b00000100)>>2) == 1)
            {
                if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000011) == 0)
                blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11111100) | 3);
                else
                blockData[chunkIndex*width*width*height + blockIndex]--;

                if((blockData[chunkIndex*width*width*height + blockIndex - width*width]&0b00000011) == 0)
                blockData[chunkIndex*width*width*height + blockIndex - width*width] = ((blockData[chunkIndex*width*width*height + blockIndex - width*width]&0b11111100) | 3);
                else
                blockData[chunkIndex*width*width*height + blockIndex - width*width]--;

                blockData[chunkIndex*width*width*height + blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b11111011);
                blockData[chunkIndex*width*width*height + blockIndex - width*width] = (blockData[chunkIndex*width*width*height + blockIndex - width*width]&0b11111011);
            }

            updateChunkV2(chunkX, chunkY);
            return;
        }
        if(blockType == 51 || blockType == 52) //sleep in bed
        {
            if(dayNightCycle == true)
            {
                if(skyBrightness < 8)
                {
                    dayTime = 1200;
                    dayTimeChange = 0;
                    skyBrightness = 8;

                    return;
                }
            }
        }

		if(blockSelected == 94) //shear a sheep
		{
			if(entityInFrontIndex > -1)
			{
				if(entityList[entityInFrontIndex].type == 1)
				{
					if(entityList[entityInFrontIndex].hasFur == true)
					{
						entityList[entityInFrontIndex].hasFur = false;
					    allObj[entityList[entityList[entityInFrontIndex].extraEntity].locationOnAllObj].active = false;
						makeItem((int)(allObj[entityList[entityInFrontIndex].locationOnAllObj].position.x*10), (int)(allObj[entityList[entityInFrontIndex].locationOnAllObj].position.y*10), (int)(allObj[entityList[entityInFrontIndex].locationOnAllObj].position.z*10), 0, 0, 0, 95);
                        
                        if(survival == true)
                        {
                            hotbarBlockHp[currentSlot]--;

                            if(hotbarBlockHp[currentSlot] == 0)
                            hotbarBlockTypes[currentSlot] = -1;

                            if(hotbarBlockTypes[currentSlot] != -1)
                            blockSelected = hotbarBlockTypes[currentSlot];
                            else
                            blockSelected = -1;
                        }
					}
				}
			}
		}

        if(blockSelected == 66) //feed a feedable mob
        {
            if(entityInFrontIndex > -1)
			{
				if(entityList[entityInFrontIndex].type == 1 || entityList[entityInFrontIndex].type == 2)
                {
				    entityList[entityInFrontIndex].isFed = true;

                    if(survival == true)
                    {
                        hotbarBlockAmount[currentSlot] -= 1;
                        if(hotbarBlockAmount[currentSlot] == 0)
                        hotbarBlockTypes[currentSlot] = -1;

                        if(hotbarBlockTypes[currentSlot] != -1)
                        blockSelected = hotbarBlockTypes[currentSlot];
                        else
                        blockSelected = -1;
                    }
                }
			}
        }

        if(allItem[blockSelected].toolType == 7) //eating
        {
            if(hotbarBlockAmount[currentSlot] > 0)
            {
                health += allItem[blockSelected].toolLevel;

                hotbarBlockAmount[currentSlot] -= 1;
                if(hotbarBlockAmount[currentSlot] == 0)
                hotbarBlockTypes[currentSlot] = -1;

                if(health > 20)
                health = 20;
            }
        }

        if(allItem[blockSelected].toolType == 9) //placing spawn egg
        {
            if(hotbarBlockAmount[currentSlot] > 0)
            {
                if(allItem[blockSelected].toolLevel == 1)
                makeSheep2(addBlock.x, addBlock.y, addBlock.z);
                if(allItem[blockSelected].toolLevel == 2)
                makePig(addBlock.x, addBlock.y, addBlock.z);

                if(survival == true)
                {
                    hotbarBlockAmount[currentSlot] -= 1;
                    if(hotbarBlockAmount[currentSlot] == 0)
                    hotbarBlockTypes[currentSlot] = -1;

                    if(hotbarBlockTypes[currentSlot] != -1)
                    blockSelected = hotbarBlockTypes[currentSlot];
                    else
                    blockSelected = -1;
                }
            }
        }
        
        if(allItem[blockSelected].toolType == 0 && blockSelected != -1 && hotbarBlockAmount[currentSlot] > 0)
        {
            if(blockSelected == 65)
            {
                if(blockType != 30 && blockType != 31) //seeds only on soil
                return;
            }

            placeBlock(blockSelected);

            if(allItem[blockSelected].blockId == 26)
            createChestData(addBlock.x, addBlock.y, addBlock.z);
        }
    }
}
