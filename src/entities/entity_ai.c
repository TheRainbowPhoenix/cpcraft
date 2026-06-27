/* src/entities/entity_ai.c - Entity update logic (sheep, pig, item AI) */
#include "engine.h"
#include "config.h"
#include "chunk_constants.h"


void updateEntitys()
{
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == true)
        {
			if(entityList[i].type == 1) //sheep ai
			{
				//state 0=standing still, 1=walking, 2=grazing, 3=following player, 4=running, 5=just hit
				int objectIndex = entityList[i].locationOnAllObj;
				int objectIndex2 = entityList[entityList[i].extraEntity].locationOnAllObj;

				int chunkX = allObj[objectIndex].position.x/width;
				int chunkZ = allObj[objectIndex].position.z/width;

				int posX = (int)(allObj[objectIndex].position.x)%width;
				int posY = (int)(allObj[objectIndex].position.y);
				int posZ = (int)(allObj[objectIndex].position.z)%width;

				if(chunk2DActive[chunkX + chunkZ*totalChunkWidth] == true)
				{
                    if(entityList[i].hasFur == true)
                    {
					    allObj[objectIndex].active = true;
                        allObj[objectIndex2].active = false;
                    }
					else
                    {
                        allObj[objectIndex].active = false;
					    allObj[objectIndex2].active = true;
                    }

					if(entityList[i].state == 5)
					{
						//correct the colors
                        if(survival == true)
						entityList[i].state = 4;

						color_t               color1[28] = {
                            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
                            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
                            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
                            0xe73c, 0xe73c, 0xe73c, 0x838c,
                        };

						for (int j = 0; j < 28; j++)
						allObj[objectIndex].solidColor[j] = color1[j];

						color_t               color2[32] = {
                            0x838c, 0x838c, 0x838c, 0x838c, 0x838c, 0x838c, 0xe73c, 0xe73c,
                            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
                            0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
                            0xe73c, 0xe73c, 0xe73c, 0x838c, 0x838c, 0x838c, 0x838c, 0x838c
                        };

						for (int j = 0; j < 32; j++)
						allObj[objectIndex2].solidColor[j] = color2[j];

                        entityList[i].renderTextured = true;
                        entityList[entityList[i].extraEntity].renderTextured = true;
					}

					if(entityList[i].state != 4)
					{
						if(entityList[i].moveTimer < 0)
						{
							char randomNumber = generate_random(RTC_GetTicks()+i, 6);

							if(randomNumber < 4)
							{
								entityList[i].state = 1;
								entityList[i].moveTimer = generate_random(RTC_GetTicks()+i+1, 4)+2;

								Vector3B        vd1[48] = {
                                    {-4, 7, -6}, {-4, 7, 6}, {-4, 13, -6}, {-4, 13, 6}, {4, 7, -6}, {4, 7, 6}, {4, 13, -6}, {4, 13, 6},
                                    {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
                                    {-3, 0, 2}, {-3, 0, 5}, {-3, 7, 2}, {-3, 7, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 7, -6}, {-3, 7, -3},
                                    {-1, 0, 2}, {-1, 0, 5}, {-1, 7, 2}, {-1, 7, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 7, -6}, {-1, 7, -3},
                                    {1, 0, 2}, {1, 0, 5}, {1, 7, 2}, {1, 7, 5}, {1, 0, -6}, {1, 0, -3}, {1, 7, -6}, {1, 7, -3},
                                    {3, 0, 2}, {3, 0, 5}, {3, 7, 2}, {3, 7, 5}, {3, 0, -6}, {3, 0, -3}, {3, 7, -6}, {3, 7, -3},
                                };

								for (int j = 0; j < 48; j++)
       							{
       							    allObj[objectIndex].verticesIPC[j].x = vd1[j].x + allObj[objectIndex].position.x * 10;
       							    allObj[objectIndex].verticesIPC[j].y = vd1[j].y + allObj[objectIndex].position.y * 10;
       							    allObj[objectIndex].verticesIPC[j].z = vd1[j].z + allObj[objectIndex].position.z * 10;
       							}

								Vector3I center = {allObj[objectIndex].position.x*10, allObj[objectIndex].position.y*10+6, allObj[objectIndex].position.z*10};
								addRotateObjectNS(objectIndex, objectIndex, center, 0, 90*(randomNumber-1));

								Vector3B        vd2[48] = {
                                    {-3, 8, -5}, {-3, 8, 5}, {-3, 12, -5}, {-3, 12, 5}, {3, 8, -5}, {3, 8, 5}, {3, 12, -5}, {3, 12, 5},
                                    {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
                                    {-3, 0, 2}, {-3, 0, 5}, {-3, 8, 2}, {-3, 8, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 8, -6}, {-3, 8, -3},
                                    {-1, 0, 2}, {-1, 0, 5}, {-1, 8, 2}, {-1, 8, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 8, -6}, {-1, 8, -3},
                                    {1, 0, 2}, {1, 0, 5}, {1, 8, 2}, {1, 8, 5}, {1, 0, -6}, {1, 0, -3}, {1, 8, -6}, {1, 8, -3},
                                    {3, 0, 2}, {3, 0, 5}, {3, 8, 2}, {3, 8, 5}, {3, 0, -6}, {3, 0, -3}, {3, 8, -6}, {3, 8, -3},
                                };

								for (int j = 0; j < 48; j++)
        						{
        						    allObj[objectIndex2].verticesIPC[j].x = vd2[j].x + allObj[objectIndex2].position.x * 10;
        						    allObj[objectIndex2].verticesIPC[j].y = vd2[j].y + allObj[objectIndex2].position.y * 10;
        						    allObj[objectIndex2].verticesIPC[j].z = vd2[j].z + allObj[objectIndex2].position.z * 10;
        						}

								addRotateObjectNS(objectIndex2, objectIndex2, center, 0, 90*(randomNumber-1));

								entityList[i].state2 = randomNumber;
							}
							else
							{
								char randomNumber2 = generate_random(RTC_GetTicks()+i, 4);
								if(randomNumber2 == 0)
								{
									entityList[i].state = 2;
									entityList[i].moveTimer = 2;
								}
								else
								{
									entityList[i].state = 0;
									entityList[i].moveTimer = generate_random(RTC_GetTicks()+i+1, 4)+2;
								}
							}
						}
					}

					int playerDistance = calculateDistance3D((int)allObj[objectIndex].position.x, (int)allObj[objectIndex].position.y, (int)allObj[objectIndex].position.z, (int)PPosX, (int)PPosY, (int)PPosZ);
					if(playerDistance < 8) //player is in range
					{
						if(blockSelected == 66)
                        entityList[i].state = 3;
					}

                    if(entityList[i].state == 3)
					{
                        if(playerDistance > 8 || blockSelected != 66)
                        {
                            entityList[i].state = 0;
                            entityList[i].moveTimer = -1;
                            continue;
                        }

                        if(entityList[i].isFed == true) //look for other fed sheeps around to breed with
                        {
                            for (int j = 0; j < entityLength; j++)
                            {
                                if(entityList[j].active == true)
                                {
	                        		if(entityList[j].type == 1 && entityList[j].isFed == true && i != j)
	                        		{
                                        float difX = allObj[entityList[j].locationOnAllObj].position.x-allObj[objectIndex].position.x;
                                        float difZ = allObj[entityList[j].locationOnAllObj].position.z-allObj[objectIndex].position.z;

                                        float difTotal = mysqrt(difX * difX + difZ * difZ);  

                                        if(difTotal < 3)
                                        {
                                            makeSheep2((int)allObj[objectIndex].position.x, (int)allObj[objectIndex].position.y, (int)allObj[objectIndex].position.z);
                                            entityList[j].isFed = false;
                                            entityList[i].isFed = false;
                                        }
                                    }
                                }
                            }
                        }

                        float difX = PPosX-allObj[objectIndex].position.x;
                        float difZ = PPosZ-allObj[objectIndex].position.z;

                        float difTotal = mysqrt(difX * difX + difZ * difZ);  
					    Vector2 rotationForw = {difX / difTotal, difZ / difTotal}; 
                        
                        float angle = arctan_approx(rotationForw.y, rotationForw.x);

                        Vector3B        vd1[48] = {
                            {-4, 7, -6}, {-4, 7, 6}, {-4, 13, -6}, {-4, 13, 6}, {4, 7, -6}, {4, 7, 6}, {4, 13, -6}, {4, 13, 6},
                            {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
                            {-3, 0, 2}, {-3, 0, 5}, {-3, 7, 2}, {-3, 7, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 7, -6}, {-3, 7, -3},
                            {-1, 0, 2}, {-1, 0, 5}, {-1, 7, 2}, {-1, 7, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 7, -6}, {-1, 7, -3},
                            {1, 0, 2}, {1, 0, 5}, {1, 7, 2}, {1, 7, 5}, {1, 0, -6}, {1, 0, -3}, {1, 7, -6}, {1, 7, -3},
                            {3, 0, 2}, {3, 0, 5}, {3, 7, 2}, {3, 7, 5}, {3, 0, -6}, {3, 0, -3}, {3, 7, -6}, {3, 7, -3},
                        };

                        for (int j = 0; j < 48; j++)
                        {
                               allObj[objectIndex].verticesIPC[j].x = vd1[j].x + allObj[objectIndex].position.x * 10;
                               allObj[objectIndex].verticesIPC[j].y = vd1[j].y + allObj[objectIndex].position.y * 10;
                               allObj[objectIndex].verticesIPC[j].z = vd1[j].z + allObj[objectIndex].position.z * 10;
                        }

                        Vector3I center = {allObj[objectIndex].position.x*10, allObj[objectIndex].position.y*10+6, allObj[objectIndex].position.z*10};
                        addRotateObjectNS(objectIndex, objectIndex, center, 0, angle+270);

                        Vector3B        vd2[48] = {
                            {-3, 8, -5}, {-3, 8, 5}, {-3, 12, -5}, {-3, 12, 5}, {3, 8, -5}, {3, 8, 5}, {3, 12, -5}, {3, 12, 5},
                            {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
                            {-3, 0, 2}, {-3, 0, 5}, {-3, 8, 2}, {-3, 8, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 8, -6}, {-3, 8, -3},
                            {-1, 0, 2}, {-1, 0, 5}, {-1, 8, 2}, {-1, 8, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 8, -6}, {-1, 8, -3},
                            {1, 0, 2}, {1, 0, 5}, {1, 8, 2}, {1, 8, 5}, {1, 0, -6}, {1, 0, -3}, {1, 8, -6}, {1, 8, -3},
                            {3, 0, 2}, {3, 0, 5}, {3, 8, 2}, {3, 8, 5}, {3, 0, -6}, {3, 0, -3}, {3, 8, -6}, {3, 8, -3},
                        };

                        for (int j = 0; j < 48; j++)
                        {
                            allObj[objectIndex2].verticesIPC[j].x = vd2[j].x + allObj[objectIndex2].position.x * 10;
                            allObj[objectIndex2].verticesIPC[j].y = vd2[j].y + allObj[objectIndex2].position.y * 10;
                            allObj[objectIndex2].verticesIPC[j].z = vd2[j].z + allObj[objectIndex2].position.z * 10;
                        }

                        addRotateObjectNS(objectIndex2, objectIndex2, center, 0, angle+270);

                        if(difTotal > 2)
                        {
					        bool hasBlockBelow 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y-0.10-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
					        bool hasBlockOn    		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+0.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
					        bool hasBlockAbove 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+1.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
					        bool hasBlockInFont      = checkCollision(allObj[objectIndex].position.x+rotationForw.x/2, allObj[objectIndex].position.y+0.25, allObj[objectIndex].position.z+rotationForw.y/2);
					        bool hasBlockInFontAbove = checkCollision(allObj[objectIndex].position.x+rotationForw.x/2, allObj[objectIndex].position.y+1.25, allObj[objectIndex].position.z+rotationForw.y/2);

					        bool isInWater = checkIfInWater(allObj[objectIndex].position.x, allObj[objectIndex].position.y, allObj[objectIndex].position.z);

					        if(hasBlockOn == true)
					        {
					        	moveObject(objectIndex, 0, 1, 0);
					        	moveObject(objectIndex2, 0, 1, 0);
					        }

					        if(hasBlockInFont == false)
					        {
					        	moveObject(objectIndex, rotationForw.x*deltaTimeNoSlow, 0, rotationForw.y*deltaTimeNoSlow);
					        	moveObject(objectIndex2, rotationForw.x*deltaTimeNoSlow, 0, rotationForw.y*deltaTimeNoSlow);
					        }
					        else
					        {
					        	if(hasBlockInFontAbove == false)
					        	{
					        		moveObject(objectIndex, rotationForw.x*deltaTimeNoSlow, 0, rotationForw.y*deltaTimeNoSlow);
					        		moveObject(objectIndex2, rotationForw.x*deltaTimeNoSlow, 0, rotationForw.y*deltaTimeNoSlow);
					        	}
					        	else
					        	entityList[i].state = 0;
					        }

					        if(hasBlockBelow == false)
					        {
					        	if(isInWater == true)
					        	{
					        		entityList[i].fallingSince = allObj[objectIndex].position.y;

            		        		entityList[i].velocityY -= 2 * deltaTimeNoSlow;

            		        		if(entityList[i].velocityY < -2)
            		        		entityList[i].velocityY = -2;

					        		moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
					        		moveObject(objectIndex2, 0, entityList[i].velocityY*deltaTime, 0);
					        	}
					        	else
					        	{
					        		entityList[i].fallingSince = allObj[objectIndex].position.y;

            		        		entityList[i].velocityY -= 9.81f * deltaTimeNoSlow;

					        		moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
					        		moveObject(objectIndex2, 0, entityList[i].velocityY*deltaTime, 0);
					        	}
					        }
					        else
					        {
					        	entityList[i].velocityY = 0;
					        	if(entityList[i].fallingSince > allObj[objectIndex].position.y)
					        	{
					        		int fallDamage = (startFallY - PPosY) - 3;
            		        		if(fallDamage > 0)
            		        		entityList[i].health -= fallDamage;
					        	}
					        	entityList[i].fallingSince = allObj[objectIndex].position.y;
					        }
                        }
					}

					if(entityList[i].state != 3) //is following player
					entityList[i].moveTimer -= deltaTime;

					if(entityList[i].state == 4)
					{
						if(entityList[i].moveTimer < 0)
						{
							char randomNumber = generate_random(RTC_GetTicks()+i, 24);

							if(randomNumber == 0)
							entityList[i].state = 0;
							else
							{
								randomNumber = generate_random(RTC_GetTicks()+i-32, 4);

								entityList[i].moveTimer = (float)(generate_random(RTC_GetTicks()+i+2, 10))/10+0.5f;

								Vector3B        vd1[48] = {
                                    {-4, 7, -6}, {-4, 7, 6}, {-4, 13, -6}, {-4, 13, 6}, {4, 7, -6}, {4, 7, 6}, {4, 13, -6}, {4, 13, 6},
                                    {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
                                    {-3, 0, 2}, {-3, 0, 5}, {-3, 7, 2}, {-3, 7, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 7, -6}, {-3, 7, -3},
                                    {-1, 0, 2}, {-1, 0, 5}, {-1, 7, 2}, {-1, 7, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 7, -6}, {-1, 7, -3},
                                    {1, 0, 2}, {1, 0, 5}, {1, 7, 2}, {1, 7, 5}, {1, 0, -6}, {1, 0, -3}, {1, 7, -6}, {1, 7, -3},
                                    {3, 0, 2}, {3, 0, 5}, {3, 7, 2}, {3, 7, 5}, {3, 0, -6}, {3, 0, -3}, {3, 7, -6}, {3, 7, -3},
                                };

								for (int j = 0; j < 48; j++)
       							{
       							    allObj[objectIndex].verticesIPC[j].x = vd1[j].x + allObj[objectIndex].position.x * 10;
       							    allObj[objectIndex].verticesIPC[j].y = vd1[j].y + allObj[objectIndex].position.y * 10;
       							    allObj[objectIndex].verticesIPC[j].z = vd1[j].z + allObj[objectIndex].position.z * 10;
       							}

								Vector3I center = {allObj[objectIndex].position.x*10, allObj[objectIndex].position.y*10+6, allObj[objectIndex].position.z*10};
								addRotateObjectNS(objectIndex, objectIndex, center, 0, 90*(randomNumber-1));

								Vector3B        vd2[48] = {
                                    {-3, 8, -5}, {-3, 8, 5}, {-3, 12, -5}, {-3, 12, 5}, {3, 8, -5}, {3, 8, 5}, {3, 12, -5}, {3, 12, 5},
                                    {-2, 10, 8}, {-2, 10, 4}, {-2, 14, 4}, {-2, 14, 8}, {2, 10, 8}, {2, 10, 4}, {2, 14, 4}, {2, 14, 8},
                                    {-3, 0, 2}, {-3, 0, 5}, {-3, 8, 2}, {-3, 8, 5}, {-3, 0, -6}, {-3, 0, -3}, {-3, 8, -6}, {-3, 8, -3},
                                    {-1, 0, 2}, {-1, 0, 5}, {-1, 8, 2}, {-1, 8, 5}, {-1, 0, -6}, {-1, 0, -3}, {-1, 8, -6}, {-1, 8, -3},
                                    {1, 0, 2}, {1, 0, 5}, {1, 8, 2}, {1, 8, 5}, {1, 0, -6}, {1, 0, -3}, {1, 8, -6}, {1, 8, -3},
                                    {3, 0, 2}, {3, 0, 5}, {3, 8, 2}, {3, 8, 5}, {3, 0, -6}, {3, 0, -3}, {3, 8, -6}, {3, 8, -3},
                                };

								for (int j = 0; j < 48; j++)
        						{
        						    allObj[objectIndex2].verticesIPC[j].x = vd2[j].x + allObj[objectIndex2].position.x * 10;
        						    allObj[objectIndex2].verticesIPC[j].y = vd2[j].y + allObj[objectIndex2].position.y * 10;
        						    allObj[objectIndex2].verticesIPC[j].z = vd2[j].z + allObj[objectIndex2].position.z * 10;
        						}

								addRotateObjectNS(objectIndex2, objectIndex2, center, 0, 90*(randomNumber-1));

								entityList[i].state2 = randomNumber;
							}
						}

						Vector2I rotationForw[4] = {{2, 0}, {0, 2}, {-2, 0}, {0, -2}};

						bool hasBlockBelow 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y-0.1-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockOn    		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+0.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockAbove 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+1.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockInFont      = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+0.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);
						bool hasBlockInFontAbove = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+1.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);

						bool isInWater = checkIfInWater(allObj[objectIndex].position.x, allObj[objectIndex].position.y, allObj[objectIndex].position.z);

                        Vector2I newPos = {
                            .x = allObj[objectIndex].position.x + rotationForw[entityList[i].state2].x,
                            .y = allObj[objectIndex].position.z + rotationForw[entityList[i].state2].y
                        };
                        if(newPos.x < 0 || newPos.x > width*totalChunkWidth || newPos.y < 0 || newPos.y > width*totalChunkWidth)
                        {
                            entityList[i].state = 0;
                            entityList[i].moveTimer = -1;
                            continue;
                        }

						if(hasBlockOn == true)
						{
							moveObject(objectIndex, 0, 1, 0);
							moveObject(objectIndex2, 0, 1, 0);
						}

						if(hasBlockInFont == false)
						{
							moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
							moveObject(objectIndex2, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
						}
						else
						{
							if(hasBlockInFontAbove == false)
							{
								moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
								moveObject(objectIndex2, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
							}
							else
							entityList[i].state = 0;
						}

						if(hasBlockBelow == false)
						{
							if(isInWater == true)
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 2 * deltaTimeNoSlow;

            		   			if(entityList[i].velocityY < -2)
            		   			entityList[i].velocityY = -2;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
								moveObject(objectIndex2, 0, entityList[i].velocityY*deltaTime, 0);
							}
							else
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 9.81 * deltaTimeNoSlow;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
								moveObject(objectIndex2, 0, entityList[i].velocityY*deltaTime, 0);
							}
						}
						else
						{
							entityList[i].velocityY = 0;
							if(entityList[i].fallingSince > allObj[objectIndex].position.y)
							{
								int fallDamage = (startFallY - PPosY) - 3;
            		    		if(fallDamage > 0)
            		    		entityList[i].health -= fallDamage;
							}
							entityList[i].fallingSince = allObj[objectIndex].position.y;
						}
					}

					if(entityList[i].state == 2)
					{
						if(entityList[i].moveTimer < 0)
						{
							if(blocks[chunkX+chunkZ*totalChunkWidth][posX+posZ*width+(posY-1)*width*width] == 1)
							{
								entityList[i].hasFur = true;
								allObj[objectIndex2].active = true;
								blocks[chunkX+chunkZ*totalChunkWidth][posX+posZ*width+(posY-1)*width*width] = 2;
								updateChunkTexturesV2(chunkX, chunkZ);
							}
						}
					}
					
                    if(entityList[i].state == 1)
					{
						Vector2I rotationForw[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

						bool hasBlockBelow 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y-0.1-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockOn    		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+0.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockAbove 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+1.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockInFont      = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+0.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);
						bool hasBlockInFontAbove = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+1.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);

						bool isInWater = checkIfInWater(allObj[objectIndex].position.x, allObj[objectIndex].position.y, allObj[objectIndex].position.z);


                        Vector2I newPos = {
                            .x = allObj[objectIndex].position.x + rotationForw[entityList[i].state2].x,
                            .y = allObj[objectIndex].position.z + rotationForw[entityList[i].state2].y
                        };
                        if(newPos.x < 0 || newPos.x > width*totalChunkWidth || newPos.y < 0 || newPos.y > width*totalChunkWidth)
                        {
                            entityList[i].state = 0;
                            entityList[i].moveTimer = -1;
                            continue;
                        }

						if(hasBlockOn == true)
						{
							moveObject(objectIndex, 0, 1, 0);
							moveObject(objectIndex2, 0, 1, 0);
						}

						if(hasBlockInFont == false)
						{
							moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
							moveObject(objectIndex2, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
						}
						else
						{
							if(hasBlockInFontAbove == false)
							{
								moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
								moveObject(objectIndex2, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
							}
							else
							entityList[i].state = 0;
						}

						if(hasBlockBelow == false)
						{
							if(isInWater == true)
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 2 * deltaTimeNoSlow;

            		   			if(entityList[i].velocityY < -2)
            		   			entityList[i].velocityY = -2;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
								moveObject(objectIndex2, 0, entityList[i].velocityY*deltaTime, 0);
							}
							else
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 9.81f * deltaTimeNoSlow;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
								moveObject(objectIndex2, 0, entityList[i].velocityY*deltaTime, 0);
							}
						}
						else
						{
							entityList[i].velocityY = 0;
							if(entityList[i].fallingSince > allObj[objectIndex].position.y)
							{
								int fallDamage = (startFallY - PPosY) - 3;
            		    		if(fallDamage > 0)
            		    		entityList[i].health -= fallDamage;
							}
							entityList[i].fallingSince = allObj[objectIndex].position.y;
						}
					}

					if(entityList[i].health < 1)
					{
                        if(entityList[i].hasFur == true)
                        makeItem((int)(allObj[objectIndex].position.x*10), (int)(allObj[objectIndex].position.y*10), (int)(allObj[objectIndex].position.z*10), 0, 0, 0, 95);

						entityList[i].active = false;
						entityList[entityList[i].extraEntity].active = false;
						allObj[objectIndex].active = false;
						allObj[objectIndex2].active = false;

                        tlsf_free(tlsf, allObj[objectIndex].verticesIPC);
                        tlsf_free(tlsf, allObj[objectIndex].triangles);
                        tlsf_free(tlsf, allObj[objectIndex].normal);
                        tlsf_free(tlsf, allObj[objectIndex].textureSize);
                        tlsf_free(tlsf, allObj[objectIndex].color);
                        tlsf_free(tlsf, allObj[objectIndex].solidColor);

                        tlsf_free(tlsf, allObj[objectIndex2].verticesIPC);
                        tlsf_free(tlsf, allObj[objectIndex2].triangles);
                        tlsf_free(tlsf, allObj[objectIndex2].normal);
                        tlsf_free(tlsf, allObj[objectIndex2].textureSize);
                        tlsf_free(tlsf, allObj[objectIndex2].color);
                        tlsf_free(tlsf, allObj[objectIndex2].solidColor);


						triangleLength -= 60;
						verticesLength -= 96;

						usedEntitys -= 2;

                        continue;
					}
				}
				else
				{
					allObj[objectIndex].active = false;
					allObj[objectIndex2].active = false;
				}
			}
            if(entityList[i].type == 2) //pig ai
			{
				//state 0=standing still, 1=walking, 3=following player, 4=running, 5=just hit
				int objectIndex = entityList[i].locationOnAllObj;

				int chunkX = allObj[objectIndex].position.x/width;
				int chunkZ = allObj[objectIndex].position.z/width;

				int posX = (int)(allObj[objectIndex].position.x)%width;
				int posY = (int)(allObj[objectIndex].position.y);
				int posZ = (int)(allObj[objectIndex].position.z)%width;

				if(chunk2DActive[chunkX + chunkZ*totalChunkWidth] == true)
				{
					allObj[objectIndex].active = true;

					if(entityList[i].state == 5)
					{
						//correct the colors
						if(survival == true)
						entityList[i].state = 4;

						color_t     color[28] = {
                            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
                            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
                            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2,
                            0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 0xd4b2, 
                        };

						for (int j = 0; j < 28; j++)
						allObj[objectIndex].solidColor[j] = color[j];

                        entityList[i].renderTextured = true;
					}

					if(entityList[i].state != 4)
					{
						if(entityList[i].moveTimer < 0)
						{
							char randomNumber = generate_random(RTC_GetTicks()+i, 6);

							if(randomNumber < 4)
							{
								entityList[i].state = 1;
								entityList[i].moveTimer = generate_random(RTC_GetTicks()+i+1, 4)+2;

								Vector3B        vd[48] = {
                                    {-3, 6, 10}, {3, 6, 10}, {-3, 6, 4}, {3, 6, 4}, {-3, 11, 10}, {3, 11, 10}, {-3, 11, 4}, {3, 11, 4},
                                    {-4, 4, 5}, {4, 4, 5}, {-4, 4, -6}, {4, 4, -6}, {-4, 10, 5}, {4, 10, 5}, {-4, 10, -6}, {4, 10, -6},
                                    {-4, 0, 2}, {-1, 0, 2}, {-4, 4, 2}, {-1, 4, 2}, {-4, 0, 5}, {-1, 0, 5}, {-4, 4, 5}, {-1, 4, 5},
                                    {1, 0, 2}, {4, 0, 2}, {1, 4, 2}, {4, 4, 2}, {1, 0, 5}, {4, 0, 5}, {1, 4, 5}, {4, 4, 5},
                                    {-4, 0, -7}, {-1, 0, -7}, {-4, 4, -7}, {-1, 4, -7}, {-4, 0, -4}, {-1, 0, -4}, {-4, 4, -4}, {-1, 4, -4},
                                    {1, 0, -7}, {4, 0, -7}, {1, 4, -7}, {4, 4, -7}, {1, 0, -4}, {4, 0, -4}, {1, 4, -4}, {4, 4, -4},
                                };

								for (int j = 0; j < 48; j++)
       							{
       							    allObj[objectIndex].verticesIPC[j].x = vd[j].x + allObj[objectIndex].position.x * 10;
       							    allObj[objectIndex].verticesIPC[j].y = vd[j].y + allObj[objectIndex].position.y * 10;
       							    allObj[objectIndex].verticesIPC[j].z = vd[j].z + allObj[objectIndex].position.z * 10;
       							}

								Vector3I center = {allObj[objectIndex].position.x*10, allObj[objectIndex].position.y*10+6, allObj[objectIndex].position.z*10};
								addRotateObjectNS(objectIndex, objectIndex, center, 0, 90*(randomNumber-1));

								entityList[i].state2 = randomNumber;
							}
							else
							{
								entityList[i].state = 0;
								entityList[i].moveTimer = generate_random(RTC_GetTicks()+i+1, 4)+2;
							}
						}
					}

					int playerDistance = calculateDistance3D((int)allObj[objectIndex].position.x, (int)allObj[objectIndex].position.y, (int)allObj[objectIndex].position.z, (int)PPosX, (int)PPosY, (int)PPosZ);
					if(playerDistance < 8) //player is in range
					{
						if(blockSelected == 66)
                        entityList[i].state = 3;
					}

                    if(entityList[i].state == 3)
					{
                        if(playerDistance > 8 || blockSelected != 66)
                        {
                            entityList[i].state = 0;
                            entityList[i].moveTimer = -1;
                            continue;
                        }

                        if(entityList[i].isFed == true) //look for other fed sheeps around to breed with
                        {
                            for (int j = 0; j < entityLength; j++)
                            {
                                if(entityList[j].active == true)
                                {
	                        		if(entityList[j].type == 2 && entityList[j].isFed == true && i != j)
	                        		{
                                        float difX = allObj[entityList[j].locationOnAllObj].position.x-allObj[objectIndex].position.x;
                                        float difZ = allObj[entityList[j].locationOnAllObj].position.z-allObj[objectIndex].position.z;

                                        float difTotal = mysqrt(difX * difX + difZ * difZ);  

                                        if(difTotal < 3)
                                        {
                                            makePig((int)allObj[objectIndex].position.x, (int)allObj[objectIndex].position.y, (int)allObj[objectIndex].position.z);
                                            entityList[j].isFed = false;
                                            entityList[i].isFed = false;
                                        }
                                    }
                                }
                            }
                        }

                        float difX = PPosX-allObj[objectIndex].position.x;
                        float difZ = PPosZ-allObj[objectIndex].position.z;

                        float difTotal = mysqrt(difX * difX + difZ * difZ);  
					    Vector2 rotationForw = {difX / difTotal, difZ / difTotal}; 
                        
                        float angle = arctan_approx(rotationForw.y, rotationForw.x);

                        Vector3B        vd[48] = {
                            {-3, 6, 10}, {3, 6, 10}, {-3, 6, 4}, {3, 6, 4}, {-3, 11, 10}, {3, 11, 10}, {-3, 11, 4}, {3, 11, 4},
                            {-4, 4, 5}, {4, 4, 5}, {-4, 4, -6}, {4, 4, -6}, {-4, 10, 5}, {4, 10, 5}, {-4, 10, -6}, {4, 10, -6},
                            {-4, 0, 2}, {-1, 0, 2}, {-4, 4, 2}, {-1, 4, 2}, {-4, 0, 5}, {-1, 0, 5}, {-4, 4, 5}, {-1, 4, 5},
                            {1, 0, 2}, {4, 0, 2}, {1, 4, 2}, {4, 4, 2}, {1, 0, 5}, {4, 0, 5}, {1, 4, 5}, {4, 4, 5},
                            {-4, 0, -7}, {-1, 0, -7}, {-4, 4, -7}, {-1, 4, -7}, {-4, 0, -4}, {-1, 0, -4}, {-4, 4, -4}, {-1, 4, -4},
                            {1, 0, -7}, {4, 0, -7}, {1, 4, -7}, {4, 4, -7}, {1, 0, -4}, {4, 0, -4}, {1, 4, -4}, {4, 4, -4},
                        };

                        for (int j = 0; j < 48; j++)
                        {
                            allObj[objectIndex].verticesIPC[j].x = vd[j].x + allObj[objectIndex].position.x * 10;
                            allObj[objectIndex].verticesIPC[j].y = vd[j].y + allObj[objectIndex].position.y * 10;
                            allObj[objectIndex].verticesIPC[j].z = vd[j].z + allObj[objectIndex].position.z * 10;
                        }

                        Vector3I center = {allObj[objectIndex].position.x*10, allObj[objectIndex].position.y*10+6, allObj[objectIndex].position.z*10};
                        addRotateObjectNS(objectIndex, objectIndex, center, 0, angle+270);

                        if(difTotal > 2)
                        {
					        bool hasBlockBelow 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y-0.10-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
					        bool hasBlockOn    		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+0.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
					        bool hasBlockAbove 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+1.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
					        bool hasBlockInFont      = checkCollision(allObj[objectIndex].position.x+rotationForw.x/2, allObj[objectIndex].position.y+0.25, allObj[objectIndex].position.z+rotationForw.y/2);
					        bool hasBlockInFontAbove = checkCollision(allObj[objectIndex].position.x+rotationForw.x/2, allObj[objectIndex].position.y+1.25, allObj[objectIndex].position.z+rotationForw.y/2);

					        bool isInWater = checkIfInWater(allObj[objectIndex].position.x, allObj[objectIndex].position.y, allObj[objectIndex].position.z);

					        if(hasBlockOn == true)
					        moveObject(objectIndex, 0, 1, 0);

					        if(hasBlockInFont == false)
					        moveObject(objectIndex, rotationForw.x*deltaTimeNoSlow, 0, rotationForw.y*deltaTimeNoSlow);
					        else
					        {
					        	if(hasBlockInFontAbove == false)
					        	moveObject(objectIndex, rotationForw.x*deltaTimeNoSlow, 0, rotationForw.y*deltaTimeNoSlow);
					        	else
					        	entityList[i].state = 0;
					        }

					        if(hasBlockBelow == false)
					        {
					        	if(isInWater == true)
					        	{
					        		entityList[i].fallingSince = allObj[objectIndex].position.y;

            		        		entityList[i].velocityY -= 2 * deltaTimeNoSlow;

            		        		if(entityList[i].velocityY < -2)
            		        		entityList[i].velocityY = -2;

					        		moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
					        	}
					        	else
					        	{
					        		entityList[i].fallingSince = allObj[objectIndex].position.y;

            		        		entityList[i].velocityY -= 9.81f * deltaTimeNoSlow;

					        		moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
					        	}
					        }
					        else
					        {
					        	entityList[i].velocityY = 0;
					        	if(entityList[i].fallingSince > allObj[objectIndex].position.y)
					        	{
					        		int fallDamage = (startFallY - PPosY) - 3;
            		        		if(fallDamage > 0)
            		        		entityList[i].health -= fallDamage;
					        	}
					        	entityList[i].fallingSince = allObj[objectIndex].position.y;
					        }
                        }
					}

					if(entityList[i].state != 3) //is not following player
					entityList[i].moveTimer -= deltaTime;

					if(entityList[i].state == 4)
					{
						if(entityList[i].moveTimer < 0)
						{
							char randomNumber = generate_random(RTC_GetTicks()+i, 24);

							if(randomNumber == 0)
							entityList[i].state = 0;
							else
							{
								randomNumber = generate_random(RTC_GetTicks()+i-32, 4);

								entityList[i].moveTimer = (float)(generate_random(RTC_GetTicks()+i+2, 10))/10+0.5f;

								Vector3B        vd[48] = {
                                    {-3, 6, 10}, {3, 6, 10}, {-3, 6, 4}, {3, 6, 4}, {-3, 11, 10}, {3, 11, 10}, {-3, 11, 4}, {3, 11, 4},
                                    {-4, 4, 5}, {4, 4, 5}, {-4, 4, -6}, {4, 4, -6}, {-4, 10, 5}, {4, 10, 5}, {-4, 10, -6}, {4, 10, -6},
                                    {-4, 0, 2}, {-1, 0, 2}, {-4, 4, 2}, {-1, 4, 2}, {-4, 0, 5}, {-1, 0, 5}, {-4, 4, 5}, {-1, 4, 5},
                                    {1, 0, 2}, {4, 0, 2}, {1, 4, 2}, {4, 4, 2}, {1, 0, 5}, {4, 0, 5}, {1, 4, 5}, {4, 4, 5},
                                    {-4, 0, -7}, {-1, 0, -7}, {-4, 4, -7}, {-1, 4, -7}, {-4, 0, -4}, {-1, 0, -4}, {-4, 4, -4}, {-1, 4, -4},
                                    {1, 0, -7}, {4, 0, -7}, {1, 4, -7}, {4, 4, -7}, {1, 0, -4}, {4, 0, -4}, {1, 4, -4}, {4, 4, -4},
                                };

								for (int j = 0; j < 48; j++)
       							{
       							    allObj[objectIndex].verticesIPC[j].x = vd[j].x + allObj[objectIndex].position.x * 10;
       							    allObj[objectIndex].verticesIPC[j].y = vd[j].y + allObj[objectIndex].position.y * 10;
       							    allObj[objectIndex].verticesIPC[j].z = vd[j].z + allObj[objectIndex].position.z * 10;
       							}

								Vector3I center = {allObj[objectIndex].position.x*10, allObj[objectIndex].position.y*10+6, allObj[objectIndex].position.z*10};
								addRotateObjectNS(objectIndex, objectIndex, center, 0, 90*(randomNumber-1));

								entityList[i].state2 = randomNumber;
							}
						}

						Vector2I rotationForw[4] = {{2, 0}, {0, 2}, {-2, 0}, {0, -2}};

						bool hasBlockBelow 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y-0.1-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockOn    		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+0.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockAbove 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+1.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockInFont      = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+0.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);
						bool hasBlockInFontAbove = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+1.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);

						bool isInWater = checkIfInWater(allObj[objectIndex].position.x, allObj[objectIndex].position.y, allObj[objectIndex].position.z);

                        Vector2I newPos = {
                            .x = allObj[objectIndex].position.x + rotationForw[entityList[i].state2].x,
                            .y = allObj[objectIndex].position.z + rotationForw[entityList[i].state2].y
                        };
                        if(newPos.x < 0 || newPos.x > width*totalChunkWidth || newPos.y < 0 || newPos.y > width*totalChunkWidth)
                        {
                            entityList[i].state = 0;
                            entityList[i].moveTimer = -1;
                            continue;
                        }

						if(hasBlockOn == true)
						moveObject(objectIndex, 0, 1, 0);

						if(hasBlockInFont == false)
						moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
						else
						{
							if(hasBlockInFontAbove == false)
							moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
							else
							entityList[i].state = 0;
						}

						if(hasBlockBelow == false)
						{
							if(isInWater == true)
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 2 * deltaTimeNoSlow;

            		   			if(entityList[i].velocityY < -2)
            		   			entityList[i].velocityY = -2;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
							}
							else
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 9.81 * deltaTimeNoSlow;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
							}
						}
						else
						{
							entityList[i].velocityY = 0;
							if(entityList[i].fallingSince > allObj[objectIndex].position.y)
							{
								int fallDamage = (startFallY - PPosY) - 3;
            		    		if(fallDamage > 0)
            		    		entityList[i].health -= fallDamage;
							}
							entityList[i].fallingSince = allObj[objectIndex].position.y;
						}
					}
					
                    if(entityList[i].state == 1)
					{
						Vector2I rotationForw[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

						bool hasBlockBelow 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y-0.1-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockOn    		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+0.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockAbove 		 = checkCollision(allObj[objectIndex].position.x, allObj[objectIndex].position.y+1.25-entityList[i].velocityY*deltaTime, allObj[objectIndex].position.z);
						bool hasBlockInFont      = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+0.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);
						bool hasBlockInFontAbove = checkCollision(allObj[objectIndex].position.x+rotationForw[entityList[i].state2].x/2, allObj[objectIndex].position.y+1.25, allObj[objectIndex].position.z+rotationForw[entityList[i].state2].y/2);

						bool isInWater = checkIfInWater(allObj[objectIndex].position.x, allObj[objectIndex].position.y, allObj[objectIndex].position.z);


                        Vector2I newPos = {
                            .x = allObj[objectIndex].position.x + rotationForw[entityList[i].state2].x,
                            .y = allObj[objectIndex].position.z + rotationForw[entityList[i].state2].y
                        };
                        if(newPos.x < 0 || newPos.x > width*totalChunkWidth || newPos.y < 0 || newPos.y > width*totalChunkWidth)
                        {
                            entityList[i].state = 0;
                            entityList[i].moveTimer = -1;
                            continue;
                        }

						if(hasBlockOn == true)
						moveObject(objectIndex, 0, 1, 0);

						if(hasBlockInFont == false)
						moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
						else
						{
							if(hasBlockInFontAbove == false)
							moveObject(objectIndex, rotationForw[entityList[i].state2].x*deltaTimeNoSlow, 0, rotationForw[entityList[i].state2].y*deltaTimeNoSlow);
							else
							entityList[i].state = 0;
						}

						if(hasBlockBelow == false)
						{
							if(isInWater == true)
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 2 * deltaTimeNoSlow;

            		   			if(entityList[i].velocityY < -2)
            		   			entityList[i].velocityY = -2;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
							}
							else
							{
								entityList[i].fallingSince = allObj[objectIndex].position.y;

            		   			entityList[i].velocityY -= 9.81f * deltaTimeNoSlow;

								moveObject(objectIndex, 0, entityList[i].velocityY*deltaTime, 0);
							}
						}
						else
						{
							entityList[i].velocityY = 0;
							if(entityList[i].fallingSince > allObj[objectIndex].position.y)
							{
								int fallDamage = (startFallY - PPosY) - 3;
            		    		if(fallDamage > 0)
            		    		entityList[i].health -= fallDamage;
							}
							entityList[i].fallingSince = allObj[objectIndex].position.y;
						}
					}

					if(entityList[i].health < 1)
					{
                        makeItem((int)(allObj[objectIndex].position.x*10), (int)(allObj[objectIndex].position.y*10), (int)(allObj[objectIndex].position.z*10), 0, 0, 0, 98);
                        if(generate_random(RTC_GetTicks()+i, 4) == 0)
                        makeItem((int)(allObj[objectIndex].position.x*10+1), (int)(allObj[objectIndex].position.y*10), (int)(allObj[objectIndex].position.z*10+1), 0, 0, 0, 98);

						entityList[i].active = false;
						allObj[objectIndex].active = false;

                        tlsf_free(tlsf, allObj[objectIndex].verticesIPC);
                        tlsf_free(tlsf, allObj[objectIndex].triangles);
                        tlsf_free(tlsf, allObj[objectIndex].normal);
                        tlsf_free(tlsf, allObj[objectIndex].textureSize);
                        tlsf_free(tlsf, allObj[objectIndex].color);
                        tlsf_free(tlsf, allObj[objectIndex].solidColor);

						triangleLength -= 28;
						verticesLength -= 48;

						usedEntitys--;

                        continue;
					}
				}
				else
				allObj[objectIndex].active = false;
			}
            if(entityList[i].type == 3) //item 
            {
                Vector3I itemPosI = allObj[entityList[i].locationOnAllObj].actualPosition;
                Vector3 itemPos = allObj[entityList[i].locationOnAllObj].position;
                int pposXI = (int)PPosX;
                int pposYI = (int)PPosY - 1;
                int pposZI = (int)PPosZ;

				Vector2I aroundItem[5] = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

				for (int j = 0; j < 5; j++)
				{
					int newPposXI = pposXI+aroundItem[j].x;
					int newPposZI = pposZI+aroundItem[j].y;

					if(itemPosI.x / 10 == newPposXI && itemPosI.y / 10 == pposYI && itemPosI.z / 10 == newPposZI)
               		{
               		    addItemToHotbarInventory(entityList[i].state, 1);

                        allObj[entityList[i].locationOnAllObj].active = false;

                        verticesLength -= allObj[entityList[i].locationOnAllObj].sizeV;
                        triangleLength -= allObj[entityList[i].locationOnAllObj].sizeT;

                        allObj[entityList[i].locationOnAllObj].sizeV = 0;
                        allObj[entityList[i].locationOnAllObj].sizeT = 0;
                        
                        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].verticesIPC);
                        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].triangles  );
                        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].color      );
                        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].normal     );
                        tlsf_free(tlsf, allObj[entityList[i].locationOnAllObj].textureSize);
                        
                        entityList[i].state = 0;
                        entityList[i].type = 0;
                        entityList[i].active = false;

                        usedEntitys--;

						break;
               		}
				}
                
                entityList[i].velocityY -= 10 * deltaTimeNoSlow;
                bool isGroundedBlock = checkCollision(itemPos.x, itemPos.y + entityList[i].velocityY*deltaTime, itemPos.z);

                if(isGroundedBlock == false)
                moveObject(i+100, 0, entityList[i].velocityY*deltaTime, 0);
                else
                {
                    if(checkCollision(itemPos.x, itemPos.y, itemPos.z) == true)
                    moveObject(i+100, 0, 1, 0);

                    entityList[i].velocityY = 0;
                }
                
                if(allObj[entityList[i].locationOnAllObj].sizeT == 1)
                {
                    Vector3I voxelVertices4[4] = {
                        {0, 0, 0}, {0, 1, 0}, {1, 1, 1}, {1, 0, 1}
                    };

                    for (int j = 0; j < 4; j++)
                    {
                        allObj[entityList[i].locationOnAllObj].verticesIPC[j].x = voxelVertices4[j].x * 6 + itemPosI.x;
                        allObj[entityList[i].locationOnAllObj].verticesIPC[j].y = voxelVertices4[j].y * 6 + itemPosI.y;
                        allObj[entityList[i].locationOnAllObj].verticesIPC[j].z = voxelVertices4[j].z * 6 + itemPosI.z;
                    }
                    
                    entityList[i].rotation.y += deltaTimeNoSlow * 90;
                    Vector3I center = {3 + itemPosI.x, 3 + itemPosI.y, 3 + itemPosI.z};

                    addRotateObjectNS(entityList[i].locationOnAllObj, entityList[i].locationOnAllObj, center, 0, entityList[i].rotation.y);
                }
				else
				{
                    int currentBlockType = allBlock[allItem[entityList[i].state].blockId].blockType;

					for (int j = 0; j < allObj[entityList[i].locationOnAllObj].sizeV; j++)
                    {
                        allObj[entityList[i].locationOnAllObj].verticesIPC[j].x = blockTypes[currentBlockType].vertices[0][j].x / 5 + itemPosI.x;
                        allObj[entityList[i].locationOnAllObj].verticesIPC[j].y = blockTypes[currentBlockType].vertices[0][j].y / 5 + itemPosI.y;
                        allObj[entityList[i].locationOnAllObj].verticesIPC[j].z = blockTypes[currentBlockType].vertices[0][j].z / 5 + itemPosI.z;
                    }
                    
                    entityList[i].rotation.y += deltaTimeNoSlow * 90;
                    Vector3I center = {1 + itemPosI.x, 1 + itemPosI.y, 1 + itemPosI.z};

                    addRotateObjectNS(entityList[i].locationOnAllObj, entityList[i].locationOnAllObj, center, 0, entityList[i].rotation.y);
				}
            }
        }
    }
}
void makeItem(int posX, int posY, int posZ, float velocityX, float velocityY, float velocityZ, int itemIndex)
{
    int slot = -1;
    for (int i = 0; i < entityLength; i++)
    {
        if(entityList[i].active == false)
