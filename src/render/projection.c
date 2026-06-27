/* src/render/projection.c - Particle rendering, 3D-to-2D projection, object move/rotate */
#include "engine.h"


void renderParticle(float size, int mode, color_t color, screenPoint screenPixel, int fallof)
{
    int sizeOfLoopP1 = size;

    int zChanged = screenPixel.z / fallof;
    if(zChanged == 0)
    zChanged = 1;
    
    int sizeOfLoopP2 = sizeOfLoopP1 * 1000000 / zChanged / 1000;

    for (int y = screenPixel.y-sizeOfLoopP2/2; y < screenPixel.y+sizeOfLoopP2/2; y += 2)
    {
        for (int x = screenPixel.x-sizeOfLoopP2/2; x < screenPixel.x+sizeOfLoopP2/2; x += 2)
        {
            if(x > 0 && x < resX && y > 0 && y < resY)
            {
                int distanceFromCenter;
                //switch (parti.mode)
                //{
                //    case 0:
                //    distanceFromCenter = calculateDistance(parti.position.x, parti.position.y, x, y);
                //    break;
                //    case 1:
                //    distanceFromCenter = abs_int(parti.position.x-x)+abs_int(parti.position.y-y);
                //    break;
                //    case 2:
                    distanceFromCenter = 1;
                //    break;
                //}

                //if(distanceFromCenter < parti.size)
                //{
                //    if (screenPixel.z < ZBuffer[y*resX+x])
                //    {
                        *(VRAMAddress + (y + 0) * LCD_WIDTH_PX + (x + 0)) = color;
                        *(VRAMAddress + (y + 1) * LCD_WIDTH_PX + (x + 0)) = color;
                        *(VRAMAddress + (y + 0) * LCD_WIDTH_PX + (x + 1)) = color;
                        *(VRAMAddress + (y + 1) * LCD_WIDTH_PX + (x + 1)) = color;

                //        ZBuffer[y*resX+x] = screenPixel.z;
                //    }
                //}
            }
        }
    }
}

screenPoint pointToScreenCorrdinates(int xIn, int yIn, int zIn)
{
    float cameraRotationX = ToRadians(rotationX);
    float cameraRotationY = ToRadians(rotationY);

    screenPoint output;
    int cosRotX = fastCosine(cameraRotationX) * 1000;
    int sinRotX = fastSine(cameraRotationX) * 1000;
    int cosRotY = fastCosine(cameraRotationY) * 1000;
    int sinRotY = fastSine(cameraRotationY) * 1000;

    if(cosRotX == 0) cosRotX = 1;
    if(sinRotX == 0) sinRotX = 1;
    if(cosRotY == 0) cosRotY = 1;
    if(sinRotY == 0) sinRotY = 1;

    //pre-calculating values
    int cosXSinY = cosRotX * sinRotY / 1000;
    int cosXcosY = cosRotX * cosRotY / 1000;
    int sinXsinY = sinRotX * sinRotY / 1000;
    int cosYSinX = cosRotY * sinRotX / 1000;

    if(cosXSinY == 0) cosXSinY = 1;
    if(cosXcosY == 0) cosXcosY = 1;
    if(sinXsinY == 0) sinXsinY = 1;
    if(cosYSinX == 0) cosYSinX = 1;

    int resX_ = (resX - 1) / 2;
    int resY_ = (resY - 1) / 2;

    int PPosXI = PPosX * 1000;
    int PPosYI = PPosY * 1000;
    int PPosZI = PPosZ * 1000;

    if(PPosXI == 0) PPosXI = 1;
    if(PPosYI == 0) PPosYI = 1;
    if(PPosZI == 0) PPosZI = 1;
    
    int x3D = (xIn - PPosXI);
    int y3D = (yIn - PPosYI);
    int z3D = (PPosZI - zIn);

    int z = (x3D * cosXSinY - y3D * sinRotX + z3D * cosXcosY) / 1000;

    if(z > 10)
    {
        int x = (x3D * cosRotY - z3D * sinRotY);
        int y = (x3D * sinXsinY + y3D * cosRotX + z3D * cosYSinX);

        int xOnScreen = 0;
        int yOnScreen = 0;

        if(z >= 1000)
        {
            xOnScreen = ((x / z + 1000) * resX_) / 1000;
            yOnScreen = (((1000 - y / z) * 2) * resY_) / 1000 - 100;
        }
        if(z < 1000)
        {
            int zNew = 10000 / z;

            x = x / 10000;
            y = y / 10000;

            xOnScreen = ((x * zNew + 1000) * resX_) / 1000;
            yOnScreen = (((1000 - y * zNew) * 2) * resY_) / 1000 - 100;
        }

        output.x = xOnScreen;
        output.y = yOnScreen;
        output.z = z;
    }else
    output.z = -1;

    return output;
}

Vector3 forward;

