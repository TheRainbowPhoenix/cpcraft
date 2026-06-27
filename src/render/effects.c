/* src/render/effects.c - Crosshair and block destruction rendering */
#include "engine.h"
#include "config.h"
#include "chunk_constants.h"


void renderBlockDestruction(float timeDone, float totalTime)
{
	int destroyIndex = timeDone/(totalTime/10);

	Vector3B directions[6] = {{0, 1, 0},{0, 0, -1},{-1, 0, 0},{0, -1, 0},{1, 0, 0},{0, 0, 1}};
	Vector3B vertices[8] = {{10, 10, 10}, {10, 0 , 10}, {10, 10, 0 }, {10, 0 , 0 }, {0 , 10, 10}, {0 , 0 , 10},{0 , 10, 0 }, {0 , 0 , 0 }};
	Vector4B faces[6] = {{0, 4, 6, 2}, {3, 2, 6, 7}, {7, 6, 4, 5}, {5, 1, 3, 7}, {1, 0, 2, 3}, {5, 4, 0, 1}, };

	int chunkIndex = (removeBlock.x/width) + (removeBlock.z/width) * totalChunkWidth;
    int blockIndex = (removeBlock.x%width) + (removeBlock.z%width) * width + removeBlock.y * width * width;
	int currentBlockType = allBlock[blocks[chunkIndex][blockIndex]].blockType;
	int blockIndexR = blockIndex + chunkIndex * width * width * height;
	int blockRotation = blockData[blockIndexR]&(blockTypes[currentBlockType].diffrentRotationAmount-1);

	int blockSideIndex = 0;

    for (int i = 0; i < 6; i++)
	{
		int newX = removeBlock.x+directions[i].x;
        int newY = removeBlock.y+directions[i].y;
    	int newZ = removeBlock.z+directions[i].z;
    
    	if(newX > 0 && newX < width*totalChunkWidth && newY > 0 && newY < height && newZ > 0 && newZ < width*totalChunkWidth)
    	{
			int chunkIndexNew = (newX/width) + (newZ/width) * totalChunkWidth;
        	int blockIndexNew = (newX%width) + (newZ%width) * width + newY * width * width;
			int currentBlockType2 = allBlock[blocks[chunkIndexNew][blockIndexNew]].blockType;
			int blockIndexR2 = blockIndexNew + chunkIndexNew * width * width * height;
			int blockRotation2 = blockData[blockIndexR2]&(blockTypes[currentBlockType2].diffrentRotationAmount-1);

			if(blockTypes[currentBlockType].dir[blockRotation][i].x == 1)
    		blockSideIndex = 0;
    		else if(blockTypes[currentBlockType].dir[blockRotation][i].x == -1)
    		blockSideIndex = 1;
    		else if(blockTypes[currentBlockType].dir[blockRotation][i].y == 1)
    		blockSideIndex = 2;
    		else if(blockTypes[currentBlockType].dir[blockRotation][i].y == -1)
    		blockSideIndex = 3;
    		else if(blockTypes[currentBlockType].dir[blockRotation][i].z == 1)
    		blockSideIndex = 4;
    		else if(blockTypes[currentBlockType].dir[blockRotation][i].z == -1)
    		blockSideIndex = 5;

            if(blockTypes[currentBlockType2].otherBlocksRenderFace[blockRotation2][blockSideIndex] == 0 || (blockTypes[currentBlockType2].isTransprent == true && blockTypes[currentBlockType].isTransprent == false) || (blockTypes[currentBlockType2].isfluid == true && blockTypes[currentBlockType].isfluid == false))
			{
				screenPoint v1 = pointToScreenCorrdinates((vertices[faces[i].x].x + removeBlock.x * 10) * 100, (vertices[faces[i].x].y + removeBlock.y * 10) * 100, (vertices[faces[i].x].z + removeBlock.z * 10) * 100);
				screenPoint v2 = pointToScreenCorrdinates((vertices[faces[i].y].x + removeBlock.x * 10) * 100, (vertices[faces[i].y].y + removeBlock.y * 10) * 100, (vertices[faces[i].y].z + removeBlock.z * 10) * 100);
				screenPoint v3 = pointToScreenCorrdinates((vertices[faces[i].z].x + removeBlock.x * 10) * 100, (vertices[faces[i].z].y + removeBlock.y * 10) * 100, (vertices[faces[i].z].z + removeBlock.z * 10) * 100);
				screenPoint v4 = pointToScreenCorrdinates((vertices[faces[i].w].x + removeBlock.x * 10) * 100, (vertices[faces[i].w].y + removeBlock.y * 10) * 100, (vertices[faces[i].w].z + removeBlock.z * 10) * 100);

				if(v1.z != 65001 && v2.z != 65001 && v3.z != 65001 && v4.z != 65001)
                {
                    if((v1.x > 0 && v1.x < resX) || (v2.x > 0 && v2.x < resX) || (v3.x > 0 && v3.x < resX) || (v4.x > 0 && v4.x < resX))
                    {
                        if((v1.y > 0 && v1.y < resY) || (v2.y > 0 && v2.y < resY) || (v3.y > 0 && v3.y < resY) || (v4.y > 0 && v4.y < resY))
                        {
							int Depth = ((v1.z + v2.z + v3.z + v4.z) >> 2) - 1;

              				Vector2S V1_ = {v1.x, v1.y};
              				Vector2S V2_ = {v2.x, v2.y};
              				Vector2S V3_ = {v3.x, v3.y};
              				Vector2S V4_ = {v4.x, v4.y};

              				Vector2S AT = {0, 0};
              				Vector2S BT = {10, 0};
              				Vector2S CT = {10, 10};
              				Vector2S DT = {0, 10};

              				if(pixelSize == 2)
              				{
              				    renderTriangleTexturedBlackTransNBPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, 16, 16, &textures2[(111+destroyIndex)*256]);
              				    renderTriangleTexturedBlackTransNBPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, 16, 16, &textures2[(111+destroyIndex)*256]);
              				}
              				else
              				{
              				    renderTriangleTexturedBlackTransNBT(V1_, V2_, V3_, AT, BT, CT, Depth, 16, 16, &textures2[(111+destroyIndex)*256]);
              				    renderTriangleTexturedBlackTransNBT(V1_, V3_, V4_, AT, CT, DT, Depth, 16, 16, &textures2[(111+destroyIndex)*256]);
              				}
						}
					}
				}
			}
	 	}
	}
    
}
