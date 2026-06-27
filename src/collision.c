/* src/collision.c */
/* src/collision.c - Player collision detection (blocks, water, lava, doors) */
#include "engine.h"
#include "chunk_constants.h"
#include "blocks.h"
#include "block_types.h"

bool chenkCollisionDoor(float positionFX, float positionFY, float positionFZ)
{
    Vector3I position;
    position.x = positionFX;
    position.y = positionFY;
    position.z = positionFZ;

    Vector3I pPosition;
    pPosition.x = PPosX;
    pPosition.y = PPosY;
    pPosition.z = PPosZ;

    int chunkIndex = position.x / width + position.z / width * totalChunkWidth;
    int blockIndex = (position.x % width) + (position.z % width) * width + position.y * width * width;

    int blockRotation = blockData[chunkIndex*width*width*height + blockIndex]&0b00000011;

    int diffX = pPosition.x-position.x;
    int diffY = pPosition.y-position.y;
    int diffZ = pPosition.z-position.z;

    if(blockRotation == 0 || blockRotation == 2)
    {
        if(diffZ == 0)
        return false;
        else
        return true;
    }
    if(blockRotation == 1 || blockRotation == 3)
    {
        if(diffX == 0)
        return false;
        else
        return true;
    }

    return true;
}
bool checkCollision(float positionFX, float positionFY, float positionFZ)
{
    if(positionFY > height)
    return false;

    Vector3I position;
    position.x = positionFX;
    position.y = positionFY;
    position.z = positionFZ;

    int chunkIndex = position.x / width + position.z / width * totalChunkWidth;
    int blockIndex = (position.x % width) + (position.z % width) * width + position.y * width * width;

    if(allBlock[blocks[chunkIndex][blockIndex]].blockType == 12)
    return chenkCollisionDoor(positionFX, positionFY, positionFZ);

    return !blockTypes[allBlock[blocks[chunkIndex][blockIndex]].blockType].canWalkThrough;
}
bool checkIfInWater(float positionFX, float positionFY, float positionFZ)
{
    if(positionFY > height)
    return false;

    Vector3I position;
    position.x = positionFX;
    position.y = positionFY;
    position.z = positionFZ;

    int chunkIndex = position.x / width + position.z / width * totalChunkWidth;
    int blockIndex = (position.x % width) + (position.z % width) * width + position.y * width * width;

    if(blocks[chunkIndex][blockIndex] == 6)
    return true;
    else
    return false;
}
bool checkIfInLava(float positionFX, float positionFY, float positionFZ)
{
    if(positionFY > height)
    return false;

    Vector3I position;
    position.x = positionFX;
    position.y = positionFY;
    position.z = positionFZ;

    int chunkIndex = position.x / width + position.z / width * totalChunkWidth;
    int blockIndex = (position.x % width) + (position.z % width) * width + position.y * width * width;

    if(blocks[chunkIndex][blockIndex] == 24)
    return true;
    else
    return false;
}

//block placement