//user easy usable
void moveObject(int index, float plusX, float plusY, float plusZ)
{
    allObj[index].position.x += plusX;
    allObj[index].position.y += plusY;
    allObj[index].position.z += plusZ;

    int oldActX = allObj[index].actualPosition.x;
    int oldActY = allObj[index].actualPosition.y;
    int oldActZ = allObj[index].actualPosition.z;

    allObj[index].actualPosition.x = (allObj[index].position.x * 10);
    allObj[index].actualPosition.y = (allObj[index].position.y * 10);
    allObj[index].actualPosition.z = (allObj[index].position.z * 10);

    int diffX = allObj[index].actualPosition.x - oldActX;
    int diffY = allObj[index].actualPosition.y - oldActY;
    int diffZ = allObj[index].actualPosition.z - oldActZ;

    for (int i = 0; i < allObj[index].sizeV; i++)
    {
        allObj[index].verticesIPC[i].x += diffX;
        allObj[index].verticesIPC[i].y += diffY;
        allObj[index].verticesIPC[i].z += diffZ;
    }
}
void addRotateObject(int indexGet, int indexSet, Vector3I center, float pitch, float yaw)
{
    allObj[indexSet].rotation.x = allObj[indexGet].rotation.x + pitch;
    allObj[indexSet].rotation.y = allObj[indexGet].rotation.y + yaw;

    float cYaw = myCosine(ToRadians(yaw));
    float sYaw = mySine(ToRadians(yaw));
    float cpitch = myCosine(ToRadians(pitch));
    float spitch = mySine(ToRadians(pitch));

    for (int i = 0; i < allObj[indexGet].sizeV; i++)
    {
        float diffX = allObj[indexGet].verticesIPC[i].x - center.x;
        float diffY = allObj[indexGet].verticesIPC[i].y - center.y;
        float diffZ = allObj[indexGet].verticesIPC[i].z - center.z;

        float newX = diffX * cYaw - diffZ * sYaw;
        float newZ = diffX * sYaw + diffZ * cYaw;

        float newY = diffY * cpitch - newZ * spitch;
        newZ = diffY * spitch + newZ * cpitch;

        allObj[indexSet].verticesIPC[i].x = newX + center.x;
        allObj[indexSet].verticesIPC[i].y = newY + center.y;
        allObj[indexSet].verticesIPC[i].z = newZ + center.z;
    }
}
void addRotateObjectNS(int indexGet, int indexSet, Vector3I center, float pitch, float yaw)
{
    float cYaw = myCosine(ToRadians(yaw));
    float sYaw = mySine(ToRadians(yaw));
    float cpitch = myCosine(ToRadians(pitch));
    float spitch = mySine(ToRadians(pitch));

    for (int i = 0; i < allObj[indexGet].sizeV; i++)
    {
        float diffX = allObj[indexGet].verticesIPC[i].x - center.x;
        float diffY = allObj[indexGet].verticesIPC[i].y - center.y;
        float diffZ = allObj[indexGet].verticesIPC[i].z - center.z;

        float newX = diffX * cYaw - diffZ * sYaw;
        float newZ = diffX * sYaw + diffZ * cYaw;

        float newY = diffY * cpitch - newZ * spitch;
        newZ = diffY * spitch + newZ * cpitch;

        allObj[indexSet].verticesIPC[i].x = newX + center.x;
        allObj[indexSet].verticesIPC[i].y = newY + center.y;
        allObj[indexSet].verticesIPC[i].z = newZ + center.z;
    }
}
void setRotateObject(int indexGet, int indexSet, Vector3I center, float pitch, float yaw)
{   
    float yaw_y = yaw - allObj[indexGet].rotation.y;
    while(yaw_y < 0)
    yaw_y += 360;
    float pitch_x = pitch - allObj[indexGet].rotation.x;
    while(pitch_x < 0)
    pitch_x += 360;

    float cYaw = myCosine(ToRadians(yaw_y));
    float sYaw = mySine(ToRadians(yaw_y));
    float cpitch = myCosine(ToRadians(pitch_x));
    float spitch = mySine(ToRadians(pitch_x));

    allObj[indexSet].rotation.x = pitch;
    allObj[indexSet].rotation.y = yaw;

    for (int i = 0; i < allObj[indexGet].sizeV; i++)
    {
        float diffX = allObj[indexGet].verticesIPC[i].x - center.x;
        float diffY = allObj[indexGet].verticesIPC[i].y - center.y;
        float diffZ = allObj[indexGet].verticesIPC[i].z - center.z;

        float newX = diffX * cYaw - diffZ * sYaw;
        float newZ = diffX * sYaw + diffZ * cYaw;

        float newY = diffY * cpitch - newZ * spitch;
        newZ = diffY * spitch + newZ * cpitch;

        allObj[indexSet].verticesIPC[i].x = newX + center.x;
        allObj[indexSet].verticesIPC[i].y = newY + center.y;
        allObj[indexSet].verticesIPC[i].z = newZ + center.z;
    }
}

/* TODO: move me !! */
void createParticle(Vector3 position, float size, color_t color, char transparency, char mode, int totalTime, Vector3 velocity, bool gravity)
{
    int index = -1;
    for (int i = 0; i < partiLength; i++)
    {
        if(allparticles[i].used == false){
            index = i;
            break;
        }
    }
    if(index != -1)
    {
        allparticles[index].maxTime = totalTime;
        allparticles[index].mode = mode;
        allparticles[index].position = position;
        allparticles[index].size = size;
        allparticles[index].color = color;
        allparticles[index].transparency = transparency;
        allparticles[index].velocity = velocity;
        allparticles[index].gravity = gravity;
        allparticles[index].used = true;
        allparticles[index].time = 0;
    }
}
void deleteParticle(int index)
{
    allparticles[index].used = false;
}

Vector3I getCenter(int index)
{
    Vector3I center = {0, 0, 0};
    for (int i = 0; i < allObj[index].sizeV; i++)
    {
        center.x += allObj[index].verticesIPC[i].x;
        center.y += allObj[index].verticesIPC[i].y;
        center.z += allObj[index].verticesIPC[i].z;
    }
    center.x /= allObj[index].sizeV;
    center.y /= allObj[index].sizeV;
    center.z /= allObj[index].sizeV;

    return center;
}

