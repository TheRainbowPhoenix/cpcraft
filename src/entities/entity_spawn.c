/* src/entities/entity_spawn.c - makeItem, makeSheep, makePig, load variants */
#include "engine.h"
#include "config.h"
#include "chunk_constants.h"

void makeItem(int posX, int posY, int posZ, float velocityX, float velocityY, float velocityZ, int itemIndex)
{
    int slot = -1;
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == false)
        {
            slot = i;
            break;
        }
    }

    if(slot != -1)
    {
        int index = 100 + slot;

        entityList[slot].locationOnAllObj = index;
        entityList[slot].type = 3;
        entityList[slot].state = itemIndex;
        entityList[slot].velocityX = velocityX;
        entityList[slot].velocityY = velocityY;
        entityList[slot].velocityZ = velocityZ;
        entityList[slot].active = true;
        entityList[slot].renderTextured = true;

        usedEntitys++;

        if(allItem[itemIndex].isBlock == true)
        {

            int currentBlockType = allBlock[allItem[itemIndex].blockId].blockType;

            allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * blockTypes[currentBlockType].verticesLength  );
            allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * blockTypes[currentBlockType].facesLength     );
            allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * blockTypes[currentBlockType].facesLength);
            allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * blockTypes[currentBlockType].facesLength     );
            allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * blockTypes[currentBlockType].facesLength);

            for (int i = 0; i < blockTypes[currentBlockType].verticesLength; i++)
            {
                allObj[index].verticesIPC[i].x = blockTypes[currentBlockType].vertices[0][i].x / 5 + posX;
                allObj[index].verticesIPC[i].y = blockTypes[currentBlockType].vertices[0][i].y / 5 + posY;
                allObj[index].verticesIPC[i].z = blockTypes[currentBlockType].vertices[0][i].z / 5 + posZ;
            }

            for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++)
            {
                allObj[index].triangles[i].x = blockTypes[currentBlockType].faces[i].x;
				allObj[index].triangles[i].y = blockTypes[currentBlockType].faces[i].y;
				allObj[index].triangles[i].z = blockTypes[currentBlockType].faces[i].z;
				allObj[index].triangles[i].w = blockTypes[currentBlockType].faces[i].w;

                allObj[index].normal[i] = blockTypes[currentBlockType].normal[0][i];
                allObj[index].textureSize[i] = blockTypes[currentBlockType].texureSize[i];

                if(renderTextured == true)
                allObj[index].color[i] = allBlock[allItem[itemIndex].blockId].textureIndex[i];
                else
                allObj[index].color[i] = allBlock[allItem[itemIndex].blockId].noTextureColorIndex[0][i];
            }

            allObj[index].sizeT = blockTypes[currentBlockType].facesLength;
            allObj[index].sizeV = blockTypes[currentBlockType].verticesLength;
        }
        else
        {
            allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 4     );
            allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 1     );
            allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 1);
            allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 1     );
            allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 1);

            Vector3I voxelVertices4[4] = {
                {0, 0, 0}, {0, 1, 0}, {1, 1, 1}, {1, 0, 1}
            };

            for (int i = 0; i < 4; i++)
            {
                allObj[index].verticesIPC[i].x = voxelVertices4[i].x * 6 + posX;
                allObj[index].verticesIPC[i].y = voxelVertices4[i].y * 6 + posY;
                allObj[index].verticesIPC[i].z = voxelVertices4[i].z * 6 + posZ;
            }

            allObj[index].triangles[0].x = 1;
            allObj[index].triangles[0].y = 2;
            allObj[index].triangles[0].z = 3;
            allObj[index].triangles[0].w = 0;

            allObj[index].textureSize[0] = 255;

            allObj[index].normal[0].x = 0;
            allObj[index].normal[0].y = 0;
            allObj[index].normal[0].z = 127;

            allObj[index].color[0] = allItem[itemIndex].textureIfNoBlock;

            allObj[index].sizeT = 1;
            allObj[index].sizeV = 4;
        }

        allObj[index].position.x = (float)(posX) / 10;
        allObj[index].position.y = (float)(posY) / 10;
        allObj[index].position.z = (float)(posZ) / 10;

        allObj[index].actualPosition.x = posX;
        allObj[index].actualPosition.y = posY;
        allObj[index].actualPosition.z = posZ;

        allObj[index].active = true;

        verticesLength = 0;
        triangleLength = 0;

        for (int i = 0; i < objLength; i++)
        {
            if(allObj[i].active == true)
            {
                verticesLength += allObj[i].sizeV;
                triangleLength += allObj[i].sizeT;
            }
        }
    }
}
void makeSheep2(int posX, int posY, int posZ)
{
    int slot = -1;
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == false)
        {
            slot = i;
            break;
        }
    }

    if(slot != -1)
    {
		usedEntitys++;
        int index = slot+100;
        allObj[index].active = true;

        Vector3B        vd[48] = {
            {-4, 7, -6}, {-4, 7, 6}, {-4, 13, -6}, {-4, 13, 6}, {4, 7, -6}, {4, 7, 6}, {4, 13, -6}, {4, 13, 6},
            {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
            {-3, 0, 2}, {-3, 0, 5}, {-3, 7, 2}, {-3, 7, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 7, -6}, {-3, 7, -3},
            {-1, 0, 2}, {-1, 0, 5}, {-1, 7, 2}, {-1, 7, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 7, -6}, {-1, 7, -3},
            {1, 0, 2}, {1, 0, 5}, {1, 7, 2}, {1, 7, 5}, {1, 0, -6}, {1, 0, -3}, {1, 7, -6}, {1, 7, -3},
            {3, 0, 2}, {3, 0, 5}, {3, 7, 2}, {3, 7, 5}, {3, 0, -6}, {3, 0, -3}, {3, 7, -6}, {3, 7, -3},
        };
        Vector4B        td[28] = {
            {7, 6, 2, 3}, {5, 1, 0, 4}, {5, 4, 6, 7}, {3, 2, 0, 1}, {7, 3, 1, 5}, {6, 4, 0, 2}, {11, 15, 14, 10}, {14, 13, 9, 10},
            {15, 12, 13, 14}, {12, 8, 9, 13}, {8, 11, 10, 9}, {32, 34, 42, 40}, {24, 26, 27, 25}, {16, 18, 26, 24}, {20, 22, 30, 28}, {25, 27, 19, 17},
            {21, 23, 22, 20}, {40, 42, 43, 41}, {36, 38, 46, 44}, {37, 39, 38, 36}, {28, 30, 31, 29}, {33, 35, 34, 32}, {29, 31, 23, 21}, {44, 46, 47, 45},
            {41, 43, 35, 33}, {17, 19, 18, 16}, {45, 47, 39, 37}, {12, 15, 11, 8},
        };
        Vector3B        nd[28] = {
            {0, 127, 0}, {0, -127, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {0, 0, -127}, {0, 127, 0}, {0, 0, -127},
            {127, 0, 0}, {0, -127, 0}, {-127, 0, 0}, {0, 0, -127}, {127, 0, 0}, {0, 0, -127}, {0, 0, -127}, {0, 0, 127},
            {-127, 0, 0}, {127, 0, 0}, {0, 0, -127}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {127, 0, 0},
            {0, 0, 127}, {-127, 0, 0}, {0, 0, 127}, {0, 0, 127},
        };
        unsigned char  textureSize[28] = {
            0xf9, 0xf9, 0xf7, 0xf7, 0x79, 0x79, 0x55, 0x55,
            0x55, 0x55, 0x55, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
            0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
            0xB3, 0xB3, 0xB3, 0xff,
        };
        unsigned char  textureIndex[28] = {
            130, 130, 130, 130, 130, 130, 130, 130,
            130, 130, 130, 131, 131, 131, 131, 131,
            131, 131, 131, 131, 131, 131, 131, 131,
            131, 131, 131, 132,
        };
        color_t               color[28] = {
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0x838c,
        };

        allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 48     );
        allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 28     );
        allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
        allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 28     );
        allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
        allObj[index].solidColor  = tlsf_malloc(tlsf, sizeof(color_t) * 28      );

        for (int i = 0; i < 48; i++)
        {
            allObj[index].verticesIPC[i].x = vd[i].x + posX * 10 + 5;
            allObj[index].verticesIPC[i].y = vd[i].y + posY * 10;
            allObj[index].verticesIPC[i].z = vd[i].z + posZ * 10 + 5;
        }
        for (int i = 0; i < 28; i++)
        {
            allObj[index].triangles[i].x = td[i].x;
			allObj[index].triangles[i].y = td[i].y;
			allObj[index].triangles[i].z = td[i].z;
			allObj[index].triangles[i].w = td[i].w;

            allObj[index].solidColor[i] = color[i];
            allObj[index].color[i] = textureIndex[i];
            allObj[index].textureSize[i] = textureSize[i];
            allObj[index].normal[i] = nd[i];
        }

		verticesLength += 48;
		triangleLength += 28;

        allObj[index].sizeV = 48;
        allObj[index].sizeT = 28;

        allObj[index].position.x = posX+0.5;
        allObj[index].position.y = posY;
        allObj[index].position.z = posZ+0.5;

		allObj[index].actualPosition.x = allObj[index].position.x*10;
        allObj[index].actualPosition.y = allObj[index].position.y*10;
        allObj[index].actualPosition.z = allObj[index].position.z*10;

        entityList[slot].active = true;
        entityList[slot].locationOnAllObj = index;
		entityList[slot].health = 8;
        entityList[slot].type = 1;
        entityList[slot].state = 0;
		entityList[slot].hasFur = true;
        entityList[slot].renderTextured = true;
    }

    int slot2 = -1;
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == false)
        {
            slot2 = i;
            break;
        }
    }

    if(slot2 != -1)
    {
		usedEntitys++;
        int index = slot2+100;
        allObj[index].active = false;

        Vector3B        vd[48] = {
            {-3, 8, -5}, {-3, 8, 5}, {-3, 12, -5}, {-3, 12, 5}, {3, 8, -5}, {3, 8, 5}, {3, 12, -5}, {3, 12, 5},
            {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
            {-3, 0, 2}, {-3, 0, 5}, {-3, 8, 2}, {-3, 8, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 8, -6}, {-3, 8, -3},
            {-1, 0, 2}, {-1, 0, 5}, {-1, 8, 2}, {-1, 8, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 8, -6}, {-1, 8, -3},
            {1, 0, 2}, {1, 0, 5}, {1, 8, 2}, {1, 8, 5}, {1, 0, -6}, {1, 0, -3}, {1, 8, -6}, {1, 8, -3},
            {3, 0, 2}, {3, 0, 5}, {3, 8, 2}, {3, 8, 5}, {3, 0, -6}, {3, 0, -3}, {3, 8, -6}, {3, 8, -3},
        };
        Vector4B        td[32] = {
            {7, 6, 2, 3}, {5, 1, 0, 4}, {5, 4, 6, 7}, {3, 2, 0, 1}, {7, 3, 1, 5}, {6, 4, 0, 2}, {11, 15, 14, 10}, {14, 13, 9, 10},
            {15, 12, 13, 14}, {12, 8, 9, 13}, {8, 11, 10, 9}, {32, 34, 42, 40}, {24, 26, 27, 25}, {16, 18, 26, 24}, {20, 22, 30, 28}, {25, 27, 19, 17},
            {21, 23, 22, 20}, {40, 42, 43, 41}, {36, 38, 46, 44}, {37, 39, 38, 36}, {28, 30, 31, 29}, {33, 35, 34, 32}, {29, 31, 23, 21}, {44, 46, 47, 45},
            {41, 43, 35, 33}, {17, 19, 18, 16}, {45, 47, 39, 37}, {12, 15, 11, 8}, {42, 34, 35, 43}, {46, 38, 39, 47}, {31, 30, 22, 23}, {27, 26, 18, 19},
        };
        Vector3B        nd[32] = {
            {0, 127, 0}, {0, -127, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {0, 0, -127}, {0, 127, 0}, {0, 0, -127},
            {127, 0, 0}, {0, -127, 0}, {-127, 0, 0}, {0, 0, -127}, {127, 0, 0}, {0, 0, -127}, {0, 0, -127}, {0, 0, 127},
            {-127, 0, 0}, {127, 0, 0}, {0, 0, -127}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {127, 0, 0},
            {0, 0, 127}, {-127, 0, 0}, {0, 0, 127}, {0, 0, 127}, {0, 127, 0}, {0, 127, 0}, {0, 127, 0}, {0, 127, 0},
        };
        unsigned char  textureSize[32] = {
            0xf9, 0xf9, 0xf7, 0xf7, 0x79, 0x79, 0x55, 0x55,
            0x55, 0x55, 0x55, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
            0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
            0xB3, 0xB3, 0xB3, 0xff, 0x33, 0x33, 0x33, 0x33
        };
        unsigned char  textureIndex[32] = {
            133, 133, 133, 133, 133, 133, 130, 130,
            130, 130, 130, 131, 131, 131, 131, 131,
            131, 131, 131, 131, 131, 131, 131, 131,
            131, 131, 131, 132, 130, 130, 130, 130
        };
        color_t               color[32] = {
            0x838c, 0x838c, 0x838c, 0x838c, 0x838c, 0x838c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0x838c, 0x838c, 0x838c, 0x838c, 0x838c
        };

        allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 48     );
        allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 32     );
        allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 32);
        allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 32     );
        allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 32);
        allObj[index].solidColor  = tlsf_malloc(tlsf, sizeof(color_t) * 32      );

        for (int i = 0; i < 48; i++)
        {
            allObj[index].verticesIPC[i].x = vd[i].x + posX * 10 + 5;
            allObj[index].verticesIPC[i].y = vd[i].y + posY * 10;
            allObj[index].verticesIPC[i].z = vd[i].z + posZ * 10 + 5;
        }
        for (int i = 0; i < 32; i++)
        {
            allObj[index].triangles[i].x = td[i].x;
			allObj[index].triangles[i].y = td[i].y;
			allObj[index].triangles[i].z = td[i].z;
			allObj[index].triangles[i].w = td[i].w;

            allObj[index].solidColor[i] = color[i];
            allObj[index].color[i] = textureIndex[i];
            allObj[index].textureSize[i] = textureSize[i];
            allObj[index].normal[i] = nd[i];
        }

		verticesLength += 48;
		triangleLength += 32;

        allObj[index].sizeT = 32;
        allObj[index].sizeV = 48;

        allObj[index].position.x = posX+0.5;
        allObj[index].position.y = posY;
        allObj[index].position.z = posZ+0.5;

		allObj[index].actualPosition.x = allObj[index].position.x*10;
        allObj[index].actualPosition.y = allObj[index].position.y*10;
        allObj[index].actualPosition.z = allObj[index].position.z*10;

        entityList[slot2].active = true;
        entityList[slot2].locationOnAllObj = index;
        entityList[slot2].type = 255;
        entityList[slot2].renderTextured = true;

        entityList[slot].extraEntity = slot2;
    }
}
void makePig(int posX, int posY, int posZ)
{
    int slot = -1;
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == false)
        {
            slot = i;
            break;
        }
    }

    if(slot != -1)
    {
		usedEntitys++;
        int index = slot+100;
        allObj[index].active = true;

        Vector3B        vd[48] = {
            {-3, 6, 10}, {3, 6, 10}, {-3, 6, 4}, {3, 6, 4}, {-3, 11, 10}, {3, 11, 10}, {-3, 11, 4}, {3, 11, 4},
            {-4, 4, 5}, {4, 4, 5}, {-4, 4, -6}, {4, 4, -6}, {-4, 10, 5}, {4, 10, 5}, {-4, 10, -6}, {4, 10, -6},
            {-4, 0, 2}, {-1, 0, 2}, {-4, 4, 2}, {-1, 4, 2}, {-4, 0, 5}, {-1, 0, 5}, {-4, 4, 5}, {-1, 4, 5},
            {1, 0, 2}, {4, 0, 2}, {1, 4, 2}, {4, 4, 2}, {1, 0, 5}, {4, 0, 5}, {1, 4, 5}, {4, 4, 5},
            {-4, 0, -7}, {-1, 0, -7}, {-4, 4, -7}, {-1, 4, -7}, {-4, 0, -4}, {-1, 0, -4}, {-4, 4, -4}, {-1, 4, -4},
            {1, 0, -7}, {4, 0, -7}, {1, 4, -7}, {4, 4, -7}, {1, 0, -4}, {4, 0, -4}, {1, 4, -4}, {4, 4, -4},
        };
        Vector4B        td[28] = {
            {1, 5, 4, 0}, {5, 1, 3, 7}, {6, 2, 0, 4}, {3, 1, 0, 2}, {5, 7, 6, 4}, {2, 6, 7, 3}, {9, 13, 12, 8}, {8, 10, 11, 9},
            {13, 15, 14, 12}, {9, 11, 15, 13}, {10, 8, 12, 14}, {10, 14, 15, 11}, {16, 18, 19, 17}, {20, 21, 23, 22}, {24, 26, 27, 25}, {28, 29, 31, 30},
            {32, 34, 35, 33}, {36, 37, 39, 38}, {40, 42, 43, 41}, {44, 45, 47, 46}, {17, 19, 23, 21}, {18, 16, 20, 22}, {25, 27, 31, 29}, {26, 24, 28, 30},
            {33, 35, 39, 37}, {34, 32, 36, 38}, {41, 43, 47, 45}, {42, 40, 44, 46},
        };
        Vector3B        nd[28] = {
            {0, 0, 127}, {127, 0, 0}, {-127, 0, 0}, {0, -127, 0}, {0, 127, 0}, {0, 0, -127}, {0, 0, 127}, {0, -127, 0},
            {0, 127, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, -127}, {0, 0, -127}, {0, 0, 127}, {0, 0, -127}, {0, 0, 127},
            {0, 0, -127}, {0, 0, 127}, {0, 0, -127}, {0, 0, 127}, {127, 0, 0}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0},
            {127, 0, 0}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0},
        };
        unsigned char  textureIndex[28] = {
            123, 124, 125, 126, 126, 126, 126, 127,
            127, 127, 127, 128, 129, 129, 129, 129,
            129, 129, 129, 129, 129, 129, 129, 129,
            129, 129, 129, 129,
        };
        unsigned char  textureSize[28] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7,
            0xf7, 0xf7, 0xf7, 0xff, 0x53, 0x53, 0x53, 0x53,
            0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53,
            0x53, 0x53, 0x53, 0x53,
        };
        color_t     color[28] = {
            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 
        };
        
        allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 48     );
        allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 28     );
        allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
        allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 28     );
        allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
        allObj[index].solidColor  = tlsf_malloc(tlsf, sizeof(color_t) *       28);

        for (int i = 0; i < 48; i++)
        {
            allObj[index].verticesIPC[i].x = vd[i].x + posX * 10 + 5;
            allObj[index].verticesIPC[i].y = vd[i].y + posY * 10;
            allObj[index].verticesIPC[i].z = vd[i].z + posZ * 10 + 5;
        }
        for (int i = 0; i < 28; i++)
        {
            allObj[index].triangles[i].x = td[i].x;
        	allObj[index].triangles[i].y = td[i].y;
        	allObj[index].triangles[i].z = td[i].z;
        	allObj[index].triangles[i].w = td[i].w;

            allObj[index].color[i] = textureIndex[i];
            allObj[index].solidColor[i] = color[i];
            allObj[index].textureSize[i] = textureSize[i];
            allObj[index].normal[i] = nd[i];
        }

		verticesLength += 48;
		triangleLength += 28;

        allObj[index].sizeT = 28;
        allObj[index].sizeV = 48;

        allObj[index].position.x = posX+0.5;
        allObj[index].position.y = posY;
        allObj[index].position.z = posZ+0.5;

		allObj[index].actualPosition.x = allObj[index].position.x*10;
        allObj[index].actualPosition.y = allObj[index].position.y*10;
        allObj[index].actualPosition.z = allObj[index].position.z*10;

        entityList[slot].active = true;
        entityList[slot].locationOnAllObj = index;
		entityList[slot].health = 8;
        entityList[slot].type = 2;
        entityList[slot].state = 0;
        entityList[slot].renderTextured = true;
    }
}

