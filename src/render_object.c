/* src/render_object.c - Main game loop (renderObject) */
#include "engine.h"
#include "chunk_constants.h"



    float time = 0;
    float redstoneUpdateTimer1 = 0;
	int currentRedstoneChunk = 0;
    int number;

    bool clearScreen = true;
    bool isGrounded;

	float timerRecording = 0;

	bool renderBlockOutline = true;
    bool justPressedHitButton = false;

    start_program();

    while(ExitLoop == false)
    { 
		performanceTime1 = RTC_GetTicks();
        deltaTimeCalculate();
        number++;
        time += deltaTimeNoSlow;

        if(UIState < 9 || UIState > 12)
        {
			updateEntityReach();
			if(entityInFrontIndex > -1)
			{
				if(entityList[entityInFrontIndex].type != 3)
				renderBlockOutline = false;
				else
				renderBlockOutline = true;
			}
			else
			renderBlockOutline = true;

			redstoneUpdateTimer1 += deltaTime;
			int chunksToUpdate = 0;
			for (int chunkX = 0; chunkX < totalChunkWidth; chunkX++)
			{
				for (int chunkZ = 0; chunkZ < totalChunkWidth; chunkZ++)
				{
					if((abs_int(chunkX-(int)(PPosX/width))+abs_int(chunkZ-(int)(PPosZ/width))) <= (redstoneRenderDistance-1))
					chunksToUpdate++;
				}
			}
			
			if(chunksToUpdate > 0)
			{
		 		while(redstoneUpdateTimer1 > (float)((0.25/(float)chunksToUpdate)*((float)currentRedstoneChunk+1)))
		 		{
		 			if(currentRedstoneChunk < chunksToUpdate)
		 			{
		 				updateMap();
		 				currentRedstoneChunk++;
		 			}
		 			else
		 			break;
		 		}
			}
            
		 	if(redstoneUpdateTimer1 > 0.25)
		 	{
		 		currentRedstoneChunk = 0;
		 		redstoneUpdateTimer1 = 0;
		 	}
			
            performanceTime2 = RTC_GetTicks();
            performace[0] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();

            if(time > 1)
            {
                fps = number;
                number = 0;
                time = 0;
                
                if(dayNightCycle == true)
                {
                    dayTime++;
                    if(dayTime > 1200)
                    dayTime = 0;

                    if((dayTime > 1140 && dayTime < 1200) || (dayTime > 0 && dayTime < 80))
                    {
                        dayTimeChange++;
                        if(dayTimeChange == 10)
                        {
                            dayTimeChange = 0;
                            skyBrightness++;

                            if(lighting == true)
                            loadLighting2();

                            updateAllChunkLighting();

                            for (int i = 0; i < 16; i++)
                            screenColor[i] = skyColors[skyBrightness-1];
                        }
                    }

                    if(dayTime > 520 && dayTime < 660)
                    {
                        dayTimeChange++;
                        if(dayTimeChange == 10)
                        {
                            dayTimeChange = 0;
                            skyBrightness--;

                            if(lighting == true)
                            loadLighting2();

                            updateAllChunkLighting();
                            
                            for (int i = 0; i < 16; i++)
                            screenColor[i] = skyColors[skyBrightness-1];
                        }
                    }
                }
                else
                {
                    if(dayTime == 1200)
                    {
                        dayTime = 0;
                        skyBrightness = 16;

                        if(lighting == true)
                        loadLighting2();

                        updateAllChunkLighting();

                        for (int i = 0; i < 16; i++)
                        screenColor[i] = skyColors[skyBrightness-1];    
                    }
                }

                if(checkIfInLava(PPosX, PPosY-1, PPosZ) == true && survival == true)
                health -= 3;
            }
            
            if(health < 1)
            {
                for (int i = 0; i < 27; i++)
                {
                    inventoryBlockTypes[i] = -1;
                    inventoryBlockAmount[i] = 0;
					inventoryBlockHp[i] = 0;
                }
                for (int i = 0; i < 9; i++)
                {
                    hotbarBlockTypes[i] = -1;
                    hotbarBlockAmount[i] = 0;
					hotbarBlockHp[i] = 0;
                }
                health = 20;
                updateItemSlected();
                
                int seedExtra_ = 0;
                PPosY = 0;

                while(PPosY < 18)
                {
                    PPosX = generate_random(SEED+seedExtra_+1, 48) + 24.5;
                    PPosZ = generate_random(SEED+seedExtra_+2, 48) + 24.5;
                    PPosY = GetPerlin((int)PPosX, (int)PPosZ)+3;
                    seedExtra_++;
                }
            }

			float cameraRotationX = ToRadians(rotationX);
            float cameraRotationY = ToRadians(rotationY);

            forward = CalculateForwardVector(cameraRotationX, cameraRotationY);
            Vector3 right = CalculateRightVector(cameraRotationX, cameraRotationY);
            bool isInWater = checkIfInWater(PPosX+forward.x * (playerMovementSpeed * deltaTime), PPosY-1.4, PPosZ-forward.z * (playerMovementSpeed * deltaTime));

            if(survival == true)
            {
                if(isInWater == false)
                {
                    gravity = 9.81f;
                    if(isGrounded == false)
                    {
                        velocityY -= gravity * deltaTimeNoSlow;

                        if(startFallY == 0)
                        startFallY = PPosY;
                    }
                }
                else
                {
                    gravity = 2;
                    if(isGrounded == false)
                    {
                        velocityY -= gravity * deltaTimeNoSlow;

                        if(velocityY < -2)
                        velocityY = -2;
                    }
                }

                isGrounded = checkCollision(PPosX, PPosY-1.8 + velocityY*deltaTime, PPosZ);

                if(velocityY > 0)
                {
                    if(checkCollision(PPosX, PPosY+0.2 + velocityY*deltaTime, PPosZ))
                    velocityY = 0;
                }

                if(isGrounded == true)
                {
                    int fallDamage = (startFallY - PPosY) - 3;
                    if(fallDamage > 0)
                    health -= fallDamage;

                    startFallY = 0;
                    velocityY = 0;
                }

                if(PPosY + velocityY * deltaTime > 0)
                PPosY += velocityY * deltaTime;
            }
            
            
            performanceTime2 = RTC_GetTicks();
            performace[1] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();

            update();

            if(UIState == 0) {
                keyupdate();
                if(keydownlast(KEY_PRGM_8) && keydownhold(KEY_PRGM_8)){
                    if(survival == false)
                    {
                        PPosX += forward.x * (playerMovementSpeed * deltaTime);
                        PPosY -= forward.y * (playerMovementSpeed * deltaTime);
                        PPosZ -= forward.z * (playerMovementSpeed * deltaTime);
                    }else
                    {
                        if(checkCollision(PPosX+forward.x * (playerMovementSpeed * deltaTime), PPosY-1.2, PPosZ) == false)
                        PPosX += forward.x * (playerMovementSpeed * deltaTime);
                        if(checkCollision(PPosX, PPosY-1.2, PPosZ-forward.z * (playerMovementSpeed * deltaTime)) == false)
                        PPosZ -= forward.z * (playerMovementSpeed * deltaTime);
                    }
                }
                if(keydownlast(KEY_PRGM_5) && keydownhold(KEY_PRGM_5)){
                    if(survival == false)
                    {
                        PPosX -= forward.x *(playerMovementSpeed * deltaTime);
                        PPosY += forward.y *(playerMovementSpeed * deltaTime);
                        PPosZ += forward.z *(playerMovementSpeed * deltaTime);
                    }else
                    {
                        if(checkCollision(PPosX-forward.x * (playerMovementSpeed * deltaTime), PPosY-1.2, PPosZ) == false)
                        PPosX -= forward.x * (playerMovementSpeed * deltaTime);
                        if(checkCollision(PPosX, PPosY-1.2, PPosZ+forward.z * (playerMovementSpeed * deltaTime)) == false)
                        PPosZ += forward.z * (playerMovementSpeed * deltaTime);
                    }
                }
                if(keydownlast(KEY_PRGM_6) && keydownhold(KEY_PRGM_6)){
                    if(survival == false)
                    {
                        PPosX += right.x *( playerMovementSpeed * deltaTime);
                        PPosY -= right.y *( playerMovementSpeed * deltaTime);
                        PPosZ -= right.z *( playerMovementSpeed * deltaTime);
                    }else
                    {
                        if(checkCollision(PPosX+right.x * (playerMovementSpeed * deltaTime), PPosY-1.2, PPosZ) == false)
                        PPosX += right.x * (playerMovementSpeed * deltaTime);
                        if(checkCollision(PPosX, PPosY-1.2, PPosZ-right.z * (playerMovementSpeed * deltaTime)) == false)
                        PPosZ -= right.z * (playerMovementSpeed * deltaTime);
                    }
                }
                if(keydownlast(KEY_PRGM_4) && keydownhold(KEY_PRGM_4)){
                    if(survival == false)
                    {
                        PPosX -= right.x *( playerMovementSpeed * deltaTime);
                        PPosY += right.y *( playerMovementSpeed * deltaTime);
                        PPosZ += right.z *( playerMovementSpeed * deltaTime);
                    }else
                    {
                        if(checkCollision(PPosX-right.x * (playerMovementSpeed * deltaTime), PPosY-1.2, PPosZ) == false)
                        PPosX -= right.x * (playerMovementSpeed * deltaTime);
                        if(checkCollision(PPosX, PPosY-1.2, PPosZ+right.z * (playerMovementSpeed * deltaTime)) == false)
                        PPosZ += right.z * (playerMovementSpeed * deltaTime);
                    }
                }
                if(keydownlast(KEY_PRGM_UP) && keydownhold(KEY_PRGM_UP)){
                    rotationX -= (playerRotationSpeed * deltaTimeNoSlow);
                    if(rotationX < -90)
                    rotationX = -90;
                }
                if(keydownlast(KEY_PRGM_DOWN) && keydownhold(KEY_PRGM_DOWN)){
                    rotationX += (playerRotationSpeed * deltaTimeNoSlow);
                    if(rotationX > 90)
                    rotationX = 90;
                }
                if(keydownlast(KEY_PRGM_RIGHT) && keydownhold(KEY_PRGM_RIGHT))
                {
                    rotationY += (playerRotationSpeed * deltaTimeNoSlow);
                    if(rotationY > 360)
                    rotationY -= 360;
                }
                if(keydownlast(KEY_PRGM_LEFT) && keydownhold(KEY_PRGM_LEFT))
                {
                    rotationY -= (playerRotationSpeed * deltaTimeNoSlow);
                    if(rotationY < 0)
                    rotationY += 360;
                }
                if(keydownlast(KEY_PRGM_EXIT) && !keydownhold(KEY_PRGM_EXIT))
                {
                    DmaWaitNext();
                    break;
                }
                if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
                {
                    if(UIState == 0)
                    UIState = 13;
                }
                if(keydownlast(KEY_VARS) && !keydownhold(KEY_VARS))
                {
                    if(UIState == 0)
                    {
                        if(survival == true)
                        UIState = 16;
                        else
                        UIState = 17;
                    }
                }
                if(keydownlast(KEY_PRGM_1) && !keydownhold(KEY_PRGM_1))
                showPerformaceGraph = !showPerformaceGraph;
                if(keydownlast(KEY_PRGM_ALPHA) && !keydownhold(KEY_PRGM_ALPHA))
                survival = !survival;
                if(isInWater == false)
                {
                    if(keydownlast(KEY_PRGM_OPTN) && keydownhold(KEY_PRGM_OPTN)){
                        if(isGrounded == true)
                        velocityY = 5;
                    }
                }
                else
                {
                    if(keydownlast(KEY_PRGM_OPTN) && keydownhold(KEY_PRGM_OPTN))
                    velocityY = 2;
                }
                if(survival == true)
                {
                    if(keydownlast(KEY_PRGM_7) && keydownhold(KEY_PRGM_7))
                    {
                        bool hasEntityInFront = false;

                        if(entityInFrontIndex != -1)
                        {
                            if(entityList[entityInFrontIndex].type != 3)
                            {
                                if(justPressedHitButton == false)
                                {
							        int damage = 1;
							        if(blockSelected > -1)
							        damage = allItem[blockSelected].damage;

							        if(entityList[entityInFrontIndex].type == 1) //sheep
							        {
                                        entityList[entityInFrontIndex].health -= damage;

							        	int objectIndex = entityList[entityInFrontIndex].locationOnAllObj;
							        	int objectIndex2 = entityList[entityList[entityInFrontIndex].extraEntity].locationOnAllObj;

							        	for (int i = 0; i < allObj[objectIndex].sizeT; i++)
							        	allObj[objectIndex].solidColor[i] = combineColors(allObj[objectIndex].solidColor[i], 0xfa8a, 50);

							        	for (int i = 0; i < allObj[objectIndex2].sizeT; i++)
							        	allObj[objectIndex2].solidColor[i] = combineColors(allObj[objectIndex2].solidColor[i], 0xfa8a, 50);

                                        entityList[entityInFrontIndex].renderTextured = false;
                                        entityList[entityList[entityInFrontIndex].extraEntity].renderTextured = false;
                                    
							        	entityList[entityInFrontIndex].state = 5;
							        }
                                    if(entityList[entityInFrontIndex].type == 2) //pig
							        {
                                        entityList[entityInFrontIndex].health -= damage;

							        	int objectIndex = entityList[entityInFrontIndex].locationOnAllObj;

							        	for (int i = 0; i < allObj[objectIndex].sizeT; i++)
							        	allObj[objectIndex].solidColor[i] = combineColors(allObj[objectIndex].solidColor[i], 0xfa8a, 50);

                                        entityList[entityInFrontIndex].renderTextured = false;
                                    
							        	entityList[entityInFrontIndex].state = 5;
							        }
                                    
                                    justPressedHitButton = true;
                                }

                                hasEntityInFront = true;
                            }
                        }

                        if(hasEntityInFront == false)
                        {
                            destructionTimer += deltaTime;
                            updateBlockReach();

                            int currentChunk = (removeBlock.x / width) + (removeBlock.z / width) * totalChunkWidth;
                            int blockIndex = (removeBlock.x % width) + (removeBlock.z % width) * width + (removeBlock.y) * width * width;

							if(destructionBlockIndex != blockIndex) //reset if looking at diffrent block
							{
								totalDestructionTimer = 0;
                                destructionTimer = 0;
							}
							destructionBlockIndex = blockIndex;

                            int blockType = blocks[currentChunk][blockIndex];
                            int currentToolLevel =  allItem[blockSelected].toolLevel;
                            int currentToolType = allItem[blockSelected].toolType;

                            if(currentToolType == allBlock[blockType].toolToBeak)
                            totalDestructionTimer = allBlock[blockType].destroyTime[currentToolLevel];
                            else
                            totalDestructionTimer = allBlock[blockType].destroyTime[0];

                            if(destructionTimer > totalDestructionTimer)
                            {
                                totalDestructionTimer = 0;
                                destructionTimer = 0;
                                destroyBlock();
                            }
                        }
                    }
                    else
                    {
                        justPressedHitButton = false;
                        totalDestructionTimer = 10;
                        destructionTimer = 0;
                    }
                }
                else
                {
                    if(keydownlast(KEY_PRGM_7) && !keydownhold(KEY_PRGM_7))
					{
						bool hasEntityInFront = false;

                        if(entityInFrontIndex != -1)
                        {
                            if(entityList[entityInFrontIndex].type != 3)
                            {
								int damage = 1;
								if(blockSelected > -1)
								damage = allItem[blockSelected].damage;

								if(entityList[entityInFrontIndex].type == 1) //sheep
								{
                                    entityList[entityInFrontIndex].health -= damage;

									int objectIndex = entityList[entityInFrontIndex].locationOnAllObj;
									int objectIndex2 = entityList[entityList[entityInFrontIndex].extraEntity].locationOnAllObj;

									for (int i = 0; i < allObj[objectIndex].sizeT; i++)
									allObj[objectIndex].solidColor[i] = combineColors(allObj[objectIndex].solidColor[i], 0xfa8a, 50);

									for (int i = 0; i < allObj[objectIndex2].sizeT; i++)
									allObj[objectIndex2].solidColor[i] = combineColors(allObj[objectIndex2].solidColor[i], 0xfa8a, 50);
									
									entityList[entityInFrontIndex].state = 5;
								}
                                if(entityList[entityInFrontIndex].type == 2) //pig
								{
                                    entityList[entityInFrontIndex].health -= damage;
                                    
									int objectIndex = entityList[entityInFrontIndex].locationOnAllObj;

									for (int i = 0; i < allObj[objectIndex].sizeT; i++)
									allObj[objectIndex].solidColor[i] = combineColors(allObj[objectIndex].solidColor[i], 0xfa8a, 50);
									
									entityList[entityInFrontIndex].state = 5;
								}

                                hasEntityInFront = true;
                            }
                        }

						if(hasEntityInFront == false)
                    	destroyBlock();
					}
                }
                if(keydownlast(KEY_PRGM_9) && !keydownhold(KEY_PRGM_9))
                {
                    useBlock();
                }
                if(keydownlast(KEY_PRGM_F1) && !keydownhold(KEY_PRGM_F1))
                {
                    currentSlot--;
                    if(currentSlot == -1)
                    currentSlot = 8;

                    if(hotbarBlockTypes[currentSlot] != -1)
                    blockSelected = hotbarBlockTypes[currentSlot];
                    else
                    blockSelected = -1;
                }
                if(keydownlast(KEY_PRGM_F2) && !keydownhold(KEY_PRGM_F2))
                {
                    currentSlot++;
                    if(currentSlot == 9)
                    currentSlot = 0;

                    if(hotbarBlockTypes[currentSlot] != -1)
                    blockSelected = hotbarBlockTypes[currentSlot];
                    else
                    blockSelected = -1;
                }
                if(keydownlast(KEY_PRGM_F3) && !keydownhold(KEY_PRGM_F3))
                {
                    makeExplosion(9, PPosX, PPosY, PPosZ);
                }
                if(keydownlast(KEY_PRGM_F4) && !keydownhold(KEY_PRGM_F4))
                {
                    skyBrightness++;
                    if(skyBrightness > 16)
                    skyBrightness = 16;
                    else
                    {
                        //changeBrightnessAllTriangles(1);

                        loadLighting2();
                        updateAllChunkLighting();
                    }
                    for (int i = 0; i < 16; i++)
                    screenColor[i] = skyColors[skyBrightness-1];
                }
                if(keydownlast(KEY_PRGM_F5) && !keydownhold(KEY_PRGM_F5))
                {
                    skyBrightness--;
                    if(skyBrightness < 1)
                    skyBrightness = 1;
                    else
                    {
                        //changeBrightnessAllTriangles(-1);

                        loadLighting2();
                        updateAllChunkLighting();
                    }
                    for (int i = 0; i < 16; i++)
                    screenColor[i] = skyColors[skyBrightness-1];
                }
            }

            performanceTime2 = RTC_GetTicks();
            performace[2] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();

			cameraRotationX = ToRadians(rotationX); //recalculate because we just moved the camera
            cameraRotationY = ToRadians(rotationY);
			forward = CalculateForwardVector(cameraRotationX, cameraRotationY);
            right = CalculateRightVector(cameraRotationX, cameraRotationY);

            //screenPoint calculatedSPos[verticesLength]; //cg10/20
            screenPoint *calculatedSPos = 0x8C2DC800 + ALLOC_OFFSET; //cg50

            renderdV = 0;
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

            int PPosXI10 = PPosX * 10;
            int PPosYI10 = PPosY * 10;
            int PPosZI10 = PPosZ * 10;

            if(PPosXI10 == 0) PPosXI10 = 1;
            if(PPosYI10 == 0) PPosYI10 = 1;
            if(PPosZI10 == 0) PPosZI10 = 1;

            int totalAmount_ = 0;

            for (int obj = 0; obj < objLength; obj++)
            {
                if(allObj[obj].active == true)
                {
                    for (int allVert = 0; allVert < allObj[obj].sizeV; allVert++)
                    {
                        int x3D = (allObj[obj].verticesIPC[allVert].x * 100 - PPosXI);
                        int y3D = (allObj[obj].verticesIPC[allVert].y * 100 - PPosYI);
                        int z3D = (PPosZI - allObj[obj].verticesIPC[allVert].z * 100);

                        int z = (x3D * cosXSinY - y3D * sinRotX + z3D * cosXcosY) / 1000;

						if(z < 1)
						z = 1;

                        if(z > 1 && z < 65000)
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
                            else
                            {
                                int zNew = 10000 / z;

                                x = x / 10000;
                                y = y / 10000;

                                xOnScreen = ((x * zNew + 1000) * resX_) / 1000;
                                yOnScreen = (((1000 - y * zNew) * 2) * resY_) / 1000 - 100;
                            }

                            if(xOnScreen < 4096 && xOnScreen > -4096 && yOnScreen < 4096 && yOnScreen > -4096)
                            {
                                calculatedSPos[totalAmount_ + allVert].x = xOnScreen;
                                calculatedSPos[totalAmount_ + allVert].y = yOnScreen;
                                calculatedSPos[totalAmount_ + allVert].z = z;

                                renderdV++;
                            }
                            else
                            calculatedSPos[totalAmount_ + allVert].z = 65001;
                        }
                        else
                        calculatedSPos[totalAmount_ + allVert].z = 65001;
                    }
                    totalAmount_ += allObj[obj].sizeV;
                }
            }

            performanceTime2 = RTC_GetTicks();
            performace[3] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();

            if((renderingMode == 2) || (renderingMode != 2 && clearScreen == true))
            {
                if(beautifulSky == false)
                {
                    dmaStartFill((int)screenColor, (int)(GetVRAMAddress()) & 0x1FFFFFFF, 165888);
                    DmaWaitNextC0();
                }
                else
                makeSkyBox();
            }

            performanceTime2 = RTC_GetTicks();
            performace[4] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();

            //rasterization draw loop
            renderdT = 0;

            short currentWaterQuadLocation = 0;
            short waterQuads[width*width*activeChunks];
            short waterQuadsObj[width*width*activeChunks];

            short currentTransQuadLocation = 0;
            short transQuads[1024];
            short transQuadsObj[1024];

            int totalAmount = 0;
            for (int obj = 0; obj < 64; obj++)
            {
                if(allObj[obj].active == true)
                {
                    for (int alltri = 0; alltri < allObj[obj].sizeT; alltri++)
                    {
                        Vector4S indexes1 = allObj[obj].triangles[alltri];

                        screenPoint v1 = calculatedSPos[totalAmount + indexes1.x];
                        screenPoint v2 = calculatedSPos[totalAmount + indexes1.y];
                        screenPoint v3 = calculatedSPos[totalAmount + indexes1.z];
                        screenPoint v4 = calculatedSPos[totalAmount + indexes1.w];

                        if(v1.z != 65001 && v2.z != 65001 && v3.z != 65001 && v4.z != 65001)
                        {
                            if((v1.x > 0 && v1.x < resX) || (v2.x > 0 && v2.x < resX) || (v3.x > 0 && v3.x < resX) || (v4.x > 0 && v4.x < resX))
                            {
                                if((v1.y > 0 && v1.y < resY) || (v2.y > 0 && v2.y < resY) || (v3.y > 0 && v3.y < resY) || (v4.y > 0 && v4.y < resY))
                                {
                                    if(renderTextured == false)
                                    {
                                        if((allObj[obj].color[alltri] == 12 || allObj[obj].color[alltri] == 13) && renderUnderwater == true)
                                        {
                                            waterQuads[currentWaterQuadLocation] = alltri;
                                            waterQuadsObj[currentWaterQuadLocation] = obj;
                                            currentWaterQuadLocation++;
                                            continue;
                                        }
                                        if(allObj[obj].color[alltri] == 35)
                                        {
                                            transQuads[currentTransQuadLocation] = alltri;
                                            transQuadsObj[currentTransQuadLocation] = obj;
                                            currentTransQuadLocation++;
                                            continue;
                                        }
                                    }
                                    else
                                    {
                                        if(textureType[allObj[obj].color[alltri]] == 2 && renderUnderwater == true)
                                        {
                                            waterQuads[currentWaterQuadLocation] = alltri;
                                            waterQuadsObj[currentWaterQuadLocation] = obj;
                                            currentWaterQuadLocation++;
                                            continue;
                                        }
                                        if(textureType[allObj[obj].color[alltri]] == 1)
                                        {
                                            transQuads[currentTransQuadLocation] = alltri;
                                            transQuadsObj[currentTransQuadLocation] = obj;
                                            currentTransQuadLocation++;
                                            continue;
                                        }
                                    }

                                    Vector3S v1_ = allObj[obj].verticesIPC[indexes1.x];
                                    Vector3S v2_ = allObj[obj].verticesIPC[indexes1.y];
                                    Vector3S v3_ = allObj[obj].verticesIPC[indexes1.z];

                                    Vector3I direction;
                                    direction.x = v1_.x - PPosXI10;
                                    direction.y = v1_.y - PPosYI10;
                                    direction.z = v1_.z - PPosZI10;

                                    Vector3I normal = {allObj[obj].normal[alltri].x, allObj[obj].normal[alltri].y, allObj[obj].normal[alltri].z};

                                    if(dotVec3IND(normal, direction) < -10)
                                    {
                                        renderdT++;

                                        if(renderingMode == 0 || renderingMode == 2) //normal mode
                                        {
                                            int Depth = (v1.z + v2.z + v3.z + v4.z) >> 2;

                                            Vector2S V1_ = {v1.x, v1.y};
                                            Vector2S V2_ = {v2.x, v2.y};
                                            Vector2S V3_ = {v3.x, v3.y};
                                            Vector2S V4_ = {v4.x, v4.y};

                                            int brightness = allObj[obj].brightnes[alltri];
                                            if(brightness < 1)
                                            brightness = 1;

                                            if(renderTextured == false)
                                            {
                                                if(pixelSize == 2)
                                                {
                                                    if(brightness != 16)
                                                    {
                                                        renderTrianglePX2(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                                                        renderTrianglePX2(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                                                    }
                                                    else
                                                    {
                                                        renderTriangleNBPX2(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth);
                                                        renderTriangleNBPX2(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth);
                                                    }
                                                }
                                                else
                                                {
                                                    if(brightness != 16)
                                                    {
                                                        renderTriangle(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                                                        renderTriangle(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                                                    }
                                                    else
                                                    {
                                                        renderTriangleNB(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth);
                                                        renderTriangleNB(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                
                                                Vector2S AT = {0, 0};
                                                Vector2S BT = {10, 0};
                                                Vector2S CT = {10, 10};
                                                Vector2S DT = {0, 10};

                                                int texX = allObj[obj].textureSize[alltri] & 0x0F;
                                                int texY = (allObj[obj].textureSize[alltri] >> 4) & 0x0F;

                                                texX += 1;
                                                texY += 1;

                                                if(pixelSize == 2)
                                                {
                                                    if(brightness != 16)
                                                    {
                                                        renderTriangleTexturedPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                        renderTriangleTexturedPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                    }
                                                    else
                                                    {
                                                        renderTriangleTexturedNBPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                        renderTriangleTexturedNBPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                    }
                                                }
                                                else
                                                {
                                                    if(brightness != 16)
                                                    {
                                                        renderTriangleTexturedT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                        renderTriangleTexturedT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                    }
                                                    else
                                                    {
                                                        renderTriangleTexturedNBT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                        renderTriangleTexturedNBT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                    }
                                                }
                                            }
                                        }
                                        if(renderingMode == 1 || renderingMode == 2) //wireframe
                                        {
                                            renderLine(v1.x, v2.x, v1.y, v2.y);
                                            renderLine(v2.x, v3.x, v2.y, v3.y);
                                            renderLine(v3.x, v4.x, v3.y, v4.y);
                                            renderLine(v4.x, v1.x, v4.y, v1.y);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    totalAmount += allObj[obj].sizeV;
                }
            }

            for (int obj = 100; obj < objLength; obj++)
            {
                if(allObj[obj].active == true)
                {
                    for (int alltri = 0; alltri < allObj[obj].sizeT; alltri++)
                    {
                        Vector4S indexes1 = allObj[obj].triangles[alltri];

                        screenPoint v1 = calculatedSPos[totalAmount + indexes1.x];
                        screenPoint v2 = calculatedSPos[totalAmount + indexes1.y];
                        screenPoint v3 = calculatedSPos[totalAmount + indexes1.z];
                        screenPoint v4 = calculatedSPos[totalAmount + indexes1.w];

                        if(v1.z != 65001 && v2.z != 65001 && v3.z != 65001 && v4.z != 65001)
                        {
                            if((v1.x > 0 && v1.x < resX) || (v2.x > 0 && v2.x < resX) || (v3.x > 0 && v3.x < resX) || (v4.x > 0 && v4.x < resX))
                            {
                                if((v1.y > 0 && v1.y < resY) || (v2.y > 0 && v2.y < resY) || (v3.y > 0 && v3.y < resY) || (v4.y > 0 && v4.y < resY))
                                {
                                    renderdT++;

                                    if(renderingMode == 0 || renderingMode == 2) //normal mode
                                    {
                                        int Depth = (v1.z + v2.z + v3.z + v4.z) >> 2;

                                        Vector2S V1_ = {v1.x, v1.y};
                                        Vector2S V2_ = {v2.x, v2.y};
                                        Vector2S V3_ = {v3.x, v3.y};
                                        Vector2S V4_ = {v4.x, v4.y};

                                        int brightness = 16;
                                        if(lighting == true)
                                        {
                                            Vector3I pos;
                                            pos.x = (int)allObj[obj].position.x;
                                            pos.y = (int)allObj[obj].position.y;
                                            pos.z = (int)allObj[obj].position.z;

                                            int px = pos.x % width;
                                            int pz = pos.z % width;

                                            int chunkIndex = (pos.x / width) + (pos.z / width) * totalChunkWidth;
                                            int blockIndex = px + pz * width + pos.y * width * width;
                                            int lightIndex = blockIndex + chunkIndex * width * width * height;

                                            if(lighting == true)
                                            brightness = lightmap[lightIndex];
                                            else
                                            brightness = skyBrightness;
                                        }

                                        if(entityList[obj-100].renderTextured == false)
                                        {
                                            if(pixelSize == 2)
                                            {
                                                if(brightness != 16)
                                                {
                                                    renderTrianglePX2(&V1_, &V2_, &V3_, allObj[obj].solidColor[alltri], Depth, brightness);
                                                    renderTrianglePX2(&V1_, &V3_, &V4_, allObj[obj].solidColor[alltri], Depth, brightness);
                                                }
                                                else
                                                {
                                                    renderTriangleNBPX2(&V1_, &V2_, &V3_, allObj[obj].solidColor[alltri], Depth);
                                                    renderTriangleNBPX2(&V1_, &V3_, &V4_, allObj[obj].solidColor[alltri], Depth);
                                                }
                                            }
                                            else
                                            {
                                                if(brightness != 16)
                                                {
                                                    renderTriangle(&V1_, &V2_, &V3_, allObj[obj].solidColor[alltri], Depth, brightness);
                                                    renderTriangle(&V1_, &V3_, &V4_, allObj[obj].solidColor[alltri], Depth, brightness);
                                                }
                                                else
                                                {
                                                    renderTriangleNB(&V1_, &V2_, &V3_, allObj[obj].solidColor[alltri], Depth);
                                                    renderTriangleNB(&V1_, &V3_, &V4_, allObj[obj].solidColor[alltri], Depth);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            Vector2S AT = {0, 0};
                                            Vector2S BT = {10, 0};
                                            Vector2S CT = {10, 10};
                                            Vector2S DT = {0, 10};

                                            int texX = allObj[obj].textureSize[alltri] & 0x0F;
                                            int texY = (allObj[obj].textureSize[alltri] >> 4) & 0x0F;

                                            texX += 1;
                                            texY += 1;

                                            if(pixelSize == 2)
                                            {
                                                if(brightness != 16)
                                                {
                                                    renderTriangleTexturedBlackTransPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                    renderTriangleTexturedBlackTransPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                }
                                                else
                                                {
                                                    renderTriangleTexturedBlackTransNBPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                    renderTriangleTexturedBlackTransNBPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                }
                                            }
                                            else
                                            {
                                                if(brightness != 16)
                                                {
                                                    renderTriangleTexturedBlackTransT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                    renderTriangleTexturedBlackTransT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                                }
                                                else
                                                {
                                                    renderTriangleTexturedBlackTransNBT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                    renderTriangleTexturedBlackTransNBT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                                }
                                            }
                                        }
                                    }
                                    if(renderingMode == 1 || renderingMode == 2) //wireframe
                                    {
                                        renderLine(v1.x, v2.x, v1.y, v2.y);
                                        renderLine(v2.x, v3.x, v2.y, v3.y);
                                        renderLine(v3.x, v4.x, v3.y, v4.y);
                                        renderLine(v4.x, v1.x, v4.y, v1.y);
                                    }
                                }
                            }
                        }
                    }
                    totalAmount += allObj[obj].sizeV;
                }
            }

            for (int i = 0; i < currentTransQuadLocation; i++)
            {
                int alltri = transQuads[i];
                int obj = transQuadsObj[i];

                int totalAmountTrans = 0;

                for (int j = 0; j < obj; j++)
                {
                    if(allObj[j].active == true)
                    totalAmountTrans += allObj[j].sizeV;
                }

                Vector4S indexes1 = allObj[obj].triangles[alltri];

                screenPoint v1 = calculatedSPos[totalAmountTrans + indexes1.x];
                screenPoint v2 = calculatedSPos[totalAmountTrans + indexes1.y];
                screenPoint v3 = calculatedSPos[totalAmountTrans + indexes1.z];
                screenPoint v4 = calculatedSPos[totalAmountTrans + indexes1.w];

                if(renderingMode == 0 || renderingMode == 2) //normal mode
                {
                    int Depth = (v1.z + v2.z + v3.z + v4.z) >> 2;

                    Vector2S V1_ = {v1.x, v1.y};
                    Vector2S V2_ = {v2.x, v2.y};
                    Vector2S V3_ = {v3.x, v3.y};
                    Vector2S V4_ = {v4.x, v4.y};

                    int brightness = allObj[obj].brightnes[alltri];
                    if(brightness < 1)
                    brightness = 1;

                    renderdT++;

                    Vector2S AT = {0, 0};
                    Vector2S BT = {10, 0};
                    Vector2S CT = {10, 10};
                    Vector2S DT = {0, 10};

                    int texX = allObj[obj].textureSize[alltri] & 0x0F;
                    int texY = (allObj[obj].textureSize[alltri] >> 4) & 0x0F;

                    texX += 1;
                    texY += 1;

                    if(pixelSize == 2)
                    {
                        if(brightness != 16)
                        {
                            renderTriangleTexturedBlackTransPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                            renderTriangleTexturedBlackTransPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                        }
                        else
                        {
                            renderTriangleTexturedBlackTransNBPX2T(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                            renderTriangleTexturedBlackTransNBPX2T(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                        }
                    }
                    else
                    {
                        if(brightness != 16)
                        {
                            renderTriangleTexturedBlackTransT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                            renderTriangleTexturedBlackTransT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                        }
                        else
                        {
                            renderTriangleTexturedBlackTransNBT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                            renderTriangleTexturedBlackTransNBT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                        }
                    }
                }
                if(renderingMode == 1 || renderingMode == 2) //wireframe
                {
                    renderLine(v1.x, v2.x, v1.y, v2.y);
                    renderLine(v2.x, v3.x, v2.y, v3.y);
                    renderLine(v3.x, v4.x, v3.y, v4.y);
                    renderLine(v4.x, v1.x, v4.y, v1.y);
                }
            }

            for (int i = 0; i < currentWaterQuadLocation; i++)
            {
                int alltri = waterQuads[i];
                int obj = waterQuadsObj[i];

                int totalAmountWater = 0;

                for (int j = 0; j < obj; j++)
                {
                    if(allObj[j].active == true)
                    totalAmountWater += allObj[j].sizeV;
                }

                Vector4S indexes1 = allObj[obj].triangles[alltri];

                screenPoint v1 = calculatedSPos[totalAmountWater + indexes1.x];
                screenPoint v2 = calculatedSPos[totalAmountWater + indexes1.y];
                screenPoint v3 = calculatedSPos[totalAmountWater + indexes1.z];
                screenPoint v4 = calculatedSPos[totalAmountWater + indexes1.w];

                if(renderingMode == 0 || renderingMode == 2) //normal mode
                {
                    int Depth = (v1.z + v2.z + v3.z + v4.z) >> 2;

                    Vector2S V1_ = {v1.x, v1.y};
                    Vector2S V2_ = {v2.x, v2.y};
                    Vector2S V3_ = {v3.x, v3.y};
                    Vector2S V4_ = {v4.x, v4.y};

                    int brightness = allObj[obj].brightnes[alltri];
                    if(brightness < 1)
                    brightness = 1;

                    renderdT++;

                    if(renderTextured == false)
                    {
                        if(pixelSize == 2)
                        {
                            if(brightness != 16)
                            {
                                renderTrianglePX2Trans(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                                renderTrianglePX2Trans(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                            }
                            else
                            {
                                renderTriangleNBPX2Trans(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth);
                                renderTriangleNBPX2Trans(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth);
                            }
                        }
                        else
                        {
                            if(brightness != 16)
                            {
                                renderTriangleTrans(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                                renderTriangleTrans(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth, brightness);
                            }
                            else
                            {
                                renderTriangleNBTrans(&V1_, &V2_, &V3_, allColors[allObj[obj].color[alltri]], Depth);
                                renderTriangleNBTrans(&V1_, &V3_, &V4_, allColors[allObj[obj].color[alltri]], Depth);
                            }
                        }
                    }
                    else
                    {
                        Vector2S AT = {0, 0};
                        Vector2S BT = {10, 0};
                        Vector2S CT = {10, 10};
                        Vector2S DT = {0, 10};

                        int texX = allObj[obj].textureSize[alltri] & 0x0F;
                        int texY = (allObj[obj].textureSize[alltri] >> 4) & 0x0F;

                        texX += 1;
                        texY += 1;

                        if(pixelSize == 2)
                        {
                            if(brightness != 16)
                            {
                                renderTriangleTexturedPX2TransT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                renderTriangleTexturedPX2TransT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                            }
                            else
                            {
                                renderTriangleTexturedNBPX2TransT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                renderTriangleTexturedNBPX2TransT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                            }
                        }
                        else
                        {
                            if(brightness != 16)
                            {
                                renderTriangleTexturedTrans(&V1_, &V2_, &V3_, &AT, &BT, &CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                                renderTriangleTexturedTrans(&V1_, &V3_, &V4_, &AT, &CT, &DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256], brightness);
                            }
                            else
                            {
                                renderTriangleTexturedNBTransT(V1_, V2_, V3_, AT, BT, CT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                                renderTriangleTexturedNBTransT(V1_, V3_, V4_, AT, CT, DT, Depth, texX, texY, &textures2[allObj[obj].color[alltri] * 256]);
                            }
                        }
                    }
                }
                if(renderingMode == 1 || renderingMode == 2) //wireframe
                {
                    renderLine(v1.x, v2.x, v1.y, v2.y);
                    renderLine(v2.x, v3.x, v2.y, v3.y);
                    renderLine(v3.x, v4.x, v3.y, v4.y);
                    renderLine(v4.x, v1.x, v4.y, v1.y);
                }
            }

            performanceTime2 = RTC_GetTicks();
            performace[5] += performanceTime2-performanceTime1;

            for (int allparti = 0; allparti < partiLength; allparti++)
            {
                particle currentParti = allparticles[allparti];
                if(currentParti.used == true)
                {
                    screenPoint sp;

                    int x3D = (currentParti.position.x * 1000 - PPosXI);
                    int y3D = (currentParti.position.y * 1000 - PPosYI);
                    int z3D = (PPosZI - currentParti.position.z * 1000);

                    int z = (x3D * cosXSinY - y3D * sinRotX + z3D * cosXcosY) / 1000;

                    if(z > 10 && z < 65000)
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
                        else
                        {
                            int zNew = 10000 / z;

                            x = x / 10000;
                            y = y / 10000;

                            xOnScreen = ((x * zNew + 1000) * resX_) / 1000;
                            yOnScreen = (((1000 - y * zNew) * 2) * resY_) / 1000 - 100;
                        }

                        if(xOnScreen < 32768 && xOnScreen > -32768 && yOnScreen < 32768 && yOnScreen > -32768)
                        {
                            sp.x = xOnScreen;
                            sp.y = yOnScreen;
                            sp.z = z;

                            allparticles[allparti].time += deltaTime;
                            if(allparticles[allparti].time < allparticles[allparti].maxTime)
                            {
                                renderParticle(10, currentParti.mode, currentParti.color, sp, 10);

                                if(currentParti.gravity == true)
                                allparticles[allparti].velocity.y -= 9.81f * deltaTimeNoSlow;

                                allparticles[allparti].position.x += allparticles[allparti].velocity.x * deltaTime;
                                allparticles[allparti].position.y += allparticles[allparti].velocity.y * deltaTime;
                                allparticles[allparti].position.z += allparticles[allparti].velocity.z * deltaTime;
                            }
                            else
                            deleteParticle(allparti);
                        }
                    }
                }
            }

            performanceTime1 = RTC_GetTicks();

            if(checkIfInWater(PPosX, PPosY, PPosZ) == true)
            renderUnderWaterEffect();

            if(destructionTimer != 0)
			renderBlockDestruction(destructionTimer, totalDestructionTimer);

            performanceTime2 = RTC_GetTicks();
            performace[6] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();
			
            memset(ZBuffer, 255, resXZBuffer * resYZBuffer * 2);

            performanceTime2 = RTC_GetTicks();
            performace[7] += performanceTime2-performanceTime1;

            performanceTime1 = RTC_GetTicks();

            renderCorsAir(renderBlockOutline);
        }
        else
        {
            if((renderingMode == 2) || (renderingMode != 2 && clearScreen == true))
            {
                if(beautifulSky == false)
                {
                    dmaStartFill((int)screenColor, (int)(GetVRAMAddress()) & 0x1FFFFFFF, 165888);
                    DmaWaitNextC0();
                }
                else
                makeSkyBox();
            }
        }

        makeUI();

        performanceTime2 = RTC_GetTicks();
        performace[8] += performanceTime2-performanceTime1;

        performanceTime1 = RTC_GetTicks();

        DoDMAlcdNonblock();
        DmaWaitNext();

        performanceTime2 = RTC_GetTicks();
        performace[9] += performanceTime2-performanceTime1;
    }
    DmaWaitNext();
}
