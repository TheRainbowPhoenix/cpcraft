/* src/world/redstone.c - Redstone map update logic */
#include "engine.h"
#include "chunk_constants.h"


    doUpdateInChunk[i] = false;

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
            {
                int distance = calculateDistance3D(x, y, z, half, half, half);
                int yInMap = y - half + posY;

                if(distance < half && yInMap < height)
                {
                    int xInMap = x - half + posX;
                    int zInMap = z - half + posZ;
                    
                    int chunkX = xInMap / width;
                    int chunkY = zInMap / width;
                    int currentChunk_ = chunkX + chunkY * totalChunkWidth;
                    int blockIndex = (xInMap % width) + (zInMap % width) * width + (yInMap) * width * width;

                    if(chunk2DActive[currentChunk_] == true)
                    doUpdateInChunk[currentChunk_] = true;

                    blocks[currentChunk_][blockIndex] = 0;
                }
            }
        }
    }

    for (int y = 0; y < totalChunkWidth; y++)
    {
        for (int x = 0; x < totalChunkWidth; x++)
        {
            if(doUpdateInChunk[x + y*totalChunkWidth] == true)
            {
                updateChunkV2(x, y);

                if(lighting == true)
                {
                    updateLighting2(posX, posZ);

                    for (int i = 0; i < totalChunkWidth*totalChunkWidth; i++)
                    {
                        if(chunk2DActive[i] == true)
                        updateChunkLightFacesV2(i % totalChunkWidth, i / totalChunkWidth);
                    }
                }
            }
        }
    }
    
}
void updateMap()
{
	cycle++;
    Vector3I sidesStraight[4] = {{1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}};
    Vector3I sidesDown[4] = {{1, -1, 0}, {-1, -1, 0}, {0, -1, 1}, {0, -1, -1}};
    Vector3I sidesUp[4] = {{1, 1, 0}, {-1, 1, 0}, {0, 1, 1}, {0, 1, -1}};
    Vector3I allSides[6] = {{1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0}};

	Vector3I sidesdiagonal[4] = {{1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}, {-1, 0, -1}};
    
    Vector3I sidesRepeater[4] = {{1, 0, 0}, {0, 0, -1}, {-1, 0, 0}, {0, 0, 1}};

	Vector3I sidesRepeaterForStopSignal[4][2] = {{{0, 0, 1}, {0, 0, -1}}, {{1, 0, 0}, {-1, 0, 0}}, {{0, 0, 1}, {0, 0, -1}}, {{1, 0, 0}, {-1, 0, 0}}};
	char sidesRepeaterForStopSignalRotation[4][2] = {{3, 1}, {0, 2}, {3, 1}, {0, 2}};

    Vector3I allSidesDoor[10] = {{1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {1, 1, 0}, {-1, 1, 0}, {0, 1, 1}, {0, 1, -1}, {0, 2, 0}, {0, -1, 0}};

	Vector3I allSidesLeverButton[6] = {{-1, 0, 0}, {0, 0, -1}, {1, 0, 0}, {0, 0, 1}, {0, -1, 0}, {0, 1, 0}};

	Vector3I sidesWater[5] = {{1, 0, 0}, {-1, 0, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
	Vector3B sidesDirtToGrass[12] = {{1,-1,0}, {-1,-1,0}, {0,-1,1}, {0,-1,-1}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1}, {1,1,0}, {-1,1,0}, {0,1,1}, {0,1,-1}};
    
    char redstoneData[5760];
	char updateRedstoneData[5760];
	char redstoneBlockData[5760];
	char waterMovementChunk[5760];

	while((abs_int(chunkXMapRedstone-(int)(PPosX/width))+abs_int(chunkZMapRedstone-(int)(PPosZ/width))) > (redstoneRenderDistance-1))
	{
		chunkXMapRedstone++;
		if(chunkXMapRedstone >= 8)
		{
			chunkXMapRedstone = 0;
			chunkZMapRedstone++;
		}
		if(chunkZMapRedstone == 8)
		chunkZMapRedstone = 0;
	}

	int chunkX = chunkXMapRedstone;
	int chunkZ = chunkZMapRedstone;

	chunkXMapRedstone++;

    //for (int chunkZ = 0; chunkZ < totalChunkWidth; chunkZ++)
    //{
    //    for (int chunkX = 0; chunkX < totalChunkWidth; chunkX++)
    //    {
            int chunkIndex = chunkX + chunkZ * totalChunkWidth;
    //        if(chunk2DActive[chunkIndex] == true)
    //        {
                //char *redstoneData = 0xE5017000;
                memset(updateRedstoneData, 0, 5760);
				memset(redstoneBlockData, 0, 5760);
				memset(waterMovementChunk, 0, 5760);

                int updateChunkAtIndex = 0;
                int updateChunkAtIndexLight = 0;
                int updateChunkAtIndexFull = 0;
                int updateChunkAtIndexWater = 0;

                for (int y = height-1; y >= 0; y--)
                {
                    for (int z = 0; z < width; z++)
                    {
                        for (int x = 0; x < width; x++)
                        {
                            int blockIndex = x+z*width+y*width*width;

							if(blocks[chunkIndex][blockIndex] != 0)
							{
								//-------------------------redstone start-------------------------
								//updating lever
								if(blocks[chunkIndex][blockIndex] == 63)
								{
									int rotation = ((blockData[chunkIndex*width*width*height + blockIndex]&0b00001110)>>1);

									int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

									int newX = posX+allSidesLeverButton[rotation].x;
                        		    int newY = y+allSidesLeverButton[rotation].y;
                        		    int newZ = posZ+allSidesLeverButton[rotation].z;

                        		    int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		    int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		    int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

									if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000001) == 0b00000001)
									{
										if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true)
                                        {
                                            if(chunkIndexNew != chunkIndex)
										    blockData[blockDataIndex] = ((blockData[blockDataIndex]&0b01111111)|0b10000000);
                                            else
                                            {
                                                redstoneData[blockIndexNew] = ((blockData[blockDataIndex]&0b01111111)|0b10000000);
                                                updateRedstoneData[blockIndexNew] = 1;
                                            }
                                        }

										redstoneData[blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b01111111)|0b10000000);
                                        updateRedstoneData[blockIndex] = 1;
									}
									else
									{
										if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true)
                                        {
                                            if(chunkIndexNew != chunkIndex)
										    blockData[blockDataIndex] = (blockData[blockDataIndex]&0b01111111);
                                            else
                                            {
                                                redstoneData[blockIndexNew] = (blockData[blockDataIndex]&0b01111111);
                                                updateRedstoneData[blockIndexNew] = 1;
                                            }
                                        }

                                        redstoneData[blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b01111111);
                                        updateRedstoneData[blockIndex] = 1;
									}
								}

								//updating button
								if(allBlock[blocks[chunkIndex][blockIndex]].blockType == 18)
								{
									int rotation = ((blockData[chunkIndex*width*width*height + blockIndex]&0b00001110)>>1);

									int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

									int newX = posX+allSidesLeverButton[rotation].x;
                        		    int newY = y+allSidesLeverButton[rotation].y;
                        		    int newZ = posZ+allSidesLeverButton[rotation].z;

                        		    int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		    int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		    int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

									int time = ((blockData[chunkIndex*width*width*height + blockIndex]&0b00110000)>>4);
									if(time > 0)
									{
										time--;
										redstoneData[blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11001111)|(time<<4));

										if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true)
                                        {
                                            if(chunkIndexNew != chunkIndex)
										    blockData[blockDataIndex] = ((blockData[blockDataIndex]&0b01111111)|0b10000000);
                                            else
                                            {
                                                redstoneData[blockIndexNew] = ((blockData[blockDataIndex]&0b01111111)|0b10000000);
                                                updateRedstoneData[blockIndexNew] = 1;
                                            }
                                        }

										redstoneData[blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b01111111)|0b10000000);
                                        updateRedstoneData[blockIndex] = 1;
									}
									else
									{
										if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000001) == 0b00000001)
										{
											redstoneData[blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b11111110);

											if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true)
                                            {
                                                if(chunkIndexNew != chunkIndex)
									            blockData[blockDataIndex] = (blockData[blockDataIndex]&0b01111111);
                                                else
                                                {
                                                    redstoneData[blockIndexNew] = (blockData[blockDataIndex]&0b01111111);
                                                    updateRedstoneData[blockIndexNew] = 1;
                                                }
                                            }

                                            redstoneData[blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b01111111);
                                            updateRedstoneData[blockIndex] = 1;

											updateChunkAtIndexFull = 1;
										}
									}
								}

                                //updateing blocks above torch
                        		if(blocks[chunkIndex][blockIndex] == 55)
                        		{
                        		    int blockIndexNew = blockIndex+width*width;
                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndexNew;
                        		    if(blockTypes[allBlock[blocks[chunkIndex][blockIndexNew]].blockType].canHoldRedstoneSignal == true && (blockData[blockDataIndex]&0b10000000) == 0b00000000)
                        		    {
                        		        blockData[blockDataIndex] = ((blockData[blockDataIndex]&0b01111111)|0b10000000);
                        		        updateChunkAtIndex = 1;
                        		    }
                        		}
                        		if(blocks[chunkIndex][blockIndex] == 56)
                        		{
                        		    int blockIndexNew = blockIndex+width*width;
                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndexNew;
                        		    if(blockTypes[allBlock[blocks[chunkIndex][blockIndexNew]].blockType].canHoldRedstoneSignal == true  && (blockData[blockDataIndex]&0b10000000) == 0b10000000)
                        		    {
                        		        blockData[blockDataIndex] = (blockData[blockDataIndex]&0b01111111);
                        		        updateChunkAtIndex = 1;
                        		    }
                        		}

                        		//updating torch
                        		if(blocks[chunkIndex][blockIndex] == 55)
                        		{
                        		    int blockIndexNew = blockIndex-width*width;
                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndexNew;

									if(blockTypes[allBlock[blocks[chunkIndex][blockIndexNew]].blockType].canHoldRedstoneSignal == true)
									{
                        		    	if((blockData[blockDataIndex]&0b11000000) > 0b00000000)
                        		    	{
                        		    	    redstoneBlockData[blockIndex] = 56;
											blockData[chunkIndex*width*width*height + blockIndex] = 0b00000000;

                                            int blockIndexNew2 = blockIndex+width*width;
                        		            int blockDataIndex2 = chunkIndex*width*width*height + blockIndexNew2;
                        		            if(blockTypes[allBlock[blocks[chunkIndex][blockIndexNew2]].blockType].canHoldRedstoneSignal == true  && (blockData[blockDataIndex2]&0b10000000) == 0b10000000)
                        		            {
                                                if(y+1 != height)
                        		                blockData[blockDataIndex2] = (blockData[blockDataIndex2]&0b01111111);
                        		            }

                        		    	    updateChunkAtIndex = 1;
                        		    	}
									}
                        		}
                        		if(blocks[chunkIndex][blockIndex] == 56)
                        		{
                        		    int blockIndexNew = blockIndex-width*width;
                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndexNew;

									if(blockTypes[allBlock[blocks[chunkIndex][blockIndexNew]].blockType].canHoldRedstoneSignal == true)
									{
                        		    	if((blockData[blockDataIndex]&0b11000000) == 0b00000000)
                        		    	{
                        		    	    redstoneBlockData[blockIndex] = 55;
											blockData[chunkIndex*width*width*height + blockIndex] = 0b10000000;

                                            int blockIndexNew2 = blockIndex+width*width; //updating blocks above torch
                        		            int blockDataIndex2 = chunkIndex*width*width*height + blockIndexNew2;
                        		            if(blockTypes[allBlock[blocks[chunkIndex][blockIndexNew2]].blockType].canHoldRedstoneSignal == true && (blockData[blockDataIndex2]&0b10000000) == 0b00000000)
                        		            {
                                                if(y+1 != height)
                        		                redstoneData[blockIndexNew2] = ((blockData[blockDataIndex2]&0b01111111)|0b10000000);
                        		            }

                        		    	    updateChunkAtIndex = 1;
                        		    	}
									}
                        		}
	
                                //updating block in front of repeater
                        		if(blocks[chunkIndex][blockIndex] == 57)
                        		{
                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndex;
                        		    int rotation = blockData[blockDataIndex]&0b00000011;

                        		    int newX = (chunkX*width + x) - sidesRepeater[rotation].x;
                        		    int newZ = (chunkZ*width + z) - sidesRepeater[rotation].z;

                        		    if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
                        		    {
                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;
                        		        int blockDataIndexNew = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true && (blockData[blockDataIndexNew]&0b10000000) == 0b10000000)
                        		        {
                        		            blockData[blockDataIndexNew] = ((blockData[blockDataIndexNew]&0b01111111));
                        		            updateChunkAtIndex = 1;
                        		        }
                        		    }
                        		}
                        		if(blocks[chunkIndex][blockIndex] == 58)
                        		{
                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndex;
                        		    int rotation = blockData[blockDataIndex]&0b00000011;

                        		    int newX = (chunkX*width + x) - sidesRepeater[rotation].x;
                        		    int newZ = (chunkZ*width + z) - sidesRepeater[rotation].z;

                        		    if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
                        		    {
                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;
                        		        int blockDataIndexNew = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true && (blockData[blockDataIndexNew]&0b10000000) == 0b00000000)
                        		        {
                        		            blockData[blockDataIndexNew] = ((blockData[blockDataIndexNew]&0b01111111)|0b10000000);
                        		            updateChunkAtIndex = 1;
                        		        }
                        		    }
                        		}

                        		//updating repeater
                        		if(blocks[chunkIndex][blockIndex] == 57)
                        		{
                        		    int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndex;
                        		    int rotation = blockData[blockDataIndex]&0b00000011;

                        		    int newX = posX+sidesRepeater[rotation].x;
                        		    int newY = y+sidesRepeater[rotation].y;
                        		    int newZ = posZ+sidesRepeater[rotation].z;

                        		    if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                        		    {
                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

                        		        bool change = false;

										if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canHoldRedstoneSignal == true)
										{
											if((blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b11000000) > 0b00000000)
                        		            change = true;
										}
                        		        else if(blocks[chunkIndexNew][blockIndexNew] == 58)
                        		        {
                        		            int blockDataIndexNew = chunkIndexNew*width*width*height + blockIndexNew;
                        		            if((blockData[blockDataIndexNew]&0b00000011) == rotation)
                        		            change = true;
                        		        }
	
                        		        if(change == true)
                        		        {
											bool hasNoSignal = true;
											for (int i = 0; i < 2; i++)
											{
												int newX2 = posX+sidesRepeaterForStopSignal[rotation][i].x;
                        		    			int newY2 = y+sidesRepeaterForStopSignal[rotation][i].y;
                        		    			int newZ2 = posZ+sidesRepeaterForStopSignal[rotation][i].z;

												int chunkIndexNew2 = (newX2/width) + (newZ2/width) * totalChunkWidth;
                        		        		int blockIndexNew2 = (newX2%width) + (newZ2%width) * width + newY2 * width * width;

												if(blocks[chunkIndexNew2][blockIndexNew2] == 58 && (blockData[chunkIndexNew2*width*width*height+blockIndexNew2]&0b00000011) == sidesRepeaterForStopSignalRotation[rotation][i])
												hasNoSignal = false;
											}

											if(hasNoSignal == true)
											{
                        		            	redstoneBlockData[blockIndex] = 58;

                                                int newX3 = (chunkX*width + x) - sidesRepeater[rotation].x;
                        		                int newZ3 = (chunkZ*width + z) - sidesRepeater[rotation].z;

                        		                if(newX3 > 0 && newX3 < width*totalChunkWidth && newZ3 > 0 && newZ3 < width*totalChunkWidth)
                        		                {
                        		                    int chunkIndexNew3 = (newX3/width) + (newZ3/width) * totalChunkWidth;
                        		                    int blockIndexNew3 = (newX3%width) + (newZ3%width) * width + y * width * width;
                        		                    int blockDataIndexNew3 = chunkIndexNew3*width*width*height + blockIndexNew3;

                        		                    if(blockTypes[allBlock[blocks[chunkIndexNew3][blockIndexNew3]].blockType].canTransferRedstoneSignal == true && (blockData[blockDataIndexNew3]&0b10000000) == 0b00000000)
                        		                    {
                                                        if(chunkIndexNew3 != chunkIndex)
                        		                        blockData[blockDataIndexNew3] = ((blockData[blockDataIndexNew3]&0b01111111)|0b10000000);
                                                        else
                                                        {
                                                            redstoneData[blockIndexNew3] = ((blockData[blockDataIndexNew3]&0b01111111)|0b10000000);
                                                            updateRedstoneData[blockIndexNew3] = 1;
                                                        }
                        		                    }
                        		                }

                        		            	updateChunkAtIndex = 1;
											}
                        		        }
                        		    }
                        		}
                        		if(blocks[chunkIndex][blockIndex] == 58)
                        		{
                        		    int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

                        		    int blockDataIndex = chunkIndex*width*width*height + blockIndex;
                        		    int rotation = blockData[blockDataIndex]&0b00000011;

                        		    int newX = posX+sidesRepeater[rotation].x;
                        		    int newY = y+sidesRepeater[rotation].y;
                        		    int newZ = posZ+sidesRepeater[rotation].z;

                        		    if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                        		    {
                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

                        		        bool change = false;

										if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canHoldRedstoneSignal == true)
										{
											if((blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b11000000) == 0b00000000)
                        		            change = true;
										}
                        		        else if(blocks[chunkIndexNew][blockIndexNew] == 58)
                        		        {
                        		            int blockDataIndexNew = chunkIndexNew*width*width*height + blockIndexNew;
                        		            if((blockData[blockDataIndexNew]&0b00000011) != rotation)
                        		            change = true;
                        		        }
                        		        else
                        		        change = true;

                        		        if(change == true)
                        		        {
											bool hasNoSignal = true;
											for (int i = 0; i < 2; i++)
											{
												int newX2 = posX+sidesRepeaterForStopSignal[rotation][i].x;
                        		    			int newY2 = y+sidesRepeaterForStopSignal[rotation][i].y;
                        		    			int newZ2 = posZ+sidesRepeaterForStopSignal[rotation][i].z;

												int chunkIndexNew2 = (newX2/width) + (newZ2/width) * totalChunkWidth;
                        		        		int blockIndexNew2 = (newX2%width) + (newZ2%width) * width + newY2 * width * width;

												if(blocks[chunkIndexNew2][blockIndexNew2] == 58 && (blockData[chunkIndexNew2*width*width*height+blockIndexNew2]&0b00000011) == sidesRepeaterForStopSignalRotation[rotation][i])
												hasNoSignal = false;
											}

											if(hasNoSignal == true)
											{
                        		            	redstoneBlockData[blockIndex] = 57;

												int newX3 = (chunkX*width + x) - sidesRepeater[rotation].x;
                        		    			int newZ3 = (chunkZ*width + z) - sidesRepeater[rotation].z;

                        		    			if(newX3 > 0 && newX3 < width*totalChunkWidth && newZ3 > 0 && newZ3 < width*totalChunkWidth) //update block in front of the repeater
                        		    			{
                        		    			    int chunkIndexNew3 = (newX3/width) + (newZ3/width) * totalChunkWidth;
                        		    			    int blockIndexNew3 = (newX3%width) + (newZ3%width) * width + y * width * width;
                        		    			    int blockDataIndexNew3 = chunkIndexNew3*width*width*height + blockIndexNew3;

                        		    			    if(blockTypes[allBlock[blocks[chunkIndexNew3][blockIndexNew3]].blockType].canTransferRedstoneSignal == true && (blockData[blockDataIndexNew3]&0b10000000) == 0b10000000)
                        		    			    {
														if(chunkIndexNew3 != chunkIndex)
                        		    			        blockData[blockDataIndexNew3] = ((blockData[blockDataIndexNew3]&0b01111111));
														else
														{
															redstoneData[blockIndexNew3] = ((blockData[blockDataIndexNew3]&0b01111111));
                                                            updateRedstoneData[blockIndexNew3] = 1;
														}
                        		    			    }
                        		    			}

                        		            	updateChunkAtIndex = 1;
											}
                        		        }
                        		    }
                        		}

                        		//updating iron door
                        		if(blocks[chunkIndex][blockIndex] == 59)
                        		{
                        		    int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

                        		    bool openDoor = false;

                        		    for (int i = 0; i < 10; i++)
                        		    {
                        		        int newX = posX+allSidesDoor[i].x;
                        		        int newY = y+allSidesDoor[i].y;
                        		        int newZ = posZ+allSidesDoor[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 17 || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 18)
										{
											if((blockData[blockDataIndex]&0b10000000) == 0b10000000)
                        		        	openDoor = true;
										}
                        		    }

                        		    for (int i = 0; i < 4; i++)
                        		    {
                        		        int newX = posX+sidesRepeater[i].x;
                        		        int newY = y+sidesRepeater[i].y;
                        		        int newZ = posZ+sidesRepeater[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blocks[chunkIndexNew][blockIndexNew] == 58 && (blockData[blockDataIndex]&0b00000011) == i)
                        		        openDoor = true;
                        		    }

                        		    for (int i = 0; i < 4; i++)
                        		    {
                        		        int newX = posX+sidesRepeater[i].x;
                        		        int newY = y+sidesRepeater[i].y+1;
                        		        int newZ = posZ+sidesRepeater[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blocks[chunkIndexNew][blockIndexNew] == 58 && (blockData[blockDataIndex]&0b00000011) == i)
                        		        openDoor = true;
                        		    }

                        		    if(openDoor == true)
                        		    {
                        		        if(((blockData[chunkIndex*width*width*height + blockIndex]&0b00000100)>>2) == 0)
                        		        {
                        		            if((blockData[chunkIndex*width*width*height + blockIndex]&0b00000011) == 3)
                        		            blockData[chunkIndex*width*width*height + blockIndex] = (blockData[chunkIndex*width*width*height + blockIndex]&0b11111100);
                        		            else
                        		            blockData[chunkIndex*width*width*height + blockIndex]++;

                        		            if((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b00000011) == 3)
                        		            blockData[chunkIndex*width*width*height + blockIndex + width*width] = (blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111100);
                        		            else
                        		            blockData[chunkIndex*width*width*height + blockIndex + width*width]++;

                        		            blockData[chunkIndex*width*width*height + blockIndex] = ((blockData[chunkIndex*width*width*height + blockIndex]&0b11111011) | 0b00000100);
                        		            blockData[chunkIndex*width*width*height + blockIndex + width*width] = ((blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111011) | 0b00000100);

                        		            updateChunkAtIndexFull = 1;
                        		        }
                        		    }
                        		    else
                        		    {
                        		        if(((blockData[chunkIndex*width*width*height + blockIndex]&0b00000100)>>2) == 1)
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
                        		            blockData[chunkIndex*width*width*height + blockIndex + width*width] = (blockData[chunkIndex*width*width*height + blockIndex + width*width]&0b11111011);

                        		            updateChunkAtIndexFull = 1;
                        		        }
                        		    }
                        		}

                        		//updating redstone lamps
                        		if(blocks[chunkIndex][blockIndex] == 61 || blocks[chunkIndex][blockIndex] == 62)
                        		{
                        		    int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

                        		    bool turnOnLamp = false;

                        		    for (int i = 0; i < 6; i++)
                        		    {
                        		        int newX = posX+allSides[i].x;
                        		        int newY = y+allSides[i].y;
                        		        int newZ = posZ+allSides[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 17 || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 18)
										{
											if((blockData[blockDataIndex]&0b11000000) > 0b00000000)
                        		        	turnOnLamp = true;
										}

                        		        if(blocks[chunkIndexNew][blockIndexNew] == 55)
                        		        turnOnLamp = true;
                        		    }

                        		    for (int i = 0; i < 4; i++)
                        		    {
                        		        int newX = posX+sidesRepeater[i].x;
                        		        int newY = y+sidesRepeater[i].y;
                        		        int newZ = posZ+sidesRepeater[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blocks[chunkIndexNew][blockIndexNew] == 58 && (blockData[blockDataIndex]&0b00000011) == i)
                        		        turnOnLamp = true;
                        		    }

                        		    int chunkIndexNew = (posX/width) + (posZ/width) * totalChunkWidth;
                        		    int blockIndexNew = (posX%width) + (posZ%width) * width + (y+1) * width * width;
                        		    int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		    if(blocks[chunkIndexNew][blockIndexNew] == 54 && (blockData[blockDataIndex]&0b00001111) > 0)
                        		    turnOnLamp = true;

                        		    if(turnOnLamp == true)
                        		    {
                        		        if(blocks[chunkIndex][blockIndex] == 61)
                        		        {
                        		            blocks[chunkIndex][blockIndex] = 62;
                        		            updateChunkAtIndex = 1;
                        		            updateChunkAtIndexLight = 1;
                        		        }
                        		    }
                        		    else
                        		    {
                        		        if(blocks[chunkIndex][blockIndex] == 62)
                        		        {
                        		            blocks[chunkIndex][blockIndex] = 61;
                        		            updateChunkAtIndex = 1;
                        		            updateChunkAtIndexLight = 1;
                        		        }
                        		    }
                        		}

								//update the tnt do the boom
								if(blocks[chunkIndex][blockIndex] == 18)
								{
									int posX = chunkX*width + x;
                        		    int posZ = chunkZ*width + z;

                        		    for (int i = 0; i < 6; i++)
                        		    {
                        		        int newX = posX+allSides[i].x;
                        		        int newY = y+allSides[i].y;
                        		        int newZ = posZ+allSides[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType].canTransferRedstoneSignal == true || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 17 || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 18)
										{
											if((blockData[blockDataIndex]&0b11000000) > 0b00000000)
                        		        	makeExplosion(10, posX, y, posZ);
										}

                        		        if(blocks[chunkIndexNew][blockIndexNew] == 55)
                        		        makeExplosion(10, posX, y, posZ);
                        		    }

                        		    for (int i = 0; i < 4; i++)
                        		    {
                        		        int newX = posX+sidesRepeater[i].x;
                        		        int newY = y+sidesRepeater[i].y;
                        		        int newZ = posZ+sidesRepeater[i].z;

                        		        int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		        int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
                        		        int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		        if(blocks[chunkIndexNew][blockIndexNew] == 58 && (blockData[blockDataIndex]&0b00000011) == i)
                        		        makeExplosion(10, posX, y, posZ);
                        		    }

                        		    int chunkIndexNew = (x/width) + (z/width) * totalChunkWidth;
                        		    int blockIndexNew = (x%width) + (z%width) * width + (y+1) * width * width;
                        		    int blockDataIndex = chunkIndexNew*width*width*height + blockIndexNew;

                        		    if(blocks[chunkIndexNew][blockIndexNew] == 54 && (blockData[blockDataIndex]&0b00001111) > 0)
                        		    makeExplosion(10, posX, y, posZ);
								}
                                //-------------------------redstone end-------------------------
								//-------------------------normal start-------------------------
								if(blocks[chunkIndex][blockIndex] == 4) //update leaves
								{
									bool hasLogClose = false;
									int totalX = chunkX*width + x;
									int totalZ = chunkZ*width + z;

									for (int x2 = -2; x2 <= 2; x2++)
									{
										for (int y2 = -2; y2 <= 2; y2++)
										{
											for (int z2 = -2; z2 <= 2; z2++)
											{
												if(hasLogClose == false)
												{
													int x3 = totalX + x2;
													int z3 = totalZ + z2;
													int chunkIndex2 = (x3/width) + (z3/width) * totalChunkWidth; 
													int blockIndex2 = (x3%width) + (z3%width) * width + (y+y2) * width * width;
													if(blocks[chunkIndex2][blockIndex2] == 5)
													hasLogClose = true;
												}
											}
										}
									}
									if(hasLogClose == false)
                                    {
										if(generate_random3D(SEED+RTC_GetTicks(), 256, totalX, y, totalZ) == 1)
										{
									    	blocks[chunkIndex][blockIndex] = 0;
                                        	updateChunkAtIndexFull = 1;

											if(generate_random1D(SEED, 20, RTC_GetTicks()) == 1)
                            				makeItem(totalX*10, y*10, totalZ*10, 0, 0, 0, 54);
										}
                                    }
								}

								else if(blocks[chunkIndex][blockIndex] == 24 && cycle % 6 == 0) //update lava
								{
									int posX = chunkX*width + x;
              					    int posZ = chunkZ*width + z;

                                    if(y != 0)
                                    {
                                        if(blocks[chunkIndex][blockIndex-width*width] == 0) //update block below
                                        {
                                            waterMovementChunk[blockIndex-width*width] = 24;
											blockData[chunkIndex*width*width*height + (blockIndex-width*width)] = 0;
											updateChunkAtIndexWater = 1;
                                        }
										else if(blocks[chunkIndex][blockIndex-width*width] == 24) //
										{
											if((blockData[chunkIndex*width*width*height + (blockIndex-width*width)]&0b00001111) != 0b00000000) //if water below, update water below to full block
											{
												blockData[chunkIndex*width*width*height + (blockIndex-width*width)] = (blockData[chunkIndex*width*width*height + (blockIndex-width*width)]&0b11110000);
												updateChunkAtIndexWater = 2;
											}

											bool hasHigherClose = (blockData[chunkIndex*width*width*height + blockIndex]&0b10000000);
											if(y != height-1)
											{
												if((blocks[chunkIndex][blockIndex+width*width]) == 24)
												hasHigherClose = true;
											}

											if(hasHigherClose == false)
											{
												for (int i = 0; i < 4; i++)
												{
													int newX = posX+sidesStraight[i].x;
                                        			int newZ = posZ+sidesStraight[i].z;
                    
                                        			if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
                                        			{
														int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                        		    	int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;

														if(blocks[chunkIndexNew][blockIndexNew] == 24)
														{
															if((blockData[chunkIndex*width*width*height + blockIndex]&0b00001111) > (blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b00001111))
															{
																hasHigherClose = true;
																break;
															}
														}
												 	}
												}

												if(hasHigherClose == false)
												{
													blockData[chunkIndex*width*width*height + blockIndex] += 4;
													if(blockData[chunkIndex*width*width*height + blockIndex] > 8)
													{
														blocks[chunkIndex][blockIndex] = 0;
														blockData[chunkIndex*width*width*height + blockIndex] = 0;

														if((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b00001111) == 0b00000000)
														blockData[chunkIndex*width*width*height + blockIndex-width*width] = ((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b11110000)|0b00000001);
													}
													updateChunkAtIndexWater = 3;
												}
											}
										}
										else if(blocks[chunkIndex][blockIndex-width*width] == 6) //check for water below
										{
											blocks[chunkIndex][blockIndex-width*width] = 3;
                                            updateChunkAtIndexFull = 1;
										}
										else
										{
											bool hasHigherClose = (blockData[chunkIndex*width*width*height + blockIndex]&0b10000000);
											if(y != height-1)
											{
												if((blocks[chunkIndex][blockIndex+width*width]) == 24)
												hasHigherClose = true;
											}

											int waterHight = (blockData[chunkIndex*width*width*height + blockIndex]&0b00001111)+2;
											if((blockData[chunkIndex*width*width*height + blockIndex]&0b00001111) == 0b00000000)
											waterHight = 1;

											for (int i = 0; i < 4; i++)
											{
												int newX = posX+sidesStraight[i].x;
                                            	int newZ = posZ+sidesStraight[i].z;
                    
                                            	if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
                                            	{
													int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                                	int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;

													if(blocks[chunkIndexNew][blockIndexNew] == 0 || blocks[chunkIndexNew][blockIndexNew] == 24)
													{
														if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b10000000) == 0b00000000)
														{
															if(waterHight < 9)
															{
																if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b00001111) > waterHight || blocks[chunkIndexNew][blockIndexNew] == 0)
																{
																	if(chunkIndexNew == chunkIndex)
																	{
																		waterMovementChunk[blockIndexNew] = 24;
																		blockData[chunkIndexNew*width*width*height + blockIndexNew] = waterHight;
																		updateChunkAtIndexWater = 4;
																	}
																	else
																	{
																		if(abs_int((newX/width)-(int)(PPosX/width))+abs_int((newZ/width)-(int)(PPosZ/width)) <= (redstoneRenderDistance-1))
																		{
																			blocks[chunkIndexNew][blockIndexNew] = 24;
																			blockData[chunkIndexNew*width*width*height + blockIndexNew] = waterHight;
																			updateChunkAtIndexWater = 5;
																		}
																	}
																}
															}
														}
													}

													if(hasHigherClose == false)
													{
														if(blocks[chunkIndexNew][blockIndexNew] == 24)
														{
															if((blockData[chunkIndex*width*width*height + blockIndex]&0b00001111) > (blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b00001111))
															hasHigherClose = true;
														}
													}
											 	}
											}

											if(hasHigherClose == false)
											{
												blockData[chunkIndex*width*width*height + blockIndex] += 4;
												if(blockData[chunkIndex*width*width*height + blockIndex] > 8)
												{
													blocks[chunkIndex][blockIndex] = 0;
													blockData[chunkIndex*width*width*height + blockIndex] = 0;

													if((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b00001111) == 0b00000000)
													blockData[chunkIndex*width*width*height + blockIndex-width*width] = ((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b11110000)|0b00000001);
												}
												updateChunkAtIndexWater = 6;
											}
										}
									}
								}

								else if(blocks[chunkIndex][blockIndex] == 6) //update water
								{
									int posX = chunkX*width + x;
              					    int posZ = chunkZ*width + z;

                                    if(y != 0)
                                    {
                                        if(blocks[chunkIndex][blockIndex-width*width] == 0) //update block below
                                        {
                                            waterMovementChunk[blockIndex-width*width] = 6;
											blockData[chunkIndex*width*width*height + (blockIndex-width*width)] = 0;
											updateChunkAtIndexWater = 1;
                                        }
										else if(blocks[chunkIndex][blockIndex-width*width] == 6) //
										{
											if((blockData[chunkIndex*width*width*height + (blockIndex-width*width)]&0b00001111) != 0b00000000) //if water below, update water below to full block
											{
												blockData[chunkIndex*width*width*height + (blockIndex-width*width)] = (blockData[chunkIndex*width*width*height + (blockIndex-width*width)]&0b11110000);
												updateChunkAtIndexWater = 2;
											}

											bool hasHigherClose = (blockData[chunkIndex*width*width*height + blockIndex]&0b10000000);
											if(y != height-1)
											{
												if((blocks[chunkIndex][blockIndex+width*width]) == 6)
												hasHigherClose = true;
											}

											for (int i = 0; i < 4; i++)
											{
												int newX = posX+sidesStraight[i].x;
                                        		int newZ = posZ+sidesStraight[i].z;
                    
                                        		if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
                                        		{
													int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                        	    	int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;

													if(blocks[chunkIndexNew][blockIndexNew] == 24) //check for lava
													{
														if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b10000000) == 0b10000000)
														blocks[chunkIndexNew][blockIndexNew] = 25; //make obsidian
														else
														blocks[chunkIndexNew][blockIndexNew] = 10; //make cobblestone

														updateChunkAtIndexFull = 1;
													}

													if(hasHigherClose == false)
													{
														if(blocks[chunkIndexNew][blockIndexNew] == 6)
														{
															if((blockData[chunkIndex*width*width*height + blockIndex]&0b00001111) > (blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b00001111))
															hasHigherClose = true;
														}
													}
											 	}
											}
												

											if(hasHigherClose == false)
											{
												blockData[chunkIndex*width*width*height + blockIndex] += 2;
												if(blockData[chunkIndex*width*width*height + blockIndex] > 8)
												{
													blocks[chunkIndex][blockIndex] = 0;
													blockData[chunkIndex*width*width*height + blockIndex] = 0;

													if((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b00001111) == 0b00000000)
													blockData[chunkIndex*width*width*height + blockIndex-width*width] = ((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b11110000)|0b00000001);
												}
												updateChunkAtIndexWater = 3;
											}
										}
										else if(blocks[chunkIndex][blockIndex-width*width] == 24) //check for lava below
										{
											if((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b10000000) == 0b10000000)
											blocks[chunkIndex][blockIndex-width*width] = 25; //make obsidian
											else
											blocks[chunkIndex][blockIndex-width*width] = 10; //make cobblestone
                                            updateChunkAtIndexFull = 1;
										}
										else
										{
											bool hasHigherClose = (blockData[chunkIndex*width*width*height + blockIndex]&0b10000000);
											if(y != height-1)
											{
												if((blocks[chunkIndex][blockIndex+width*width]) == 6)
												hasHigherClose = true;
											}

											int waterHight = (blockData[chunkIndex*width*width*height + blockIndex]&0b00001111)+1;
											if(waterHight == 1)
											waterHight++;

											for (int i = 0; i < 4; i++)
											{
												int newX = posX+sidesStraight[i].x;
                                            	int newZ = posZ+sidesStraight[i].z;
                    
                                            	if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
                                            	{
													int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                                	int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;

													if(blocks[chunkIndexNew][blockIndexNew] == 0 || blocks[chunkIndexNew][blockIndexNew] == 6)
													{
														if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b10000000) == 0b00000000)
														{
															if(waterHight < 9)
															{
																if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b00001111) > waterHight || blocks[chunkIndexNew][blockIndexNew] == 0)
																{
																	if(chunkIndexNew == chunkIndex)
																	{
																		waterMovementChunk[blockIndexNew] = 6;
																		blockData[chunkIndexNew*width*width*height + blockIndexNew] = waterHight;
																		updateChunkAtIndexWater = 4;
																	}
																	else
																	{
																		if(abs_int((newX/width)-(int)(PPosX/width))+abs_int((newZ/width)-(int)(PPosZ/width)) <= (redstoneRenderDistance-1))
																		{
																			blocks[chunkIndexNew][blockIndexNew] = 6;
																			blockData[chunkIndexNew*width*width*height + blockIndexNew] = waterHight;
																			updateChunkAtIndexWater = 5;
																		}
																	}
																}
															}
														}
													}
													else if(blocks[chunkIndexNew][blockIndexNew] == 24) //check for lava
													{
														if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b10000000) == 0b10000000)
														blocks[chunkIndexNew][blockIndexNew] = 25; //make obsidian
														else
														blocks[chunkIndexNew][blockIndexNew] = 10; //make cobblestone

														updateChunkAtIndexFull = 1;
													}

													if(hasHigherClose == false)
													{
														if(blocks[chunkIndexNew][blockIndexNew] == 6)
														{
															if((blockData[chunkIndex*width*width*height + blockIndex]&0b00001111) > (blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b00001111))
															hasHigherClose = true;
														}
													}
											 	}
											}

											if(hasHigherClose == false)
											{
												blockData[chunkIndex*width*width*height + blockIndex] += 2;
												if(blockData[chunkIndex*width*width*height + blockIndex] > 8)
												{
													blocks[chunkIndex][blockIndex] = 0;
													blockData[chunkIndex*width*width*height + blockIndex] = 0;

													if((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b00001111) == 0b00000000)
													blockData[chunkIndex*width*width*height + blockIndex-width*width] = ((blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b11110000)|0b00000001);
												}
												updateChunkAtIndexWater = 6;
											}

											//makes new full water so the infite water shit.
											//if((blockData[chunkIndex*width*width*height + blockIndex]&0b10000000) == 0b10000000)
											//{
											//	if((blocks[chunkIndex][blockIndex-width*width] == 6 && (blockData[chunkIndex*width*width*height + blockIndex-width*width]&0b10000000) == 0b10000000) || (blocks[chunkIndex][blockIndex-width*width] != 6 && blocks[chunkIndex][blockIndex-width*width] != 0))
											//	{
											//		for (int i = 0; i < 4; i++)
											//		{
											//			int newX = posX+sidesdiagonal[i].x;
                                        	//			int newY = y+sidesdiagonal[i].y;
                                        	//			int newZ = posZ+sidesdiagonal[i].z;
                    
                                        	//			if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                                        	//			{
											//				int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                        	//		    	int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

											//				if((blockData[chunkIndexNew*width*width*height + blockIndexNew]&0b10000000) == 0b10000000)
											//				{
											//					int chunkIndexNewX = (newX/width) + (posZ/width) * totalChunkWidth;
                                        	//		    		int blockIndexNewX = (newX%width) + (posZ%width) * width + newY * width * width;

											//					if((blockData[chunkIndexNewX*width*width*height + blockIndexNewX]&0b10000000) == 0b00000000)
											//					{
											//						blockData[chunkIndexNewX*width*width*height + blockIndexNewX] = 0b10000001;
											//						updateChunkAtIndexWater = true;
											//					}

											//					int chunkIndexNewZ = (posX/width) + (newZ/width) * totalChunkWidth;
                                        	//		    		int blockIndexNewZ = (posX%width) + (newZ%width) * width + newY * width * width;

											//					if((blockData[chunkIndexNewZ*width*width*height + blockIndexNewZ]&0b10000000) == 0b00000000)
											//					{
											//						blockData[chunkIndexNewZ*width*width*height + blockIndexNewZ] = 0b10000001;
											//						updateChunkAtIndexWater = true;
											//					}
											//				}
											//			}
											//		}
											//	}
											//}
										}
                                    }
								}

                                else if(blocks[chunkIndex][blockIndex] == 23) //update sapling
                                {
                                    int posX = chunkX*width + x;
              					    int posZ = chunkZ*width + z;

                                    if((lightmap[blockIndex+chunkIndex*width*width*height] > 8 && lighting == true) || (skyBrightness > 8 && lighting == false))
                                    {
                                        if(generate_random3D(SEED+RTC_GetTicks(), 1024, posX, y, posZ) == 1)
                                        {
                                            generateTree(posX, y, posZ, generate_random1D(SEED, 4, RTC_GetTicks()) + 4);

                                            updateChunkAtIndexFull = 1;
                                        }
                                    }
                                }

                                else if(blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType].hasToBeOnGrass == true) //update block that has to be on grass
                                {
                                    int posX = chunkX*width + x;
              					    int posZ = chunkZ*width + z;

                                    if(y > 0)
                                    {
                                        if(blocks[chunkIndex][blockIndex-width*width] != 1)
                                        {
                                            makeItem(posX*10, y*10, posZ*10, 0, 0, 0, allBlock[blocks[chunkIndex][blockIndex]].drops);
                                            blocks[chunkIndex][blockIndex] = 0;

                                            updateChunkAtIndexFull = 1;
                                        }
                                    }
                                }

                                else if(blocks[chunkIndex][blockIndex] > 31 && blocks[chunkIndex][blockIndex] < 39) //wheat growing
                                {
                                    if((lightmap[blockIndex+chunkIndex*width*width*height] > 8 && lighting == true) || (skyBrightness > 8 && lighting == false))
                                    {
										int posX = chunkX*width + x;
              					    	int posZ = chunkZ*width + z;

                                        if(generate_random3D(SEED+RTC_GetTicks(), 512, posX, y, posZ) == 1)
                                        {
                                            blocks[chunkIndex][blockIndex] = blocks[chunkIndex][blockIndex]+1;
                                            updateChunkAtIndex = 1;
                                        }
                                    }
									if(y != 0)
									{
										int posX = chunkX*width + x;
              					    	int posZ = chunkZ*width + z;

										if(blocks[chunkIndex][blockIndex-width*width] != 30 && blocks[chunkIndex][blockIndex-width*width] != 31)
										{
											blocks[chunkIndex][blockIndex] = 0;
											makeItem(posX, y, posZ, 0, 0, 0, 65);
											updateChunkAtIndexFull = 1;
										}
									}
                                }
                                
								else if(blocks[chunkIndex][blockIndex] == 2) //dirt to grass
								{
									if(y != height-1)
									{
										if(blockTypes[allBlock[blocks[chunkIndex][blockIndex+width*width]].blockType].canHaveGrassBelow == true)
										{
                                            if((lightmap[blockIndex+chunkIndex*width*width*height] > 8 && lighting == true) || (skyBrightness > 8 && lighting == false))
                                            {
										        int posX = chunkX*width + x;
										        int posZ = chunkZ*width + z;

										        for (int i = 0; i < 12; i++)
										        {
										        	int newX = posX+sidesDirtToGrass[i].x;
                        		  		        	int newY = y+sidesDirtToGrass[i].y;
                        		  		        	int newZ = posZ+sidesDirtToGrass[i].z;

										        	if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
										        	{
                        		  		        		int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                        		  		        		int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

										        		if(blocks[chunkIndexNew][blockIndexNew] == 1 && generate_random3D(SEED+RTC_GetTicks(), 80, newX, newY, newZ) == 0)
										        		{
										        			if(blockTypes[allBlock[blocks[chunkIndexNew][blockIndexNew+width*width]].blockType].canHaveGrassBelow == true)
										        			{
										        				blocks[chunkIndex][blockIndex] = 1;
										        				updateChunkAtIndex = 1;
										        			}
										        		}
										        	}
										        }
                                            }
										}
									}
								}

								else if(blocks[chunkIndex][blockIndex] == 1) //grass with block above to dirt
								{
									if(y != height-1)
									{
										if(blockTypes[allBlock[blocks[chunkIndex][blockIndex+width*width]].blockType].canHaveGrassBelow == false)
										{
											int posX = chunkX*width + x;
											int posZ = chunkZ*width + z;

											if(generate_random3D(SEED+RTC_GetTicks(), 80, posX, y, posZ) == 0)
											{
												blocks[chunkIndex][blockIndex] = 1;
												updateChunkAtIndex = 1;
											}
										}
									}
								}

								else if(blocks[chunkIndex][blockIndex] == 30) //farmland to wet farmland or dirt
								{
									int posX = chunkX*width + x;
									int posZ = chunkZ*width + z;
									
									bool hasWaterClose = false;

									if(y != height-1) //crop is on top
									{
										for (int x1 = -5; x1 < 6; x1++)
										{
											for (int z1 = -5; z1 < 6; z1++)
											{
												int newX = posX+x1;
                        						int newZ = posZ+z1;

												if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
												{
													int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
													int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;

													if(blocks[chunkIndexNew][blockIndexNew] == 6)
													{
														hasWaterClose = true;
														break;
													}
												}
											}
											if(hasWaterClose == true)
											break;
										}

										if(allBlock[blocks[chunkIndex][blockIndex+width*width]].blockType != 7)
										{
											if(hasWaterClose == false)
											{
												if(generate_random3D(SEED+RTC_GetTicks(), 120, posX, y, posZ) == 0)
												{
													blocks[chunkIndex][blockIndex] = 2;
													updateChunkAtIndexFull = 1;
												}
											}
										}

										if(hasWaterClose == true)
										{
											if(generate_random3D(SEED+RTC_GetTicks(), 120, posX, y, posZ) == 0)
											{
												blocks[chunkIndex][blockIndex] = 31;
												updateChunkAtIndex = 1;
											}
										}
									}
								}

								else if(blocks[chunkIndex][blockIndex] == 31) //wet farmland to farmland
								{
									int posX = chunkX*width + x;
									int posZ = chunkZ*width + z;
									
									bool hasWaterClose = false;

									for (int x1 = -5; x1 < 6; x1++)
									{
										for (int z1 = -5; z1 < 6; z1++)
										{
											int newX = posX+x1;
                        					int newZ = posZ+z1;

											if(newX > 0 && newX < width*totalChunkWidth && newZ > 0 && newZ < width*totalChunkWidth)
											{
												int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
												int blockIndexNew = (newX%width) + (newZ%width) * width + y * width * width;

												if(blocks[chunkIndexNew][blockIndexNew] == 6)
												{
													hasWaterClose = true;
													break;
												}
											}
										}
										if(hasWaterClose == true)
										break;
									}

									if(hasWaterClose == false)
									{
										if(generate_random3D(SEED+RTC_GetTicks(), 120, posX, y, posZ) == 0)
										{
											blocks[chunkIndex][blockIndex] = 30;
											updateChunkAtIndex = 1;
										}
									}
								}
								//-------------------------normal end-------------------------
                            }
						}
                    }
                }

				for (int i = 0; i < 5760; i++) //update the map itself
				{
					if(redstoneBlockData[i] != 0)
					blocks[chunkIndex][i] = redstoneBlockData[i];

                    if(updateRedstoneData[i] == 1)
                    blockData[chunkIndex*width*width*height + i] = redstoneData[i];
				}

				memset(redstoneData, 0, 5760);
				
                for (int j = 0; j < 16; j++)
                {
                    int redstoneMapChanges = 0;
                    for (int y = 0; y < height; y++)
                    {
                        for (int z = 0; z < width; z++)
                        {
                            for (int x = 0; x < width; x++)
                            {
                                int blockIndex = x+z*width+y*width*width;
                                if(blocks[chunkIndex][blockIndex] == 54)
                                {
                                    int posX = chunkX*width + x;
                                    int posZ = chunkZ*width + z;

                                    int highestNeighbourLevel = 1;

                                    for (int i = 0; i < 6; i++)
                                    {
                                        int newX = posX+allSides[i].x;
                                        int newY = y+allSides[i].y;
                                        int newZ = posZ+allSides[i].z;

                                        if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                                        {
                                            int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                            int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

                                            if(allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 1 || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 17 || allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType == 18)
                                            {
                                                int blockDataIndex = chunkIndexNew * width*width*height + blockIndexNew;
                                                if((blockData[blockDataIndex]&0b10000000) == 0b10000000)
                                                highestNeighbourLevel = 16;
                                            }
                                        }
                                    }
                                
                                    for (int i = 0; i < 4; i++) //same level
                                    {
                                        int newX = posX+sidesRepeater[i].x;
                                        int newY = y+sidesRepeater[i].y;
                                        int newZ = posZ+sidesRepeater[i].z;

                                        if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                                        {
                                            int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                            int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

                                            if(blocks[chunkIndexNew][blockIndexNew] == 54)
                                            {
                                                if(chunkIndex != chunkIndexNew)
                                                {
                                                    if((blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00001111) > highestNeighbourLevel)
                                                    highestNeighbourLevel = (blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00001111);
                                                }
                                                else
                                                {
                                                    if((redstoneData[blockIndexNew]&0b00001111) > highestNeighbourLevel)
                                                    highestNeighbourLevel = (redstoneData[blockIndexNew]&0b00001111);
                                                }
                                            }
                                            if(blocks[chunkIndexNew][blockIndexNew] == 58)
                                            {
                                                if((blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00000011) == i)
                                                highestNeighbourLevel = 16;
                                            }
                                            if(blocks[chunkIndexNew][blockIndexNew] == 55)
                                            highestNeighbourLevel = 16;
                                        }
                                    }
                                    for (int i = 0; i < 4; i++) //one down
                                    {
                                        int newX = posX+sidesDown[i].x;
                                        int newY = y+sidesDown[i].y;
                                        int newZ = posZ+sidesDown[i].z;

                                        if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                                        {
                                            int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
                                            int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

                                            if(blocks[chunkIndexNew][blockIndexNew] == 54)
                                            {
                                                if(chunkIndex != chunkIndexNew)
                                                {
                                                    if((blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00001111) > highestNeighbourLevel)
                                                    highestNeighbourLevel = (blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00001111);
                                                }
                                                else
                                                {
                                                    if((redstoneData[blockIndexNew]&0b00001111) > highestNeighbourLevel)
                                                    highestNeighbourLevel = (redstoneData[blockIndexNew]&0b00001111);
                                                }
                                            }
                                        }
                                    }
                                    for (int i = 0; i < 4; i++) //one up
                                    {
                                        int newX = posX+sidesUp[i].x;
                                        int newY = y+sidesUp[i].y;
                                        int newZ = posZ+sidesUp[i].z;

                                        if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
                                        {
                                            int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;

                                            if(blocks[chunkIndexNew][x+z*width+newY*width*width] == 0)
                                            {
                                                int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;

                                                if(blocks[chunkIndexNew][blockIndexNew] == 54)
                                                {
                                                    if(chunkIndex != chunkIndexNew)
                                                    {
                                                        if((blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00001111) > highestNeighbourLevel)
                                                        highestNeighbourLevel = (blockData[chunkIndexNew*width*width*height+blockIndexNew]&0b00001111);
                                                    }
                                                    else
                                                    {
                                                        if((redstoneData[blockIndexNew]&0b00001111) > highestNeighbourLevel)
                                                        highestNeighbourLevel = (redstoneData[blockIndexNew]&0b00001111);
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if(highestNeighbourLevel > 0 && redstoneData[blockIndex] != highestNeighbourLevel-1)
                                    {
                                        redstoneData[blockIndex] = highestNeighbourLevel-1;
                                        redstoneMapChanges++;
                                    }
                                }
                            }
                        }
                    }

                    if(redstoneMapChanges == 0)
                    break;
                }

                for (int i = 0; i < 5760; i++)
                {
                    if(blocks[chunkIndex][i] == 54)
                    {
                        if((blockData[chunkIndex*width*width*height+i]&0b00001111) != redstoneData[i])
                        {
                            updateChunkAtIndex = 1;
                            blockData[chunkIndex*width*width*height+i] = redstoneData[i];

                            if(allBlock[blocks[chunkIndex][i-width*width]].blockType == 1)
                            {
                                if(redstoneData[i] > 0)
								{
                                	blockData[chunkIndex*width*width*height+(i-width*width)] = ((blockData[chunkIndex*width*width*height+(i-width*width)]&0b10111111)|0b01000000);
									blockData[chunkIndex*width*width*height+i] = ((blockData[chunkIndex*width*width*height+i]&0b10111111)|0b01000000);
								}
                                else
								{
                                	blockData[chunkIndex*width*width*height+(i-width*width)] = (blockData[chunkIndex*width*width*height+(i-width*width)]&0b10111111);
									blockData[chunkIndex*width*width*height+i] = (blockData[chunkIndex*width*width*height+i]&0b10111111);
								}
                            }
                        }
                    }
                }

                if(updateChunkAtIndexWater > 0)
                {
                    for (int i = 0; i < width*width*height; i++)
                    {
                        if(waterMovementChunk[i] != 0)
                        blocks[chunkIndex][i] = waterMovementChunk[i];
                    }

					if(lighting == true)
					updateChunkLighting2(chunkX, chunkZ); 

					updateChunkAtIndexFull = 1;
                }

                if(updateChunkAtIndex == 1)
                updateChunkTexturesV2(chunkX, chunkZ);

				if(updateChunkAtIndexLight == 1)
				{
					if(lighting == true)
					{
                		updateChunkLighting2(chunkX, chunkZ); 
						updateChunkLightFacesV2(chunkX, chunkZ);
					}
				}

                if(updateChunkAtIndexFull == 1)
                updateChunkV2(chunkX, chunkZ);
    //        }
    //    }
    //}
}
void changeBrightnessAllTriangles(int lighChange)
{
    for (int i = 0; i < totalChunkWidth*totalChunkWidth; i++)
    {
        for (int j = 0; j < allObj[i].sizeT; j++)
        allObj[i].brightnes[j] += lighChange;