void makeSheep2Load(int posX, int posY, int posZ, int slot, bool hasFur)
{
	usedEntitys++;
    int index = slot+100;
    allObj[index].active = true;

    Vector3B        vd[48] = {
        {-4, 7, -6}, {-4, 7, 6}, {-4, 13, -6}, {-4, 13, 6}, {4, 7, -6}, {4, 7, 6}, {4, 13, -6}, {4, 13, 6},
        {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
        {-3, 0, 2}, {-3, 0, 5}, {-3, 7, 2}, {-3, 7, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 7, -6}, {-3, 7, -3},
        {-1, 0, 2}, {-1, 0, 5}, {-1, 7, 2}, {-1, 7, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 7, -6}, {-1, 7, -3},
        {1, 0, 2}, {1, 0, 5}, {1, 7, 2}, {1, 7, 5}, {1, 0, -6}, {1, 0, -3}, {1, 7, -6}, {1, 7, -3},
        {3, 0, 2}, {3, 0, 5}, {3, 7, 2}, {3, 7, 5}, {3, 0, -6}, {3, 0, -3}, {3, 7, -6}, {3, 7, -3},
    };
    Vector4B        td[28] = {
        {7, 6, 2, 3}, {5, 1, 0, 4}, {5, 4, 6, 7}, {3, 2, 0, 1}, {7, 3, 1, 5}, {6, 4, 0, 2}, {11, 15, 14, 10}, {14, 13, 9, 10},
        {15, 12, 13, 14}, {12, 8, 9, 13}, {8, 11, 10, 9}, {32, 34, 42, 40}, {24, 26, 27, 25}, {16, 18, 26, 24}, {20, 22, 30, 28}, {25, 27, 19, 17},
        {21, 23, 22, 20}, {40, 42, 43, 41}, {36, 38, 46, 44}, {37, 39, 38, 36}, {28, 30, 31, 29}, {33, 35, 34, 32}, {29, 31, 23, 21}, {44, 46, 47, 45},
        {41, 43, 35, 33}, {17, 19, 18, 16}, {45, 47, 39, 37}, {12, 15, 11, 8},
    };
    Vector3B        nd[28] = {
        {0, 127, 0}, {0, -127, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {0, 0, -127}, {0, 127, 0}, {0, 0, -127},
        {127, 0, 0}, {0, -127, 0}, {-127, 0, 0}, {0, 0, -127}, {127, 0, 0}, {0, 0, -127}, {0, 0, -127}, {0, 0, 127},
        {-127, 0, 0}, {127, 0, 0}, {0, 0, -127}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {127, 0, 0},
        {0, 0, 127}, {-127, 0, 0}, {0, 0, 127}, {0, 0, 127},
    };
    unsigned char  textureSize[28] = {
        0xf9, 0xf9, 0xf7, 0xf7, 0x79, 0x79, 0x55, 0x55,
        0x55, 0x55, 0x55, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
        0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
        0xB3, 0xB3, 0xB3, 0xff,
    };
    unsigned char  textureIndex[28] = {
        130, 130, 130, 130, 130, 130, 130, 130,
        130, 130, 130, 131, 131, 131, 131, 131,
        131, 131, 131, 131, 131, 131, 131, 131,
        131, 131, 131, 132,
    };
    color_t               color[28] = {
        0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
        0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
        0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
        0xe73c, 0xe73c, 0xe73c, 0x838c,
    };

    allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 48     );
    allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 28     );
    allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
    allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 28     );
    allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
    allObj[index].solidColor  = tlsf_malloc(tlsf, sizeof(color_t) * 28      );

    for (int i = 0; i < 48; i++)
    {
        allObj[index].verticesIPC[i].x = vd[i].x + posX * 10 + 5;
        allObj[index].verticesIPC[i].y = vd[i].y + posY * 10;
        allObj[index].verticesIPC[i].z = vd[i].z + posZ * 10 + 5;
    }
    for (int i = 0; i < 28; i++)
    {
        allObj[index].triangles[i].x = td[i].x;
        allObj[index].triangles[i].y = td[i].y;
        allObj[index].triangles[i].z = td[i].z;
        allObj[index].triangles[i].w = td[i].w;

        allObj[index].solidColor[i] = color[i];
        allObj[index].color[i] = textureIndex[i];
        allObj[index].textureSize[i] = textureSize[i];
        allObj[index].normal[i] = nd[i];
    }

    verticesLength += 48;
    triangleLength += 28;

    allObj[index].sizeV = 48;
    allObj[index].sizeT = 28;

	allObj[index].actualPosition.x = posX + 0.5;
    allObj[index].actualPosition.y = posY;
    allObj[index].actualPosition.z = posZ + 0.5;

    allObj[index].position.x = (float)allObj[index].actualPosition.x/10;
    allObj[index].position.y = (float)allObj[index].actualPosition.y/10;
    allObj[index].position.z = (float)allObj[index].actualPosition.z/10;

	entityList[slot].active = true;
    entityList[slot].locationOnAllObj = index;
	entityList[slot].health = 8;
    entityList[slot].renderTextured = false;

	usedEntitys++;

	int slot2 = -1;
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == false)
        {
            slot2 = i;
            break;
        }
    }

    if(slot2 != -1)
    {
		index = slot2+100;

		if(hasFur == true)
		allObj[index].active = true;

		Vector3B        vd[48] = {
            {-3, 8, -5}, {-3, 8, 5}, {-3, 12, -5}, {-3, 12, 5}, {3, 8, -5}, {3, 8, 5}, {3, 12, -5}, {3, 12, 5},
            {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
            {-3, 0, 2}, {-3, 0, 5}, {-3, 8, 2}, {-3, 8, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 8, -6}, {-3, 8, -3},
            {-1, 0, 2}, {-1, 0, 5}, {-1, 8, 2}, {-1, 8, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 8, -6}, {-1, 8, -3},
            {1, 0, 2}, {1, 0, 5}, {1, 8, 2}, {1, 8, 5}, {1, 0, -6}, {1, 0, -3}, {1, 8, -6}, {1, 8, -3},
            {3, 0, 2}, {3, 0, 5}, {3, 8, 2}, {3, 8, 5}, {3, 0, -6}, {3, 0, -3}, {3, 8, -6}, {3, 8, -3},
        };
        Vector4B        td[32] = {
            {7, 6, 2, 3}, {5, 1, 0, 4}, {5, 4, 6, 7}, {3, 2, 0, 1}, {7, 3, 1, 5}, {6, 4, 0, 2}, {11, 15, 14, 10}, {14, 13, 9, 10},
            {15, 12, 13, 14}, {12, 8, 9, 13}, {8, 11, 10, 9}, {32, 34, 42, 40}, {24, 26, 27, 25}, {16, 18, 26, 24}, {20, 22, 30, 28}, {25, 27, 19, 17},
            {21, 23, 22, 20}, {40, 42, 43, 41}, {36, 38, 46, 44}, {37, 39, 38, 36}, {28, 30, 31, 29}, {33, 35, 34, 32}, {29, 31, 23, 21}, {44, 46, 47, 45},
            {41, 43, 35, 33}, {17, 19, 18, 16}, {45, 47, 39, 37}, {12, 15, 11, 8}, {42, 34, 35, 43}, {46, 38, 39, 47}, {31, 30, 22, 23}, {27, 26, 18, 19},
        };
        Vector3B        nd[32] = {
            {0, 127, 0}, {0, -127, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {0, 0, -127}, {0, 127, 0}, {0, 0, -127},
            {127, 0, 0}, {0, -127, 0}, {-127, 0, 0}, {0, 0, -127}, {127, 0, 0}, {0, 0, -127}, {0, 0, -127}, {0, 0, 127},
            {-127, 0, 0}, {127, 0, 0}, {0, 0, -127}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, 127}, {127, 0, 0},
            {0, 0, 127}, {-127, 0, 0}, {0, 0, 127}, {0, 0, 127}, {0, 127, 0}, {0, 127, 0}, {0, 127, 0}, {0, 127, 0},
        };
        unsigned char  textureSize[32] = {
            0xf9, 0xf9, 0xf7, 0xf7, 0x79, 0x79, 0x55, 0x55,
            0x55, 0x55, 0x55, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
            0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
            0xB3, 0xB3, 0xB3, 0xff, 0x33, 0x33, 0x33, 0x33
        };
        unsigned char  textureIndex[32] = {
            133, 133, 133, 133, 133, 133, 130, 130,
            130, 130, 130, 131, 131, 131, 131, 131,
            131, 131, 131, 131, 131, 131, 131, 131,
            131, 131, 131, 132, 130, 130, 130, 130
        };
        color_t               color[32] = {
            0x838c, 0x838c, 0x838c, 0x838c, 0x838c, 0x838c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
            0xe73c, 0xe73c, 0xe73c, 0x838c, 0x838c, 0x838c, 0x838c, 0x838c
        };

        allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 48     );
        allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 32     );
        allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 32);
        allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 32     );
        allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 32);
        allObj[index].solidColor  = tlsf_malloc(tlsf, sizeof(color_t) * 32      );

        for (int i = 0; i < 48; i++)
        {
            allObj[index].verticesIPC[i].x = vd[i].x + posX * 10 + 5;
            allObj[index].verticesIPC[i].y = vd[i].y + posY * 10;
            allObj[index].verticesIPC[i].z = vd[i].z + posZ * 10 + 5;
        }
        for (int i = 0; i < 32; i++)
        {
            allObj[index].triangles[i].x = td[i].x;
			allObj[index].triangles[i].y = td[i].y;
			allObj[index].triangles[i].z = td[i].z;
			allObj[index].triangles[i].w = td[i].w;

            allObj[index].solidColor[i] = color[i];
            allObj[index].color[i] = textureIndex[i];
            allObj[index].textureSize[i] = textureSize[i];
            allObj[index].normal[i] = nd[i];
        }

		verticesLength += 48;
		triangleLength += 32;

        allObj[index].sizeT = 32;
        allObj[index].sizeV = 48;

		allObj[index].actualPosition.x = posX + 0.5;
		allObj[index].actualPosition.y = posY;
		allObj[index].actualPosition.z = posZ + 0.5;

		allObj[index].position.x = (float)allObj[index].actualPosition.x/10;
		allObj[index].position.y = (float)allObj[index].actualPosition.y/10;
		allObj[index].position.z = (float)allObj[index].actualPosition.z/10;

		entityList[slot2].active = true;
		entityList[slot2].locationOnAllObj = index;
		entityList[slot2].type = 255;
        entityList[slot2].renderTextured = false;

		entityList[slot].extraEntity = slot2;
	}
}
void makeItemLoad(int posX, int posY, int posZ, float velocityX, float velocityY, float velocityZ, int itemIndex, int slot)
{
    int index = 100 + slot;

    entityList[slot].locationOnAllObj = index;
    entityList[slot].type = 3;
    entityList[slot].state = itemIndex;
    entityList[slot].velocityX = velocityX;
    entityList[slot].velocityY = velocityY;
    entityList[slot].velocityZ = velocityZ;
    entityList[slot].renderTextured = true;

    usedEntitys++;

    if(allItem[itemIndex].isBlock == true)
    {
        int currentBlockType = allBlock[allItem[itemIndex].blockId].blockType;

        allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * blockTypes[currentBlockType].verticesLength  );
        allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * blockTypes[currentBlockType].facesLength     );
        allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * blockTypes[currentBlockType].facesLength);
        allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * blockTypes[currentBlockType].facesLength     );
        allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * blockTypes[currentBlockType].facesLength);

        for (int i = 0; i < blockTypes[currentBlockType].verticesLength; i++)
        {
            allObj[index].verticesIPC[i].x = blockTypes[currentBlockType].vertices[0][i].x / 5 + posX;
            allObj[index].verticesIPC[i].y = blockTypes[currentBlockType].vertices[0][i].y / 5 + posY;
            allObj[index].verticesIPC[i].z = blockTypes[currentBlockType].vertices[0][i].z / 5 + posZ;
        }

        for (int i = 0; i < blockTypes[currentBlockType].facesLength; i++)
        {
            allObj[index].triangles[i].x = blockTypes[currentBlockType].faces[i].x;
			allObj[index].triangles[i].y = blockTypes[currentBlockType].faces[i].y;
			allObj[index].triangles[i].z = blockTypes[currentBlockType].faces[i].z;
			allObj[index].triangles[i].w = blockTypes[currentBlockType].faces[i].w;

            allObj[index].normal[i] = blockTypes[currentBlockType].normal[0][i];
            allObj[index].textureSize[i] = blockTypes[currentBlockType].texureSize[i];

            if(renderTextured == true)
            allObj[index].color[i] = allBlock[allItem[itemIndex].blockId].textureIndex[i];
            else
            allObj[index].color[i] = allBlock[allItem[itemIndex].blockId].noTextureColorIndex[0][i];
        }

        allObj[index].sizeT = blockTypes[currentBlockType].facesLength;
        allObj[index].sizeV = blockTypes[currentBlockType].verticesLength;
    }
    else
    {
        allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 4     );
        allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 1     );
        allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 1);
        allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 1     );
        allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 1);

        Vector3I voxelVertices4[4] = {
            {0, 0, 0}, {0, 1, 0}, {1, 1, 1}, {1, 0, 1}
        };

        for (int i = 0; i < 4; i++)
        {
            allObj[index].verticesIPC[i].x = voxelVertices4[i].x * 6 + posX;
            allObj[index].verticesIPC[i].y = voxelVertices4[i].y * 6 + posY;
            allObj[index].verticesIPC[i].z = voxelVertices4[i].z * 6 + posZ;
        }

        allObj[index].triangles[0].x = 1;
        allObj[index].triangles[0].y = 2;
        allObj[index].triangles[0].z = 3;
        allObj[index].triangles[0].w = 0;

        allObj[index].textureSize[0] = 255;

        allObj[index].normal[0].x = 0;
        allObj[index].normal[0].y = 0;
        allObj[index].normal[0].z = 127;

        allObj[index].color[0] = allItem[itemIndex].textureIfNoBlock;

        allObj[index].sizeT = 1;
        allObj[index].sizeV = 4;
    }

    allObj[index].position.x = (float)(posX) / 10;
    allObj[index].position.y = (float)(posY) / 10;
    allObj[index].position.z = (float)(posZ) / 10;

    allObj[index].actualPosition.x = posX;
    allObj[index].actualPosition.y = posY;
    allObj[index].actualPosition.z = posZ;

    allObj[index].active = true;

    verticesLength = 0;
    triangleLength = 0;

    for (int i = 0; i < objLength; i++)
    {
        if(allObj[i].active == true)
        {
            verticesLength += allObj[i].sizeV;
            triangleLength += allObj[i].sizeT;
        }
    }
}
void makePigLoad(int posX, int posY, int posZ, int slot)
{
	usedEntitys++;
    int index = slot+100;
    allObj[index].active = true;

    Vector3B        vd[48] = {
        {-3, 6, 10}, {3, 6, 10}, {-3, 6, 4}, {3, 6, 4}, {-3, 11, 10}, {3, 11, 10}, {-3, 11, 4}, {3, 11, 4},
        {-4, 4, 5}, {4, 4, 5}, {-4, 4, -6}, {4, 4, -6}, {-4, 10, 5}, {4, 10, 5}, {-4, 10, -6}, {4, 10, -6},
        {-4, 0, 2}, {-1, 0, 2}, {-4, 4, 2}, {-1, 4, 2}, {-4, 0, 5}, {-1, 0, 5}, {-4, 4, 5}, {-1, 4, 5},
        {1, 0, 2}, {4, 0, 2}, {1, 4, 2}, {4, 4, 2}, {1, 0, 5}, {4, 0, 5}, {1, 4, 5}, {4, 4, 5},
        {-4, 0, -7}, {-1, 0, -7}, {-4, 4, -7}, {-1, 4, -7}, {-4, 0, -4}, {-1, 0, -4}, {-4, 4, -4}, {-1, 4, -4},
        {1, 0, -7}, {4, 0, -7}, {1, 4, -7}, {4, 4, -7}, {1, 0, -4}, {4, 0, -4}, {1, 4, -4}, {4, 4, -4},
    };
    Vector4B        td[28] = {
        {1, 5, 4, 0}, {5, 1, 3, 7}, {6, 2, 0, 4}, {3, 1, 0, 2}, {5, 7, 6, 4}, {2, 6, 7, 3}, {9, 13, 12, 8}, {8, 10, 11, 9},
        {13, 15, 14, 12}, {9, 11, 15, 13}, {10, 8, 12, 14}, {10, 14, 15, 11}, {16, 18, 19, 17}, {20, 21, 23, 22}, {24, 26, 27, 25}, {28, 29, 31, 30},
        {32, 34, 35, 33}, {36, 37, 39, 38}, {40, 42, 43, 41}, {44, 45, 47, 46}, {17, 19, 23, 21}, {18, 16, 20, 22}, {25, 27, 31, 29}, {26, 24, 28, 30},
        {33, 35, 39, 37}, {34, 32, 36, 38}, {41, 43, 47, 45}, {42, 40, 44, 46},
    };
    Vector3B        nd[28] = {
        {0, 0, 127}, {127, 0, 0}, {-127, 0, 0}, {0, -127, 0}, {0, 127, 0}, {0, 0, -127}, {0, 0, 127}, {0, -127, 0},
        {0, 127, 0}, {127, 0, 0}, {-127, 0, 0}, {0, 0, -127}, {0, 0, -127}, {0, 0, 127}, {0, 0, -127}, {0, 0, 127},
        {0, 0, -127}, {0, 0, 127}, {0, 0, -127}, {0, 0, 127}, {127, 0, 0}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0},
        {127, 0, 0}, {-127, 0, 0}, {127, 0, 0}, {-127, 0, 0},
    };
    unsigned char  textureIndex[28] = {
        123, 124, 125, 126, 126, 126, 126, 127,
        127, 127, 127, 128, 129, 129, 129, 129,
        129, 129, 129, 129, 129, 129, 129, 129,
        129, 129, 129, 129,
    };
    unsigned char  textureSize[28] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7,
        0xf7, 0xf7, 0xf7, 0xff, 0x53, 0x53, 0x53, 0x53,
        0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53,
        0x53, 0x53, 0x53, 0x53,
    };
    color_t     color[28] = {
        0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
        0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
        0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
        0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 
    };
    
    allObj[index].verticesIPC = tlsf_malloc(tlsf, sizeof(Vector3S) * 48     );
    allObj[index].triangles   = tlsf_malloc(tlsf, sizeof(Vector4S) * 28     );
    allObj[index].color       = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
    allObj[index].normal      = tlsf_malloc(tlsf, sizeof(Vector3B) * 28     );
    allObj[index].textureSize = tlsf_malloc(tlsf, sizeof(unsigned char) * 28);
    allObj[index].solidColor  = tlsf_malloc(tlsf, sizeof(color_t) *       28);

    for (int i = 0; i < 48; i++)
    {
        allObj[index].verticesIPC[i].x = vd[i].x + posX * 10 + 5;
        allObj[index].verticesIPC[i].y = vd[i].y + posY * 10;
        allObj[index].verticesIPC[i].z = vd[i].z + posZ * 10 + 5;
    }
    for (int i = 0; i < 28; i++)
    {
        allObj[index].triangles[i].x = td[i].x;
        allObj[index].triangles[i].y = td[i].y;
        allObj[index].triangles[i].z = td[i].z;
        allObj[index].triangles[i].w = td[i].w;

        allObj[index].color[i] = textureIndex[i];
        allObj[index].solidColor[i] = color[i];
        allObj[index].textureSize[i] = textureSize[i];
        allObj[index].normal[i] = nd[i];
    }

	verticesLength += 48;
	triangleLength += 28;

    allObj[index].sizeT = 28;
    allObj[index].sizeV = 48;

    allObj[index].position.x = posX+0.5;
    allObj[index].position.y = posY;
    allObj[index].position.z = posZ+0.5;

	allObj[index].actualPosition.x = allObj[index].position.x*10;
    allObj[index].actualPosition.y = allObj[index].position.y*10;
    allObj[index].actualPosition.z = allObj[index].position.z*10;

    entityList[slot].active = true;
    entityList[slot].locationOnAllObj = index;
	entityList[slot].health = 8;
    entityList[slot].renderTextured = true;
}

//other
