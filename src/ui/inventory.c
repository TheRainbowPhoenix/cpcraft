/* src/ui/inventory.c - Full inventory, crafting, chest UI */
#include "engine.h"
#include "config.h"


void swapItems(int cursorX1, int cursorY1, int cursorX2, int cursorY2)
{
    short *amountC1;
    short *typeC1;
	short *itemHp1;
    short *amountC2;
    short *typeC2;
	short *itemHp2;

    if(cursorY1 == 3){
        typeC1 = &hotbarBlockTypes[cursorX1];
        amountC1 = &hotbarBlockAmount[cursorX1];
		itemHp1 = &hotbarBlockHp[cursorX1];
    }
    else{
        typeC1 = &inventoryBlockTypes[cursorY1 * 9 + cursorX1];
        amountC1 = &inventoryBlockAmount[cursorY1 * 9 + cursorX1];
		itemHp1 = &inventoryBlockHp[cursorY1 * 9 + cursorX1];
    }

    if(cursorY2 == 3){
        typeC2 = &hotbarBlockTypes[cursorX2];
        amountC2 = &hotbarBlockAmount[cursorX2];
		itemHp2 = &hotbarBlockHp[cursorX2];
    }
    else{
        typeC2 = &inventoryBlockTypes[cursorY2 * 9 + cursorX2];
        amountC2 = &inventoryBlockAmount[cursorY2 * 9 + cursorX2];
		itemHp2 = &inventoryBlockHp[cursorY2 * 9 + cursorX2];
    }

    int tempAmount = *amountC2;
    int tempType = *typeC2;
	int tempHp = *itemHp2;

    *amountC2 = *amountC1;
    *typeC2 = *typeC1;
	*itemHp2 = *itemHp1;

    *amountC1 = tempAmount;
    *typeC1 = tempType;
	*itemHp1 = tempHp;
}
void swapItemsChest(int cursorX1, int cursorY1, int cursorX2, int cursorY2)
{
    short *amountC1;
    short *typeC1;
	short *itemHp1;
    short *amountC2;
    short *typeC2;
	short *itemHp2;

    if(cursorY1 == 6)
    {
        typeC1 = &hotbarBlockTypes[cursorX1];
        amountC1 = &hotbarBlockAmount[cursorX1];
		itemHp1 = &hotbarBlockHp[cursorX1];
    }
    else if(cursorY1 >= 3 && cursorY1 < 6)
    {
        typeC1 = &inventoryBlockTypes[(cursorY1-3) * 9 + cursorX1];
        amountC1 = &inventoryBlockAmount[(cursorY1-3) * 9 + cursorX1];
		itemHp1 = &inventoryBlockHp[(cursorY1-3) * 9 + cursorX1];
    }
    else if(cursorY1 >= 0 && cursorY1 < 3)
    {
        typeC1 = &allchest[currentChestIndex].chestBlockTypes[cursorY1 * 9 + cursorX1];
        amountC1 = &allchest[currentChestIndex].chestBlockAmount[cursorY1 * 9 + cursorX1];
		itemHp1 = &allchest[currentChestIndex].chestBlockHp[cursorY1 * 9 + cursorX1];
    }

    if(cursorY2 == 6)
    {
        typeC2 = &hotbarBlockTypes[cursorX2];
        amountC2 = &hotbarBlockAmount[cursorX2];
		itemHp2 = &hotbarBlockHp[cursorX2];
    }
    else if(cursorY2 >= 3 && cursorY2 < 6)
    {
        typeC2 = &inventoryBlockTypes[(cursorY2-3) * 9 + cursorX2];
        amountC2 = &inventoryBlockAmount[(cursorY2-3) * 9 + cursorX2];
		itemHp2 = &inventoryBlockHp[(cursorY2-3) * 9 + cursorX2];
    }
    else if(cursorY2 >= 0 && cursorY2 < 3)
    {
        typeC2 = &allchest[currentChestIndex].chestBlockTypes[cursorY2 * 9 + cursorX2];
        amountC2 = &allchest[currentChestIndex].chestBlockAmount[cursorY2 * 9 + cursorX2];
		itemHp2 = &allchest[currentChestIndex].chestBlockHp[cursorY2 * 9 + cursorX2];
    }

    int tempAmount = *amountC2;
    int tempType = *typeC2;
	int tempHp = *itemHp2;

    *amountC2 = *amountC1;
    *typeC2 = *typeC1;
	*itemHp2 = *itemHp1;

    *amountC1 = tempAmount;
    *typeC1 = tempType;
	*itemHp1 = tempHp;
}

void makeUI()
{
    /*game*/if(UIState == 0)
    {
        char bufferFPS[50];
        sprintf(bufferFPS, "FPS: %d", (int)fps);
        renderText(330, 5, bufferFPS);

        if(showPerformaceGraph == true)
        {
            updatePerformanceLog();

            char bufferX[50];
            sprintf(bufferX, "X: %d", (int)PPosX);
            renderText(330, 20, bufferX);

            char bufferY[50];
            sprintf(bufferY, "Y: %d", (int)PPosY);
            renderText(330, 35, bufferY);

            char bufferZ[50];
            sprintf(bufferZ, "Z: %d", (int)PPosZ);
            renderText(330, 50, bufferZ);

            char bufferVertice[50];
            sprintf(bufferVertice, "vert: %d/%d", renderdV, verticesLength);
            renderText(205, 5, bufferVertice);

            char bufferTriangle[50];
            sprintf(bufferTriangle, "quads: %d/%d", renderdT, triangleLength);
            renderText(205, 20, bufferTriangle);

            char bufferEntity[50];
            sprintf(bufferEntity, "entitys: %d/%d", usedEntitys, entityLength);
            renderText(205, 35, bufferEntity);

            char bufferRX[50];
            sprintf(bufferRX, "rotationX: %d", (int)rotationX);
            renderText(205, 50, bufferRX);

            char bufferRY[50];
            sprintf(bufferRY, "rotationY: %d", (int)rotationY);
            renderText(205, 65, bufferRY);
        }

        //0.1ms
        makeHotbar(currentSlot);

        if(survival == true)
        makeHealthBar();
    }
    if(UIState == 9 ) //startup menu
    {
        int Buttonlength = 3;
        char text[3][50] = 
        {
            "Singleplayer",
            "Options",
            "Quit"
        };
        int textLength[3] = {12, 7, 4};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 0)
            {
                UIState = 10;
                cursorY = 0;
            }

            if(cursorY == 1)
            {
                UIState = 12;
                cursorY = 0;
            }

            if(cursorY == 2)
            ExitLoop = true;
        }

        for (int y = 0; y < 216; y++)
        {
            for (int x = 0; x < 384; x++)
            {
                int xOnTexture = x%16;
                int yOnTexture = y%16;
                VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
            }
        }

        CopySprite(minercaftLogo, 76, 20, 231, 42, 0x4389);

        for (int i = 0; i < Buttonlength; i++)
        {
            if(cursorY != i)
            {
                CopySprite(buttons[0], 112, 100+i*30, 160, 20, 0x001);
                drawMinecraftFont(112, 272, 106+i*30, true, textLength[i], text[i], 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], 112, 100+i*30, 160, 20, 0x001);
                drawMinecraftFont(112, 272, 106+i*30, true, textLength[i], text[i], 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 10) //world selection
    {
        int Buttonlength = 6;
        char text[6][50] = 
        {
            "Slot 1 - ",
            "Slot 2 - ",
            "Slot 3 - ",
            "Slot 4 - ",
            "Slot 5 - ",
            "Cancel", 
        };
        int textLength[6] = {9, 9, 9, 9, 9, 6};
        int textHeight[6] = {30, 55, 80, 105, 130, 165};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = 9;
            cursorY = 0;
            WorldOpenMode = 0;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_SHIFT) && !keydownhold(KEY_PRGM_SHIFT))
        {
            WorldOpenMode++;
            if(WorldOpenMode == 4)
            WorldOpenMode = 0;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY < 5)
            {
                if(worldExists[cursorY] == 0 && WorldOpenMode == 0)
                {
                    UIState = 11;
                    currentSelectedWorld = cursorY+1;
                    cursorY = 0;
                    WorldOpenMode = 0;

                    dayTime = 1200;
                    dayTimeChange = 0;
                }
                else if(worldExists[cursorY] == 1 && WorldOpenMode == 0)
                {
                    loadAllChunk(cursorY+1);
                    UIState = 0;
                    currentSelectedWorld = cursorY+1;
                    cursorY = 0;
                    WorldOpenMode = 0;

                    if(dayNightCycle == false)
                    {
                        dayTime = 1200;
                        dayTimeChange = 0;
                    }

                    loadInChunks();
                }
                else if(WorldOpenMode == 1)
                {
                    worldExists[cursorY] = 0;
                    savegameData();
                    WorldOpenMode = 0;
                }
                else if(WorldOpenMode == 2 && worldExists[cursorY] == 0)
                {
                    UIState = 15;
                    currentSelectedWorld = cursorY+1;
                    cursorY = 0;
                    useCurrentVersion = false;
                    WorldOpenMode = 0;
                }
                else if(WorldOpenMode == 3 && worldExists[cursorY] == 1)
                {
                    UIState = 15;
                    currentSelectedWorld = cursorY+1;
                    loadWorldData(currentSelectedWorld);
                    cursorY = 0;
                    useCurrentVersion = true;
                    WorldOpenMode = 0;

                    dayTime = 1200;
                    dayTimeChange = 0;
                }
            }

            if(cursorY == 5)
            {
                UIState = 9;
                cursorY = 0;
                WorldOpenMode = 0;
            }
        }

        for (int y = 0; y < 216; y++)
        {
            for (int x = 0; x < 384; x++)
            {
                int xOnTexture = x%16;
                int yOnTexture = y%16;
                VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
            }
        }

        drawMinecraftFont(0, 384, 10, true, 12, "Select world", 0xdefb, 0x39c7);

        if(WorldOpenMode == 0)
        drawMinecraftFont(0, 384, 190, true, 17, "Mode: Create/Load", 0xdefb, 0x39c7);
        if(WorldOpenMode == 1)
        drawMinecraftFont(0, 384, 190, true, 12, "Mode: Delete", 0xdefb, 0x39c7);
        if(WorldOpenMode == 2)
        drawMinecraftFont(0, 384, 190, true, 31, "Mode: Convert old/Recover world", 0xdefb, 0x39c7);
        if(WorldOpenMode == 3)
        drawMinecraftFont(0, 384, 190, true, 21, "Mode: Edit world data", 0xdefb, 0x39c7);

        drawMinecraftFont(0, 384, 200, true, 27, "Press shift to change modes", 0xdefb, 0x39c7);

        for (int i = 0; i < Buttonlength; i++)
        {
            int extra = 0;
            char buttonText[50];
            sprintf(buttonText, "%s", text[i]);

            if(i < 5)
            {
                char temp[20];

                if(worldExists[i] == 0){
                    sprintf(temp, "Empty");
                    extra = 5;
                }
                if(worldExists[i] == 1){
                    sprintf(temp, "Used");
                    extra = 4;
                }

                strcat(buttonText, temp);
            }

            if(cursorY != i)
            {
                CopySprite(buttons[0], 112, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(112, 272, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], 112, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(112, 272, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 11) //world creation
    {
        int Buttonlength = 8;
        char text[8][50] = 
        {
            "superflat: ",
            "Seed: ",
            "Caves: ",
            "Survival: ",
            "Day/night cycle: ",
			"mobs: ",
            "Create world",
            "Cancel", 
        };
        int textLength[8] = {11, 6, 7, 10, 17, 6, 12, 6};
        int textHeight[8] = {30, 30, 55, 55, 80, 80, 115, 150};
        Vector2I textWidth[8] = {{0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 383}, {0, 383}};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = 10;
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
        {
            if(cursorY == 1)
            SEED--;
        }
        if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
        {
            if(cursorY == 1)
            SEED++;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 0)
            {
                if(advancedTerrain == 0)
                advancedTerrain = 2;
                else
                advancedTerrain = 0;
            }

            if(cursorY == 2)
            caves = !caves;

            if(cursorY == 3)
            survival = !survival;
            
            if(cursorY == 4)
            dayNightCycle = !dayNightCycle;

			if(cursorY == 5)
            mobs = !mobs;

            if(cursorY == 6)
            {
                if(SEED == 0)
                SEED = generate_random(RTC_GetTicks(), 999999999);

                for (int y = 0; y < totalChunkWidth; y++)
                {
                    for (int x = 0; x < totalChunkWidth; x++)
                    {
                        loadPerlin(x, y);

                        if(caves == true)
                        loadPerlinCaves(x, y);

                        loadingScreen(x + y*8 + 1, 64, "Loading...", 10);
                    }
                }

                if(lighting == true)
                loadLighting2();

                int seedExtra_ = 0;
                PPosY = 0;

                while(PPosY < 18)
                {
                    PPosX = generate_random(SEED+seedExtra_+1, 48) + 24.5;
                    PPosZ = generate_random(SEED+seedExtra_+2, 48) + 24.5;
                    PPosY = GetPerlin((int)PPosX, (int)PPosZ)+3;
                    seedExtra_++;
                }

                worldExists[currentSelectedWorld-1] = 1;
                worldVersion[currentSelectedWorld-1] = currentVersion;
                savegameData();

                saveAllChunk(currentSelectedWorld, false);

                UIState = 0;
                cursorY = 0;

                loadInChunks();
            }

            if(cursorY == 7)
            {
                UIState = 10;
                cursorY = 0;
            }
        }

        for (int y = 0; y < 216; y++)
        {
            for (int x = 0; x < 384; x++)
            {
                int xOnTexture = x%16;
                int yOnTexture = y%16;
                VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
            }
        }

        drawMinecraftFont(0, 384, 10, true, 14, "World Creation", 0xdefb, 0x39c7);

        for (int i = 0; i < Buttonlength; i++)
        {
            int extra = 0;
            char buttonText[50];
            sprintf(buttonText, "%s", text[i]);

            if(i < 6)
            {
                char temp[20];
                if(i == 0)
                {
                    if(advancedTerrain == 0){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    if(advancedTerrain == 2){
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 1)
                {
                    sprintf(temp, "%d", SEED);
                    extra = countDigits(SEED);
                }
                if(i == 2)
                {
                    if(caves == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 3)
                {
                    if(survival == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 4)
                {
                    if(dayNightCycle == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }

				if(i == 5)
                {
                    if(mobs == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }

                strcat(buttonText, temp);
            }

            int startX = (textWidth[i].y - textWidth[i].x - 160) / 2 + textWidth[i].x;
            int endX = startX + 160;
            if(cursorY != i)
            {
                CopySprite(buttons[0], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 15) //world conversion
    {
        int Buttonlength = 8;
        char text[8][50] = 
        {
            "superflat: ",
            "Seed: ",
            "Caves: ",
            "Survival: ",
            "Day/night cycle: ",
			"mobs: ",
            "Create world",
            "Cancel", 
        };
        int textLength[8] = {11, 6, 7, 10, 17, 6, 12, 6};
        int textHeight[8] = {30, 30, 55, 55, 80, 80, 115, 150};
        Vector2I textWidth[8] = {{0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 383}, {0, 383}};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = 10;
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
        {
            if(cursorY == 1)
            SEED--;
        }
        if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
        {
            if(cursorY == 1)
            SEED++;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 0)
            {
                if(advancedTerrain == 0)
                advancedTerrain = 2;
                else
                advancedTerrain = 0;
            }

            if(cursorY == 2)
            caves = !caves;

            if(cursorY == 3)
            survival = !survival;
            
            if(cursorY == 4)
            {
                dayNightCycle = !dayNightCycle;
                if(dayNightCycle == true)
                dayTime = 1200;
            }

			if(cursorY == 5)
            mobs = !mobs;

            if(cursorY == 6)
            {
                worldExists[currentSelectedWorld-1] = 1;
                if(useCurrentVersion == true)
                worldVersion[currentSelectedWorld-1] = currentVersion;
                else
                worldVersion[currentSelectedWorld-1] = 0;
                saveWorldData(currentSelectedWorld);
                savegameData();

                UIState = 10;
                cursorY = 0;
            }

            if(cursorY == 7)
            {
                UIState = 10;
                cursorY = 0;
            }
        }

        for (int y = 0; y < 216; y++)
        {
            for (int x = 0; x < 384; x++)
            {
                int xOnTexture = x%16;
                int yOnTexture = y%16;
                VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
            }
        }

        drawMinecraftFont(0, 384, 10, true, 22, "Create/Edit world data", 0xdefb, 0x39c7);

        for (int i = 0; i < Buttonlength; i++)
        {
            int extra = 0;
            char buttonText[50];
            sprintf(buttonText, "%s", text[i]);

            if(i < 6)
            {
                char temp[20];
                if(i == 0)
                {
                    if(advancedTerrain == 0){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    if(advancedTerrain == 2){
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 1)
                {
                    sprintf(temp, "%d", SEED);
                    extra = countDigits(SEED);
                }
                if(i == 2)
                {
                    if(caves == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 3)
                {
                    if(survival == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 4)
                {
                    if(dayNightCycle == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
				if(i == 5)
                {
                    if(mobs == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }

                strcat(buttonText, temp);
            }

            int startX = (textWidth[i].y - textWidth[i].x - 160) / 2 + textWidth[i].x;
            int endX = startX + 160;
            if(cursorY != i)
            {
                CopySprite(buttons[0], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 12) //settings startup menu
    {
        int Buttonlength = 12;
        char text[12][50] = 
        {
            "back", //0
            "Render distance: ", //1
            "Compress vertices: ", //2
            "Render underwater: ", //3
            "Render resolution: ", //4
            "Render textured: ", //5
            "Lighting: ", //6
            "Stop chunk loading: ", //7
            "Render mode: ", //8
			"Chunk update distance: ", //9
			"Beautiful Sky: ", //10
            "Texture packs", //11
        };
        int textLength[12] = {4, 17, 19, 19, 19, 17, 10, 20, 13, 23, 15, 13};
        int textHeight[12] = {30, 55, 55, 80, 80, 105, 105, 130, 130, 155, 155, 180};
        Vector2I textWidth[12] = {{0, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 383}};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = 9;
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(cursorY == 1)
        {
            if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
            {
                if(renderDistance != 8)
                renderDistance += 1;

				saveSettings();
            }
            if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
            {
                if(renderDistance != 0)
                renderDistance -= 1;

				saveSettings();
            }
        }
		if(cursorY == 9)
        {
            if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
            {
                if(redstoneRenderDistance != 4)
                redstoneRenderDistance += 1;

				saveSettings();
            }
            if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
            {
                if(redstoneRenderDistance != 0)
                redstoneRenderDistance -= 1;

				saveSettings();
            }
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 0)
            {
                UIState = 9;
                cursorY = 0;
            }

            if(cursorY == 2)
            compressVertices = !compressVertices;

            if(cursorY == 3)
            renderUnderwater = !renderUnderwater;

            if(cursorY == 4)
            {
                if(isOncCG50 == true)
                {
                    if(pixelSize == 1)
                    pixelSize = 2;
                    else
                    pixelSize = 1;

                    resXZBuffer = resX / pixelSize;
                    resYZBuffer = resY / pixelSize;
                }
            }

            if(cursorY == 5)
            renderTextured = !renderTextured;

            if(cursorY == 6)
            {
                if(isOncCG50 == true)
                lighting = !lighting;
            }

            if(cursorY == 7)
            stopChunkLoading = !stopChunkLoading;

            if(cursorY == 8)
            {
                renderingMode++;
                if(renderingMode > 2)
                renderingMode = 0;
            }

			if(cursorY == 10)
            beautifulSky = !beautifulSky;

            if(cursorY == 11)
            {
                UIState = 19;
                UIstateOld = 12;
                cursorY = 0;
            }

			saveSettings();
        }

        for (int y = 0; y < 216; y++)
        {
            for (int x = 0; x < 384; x++)
            {
                int xOnTexture = x%16;
                int yOnTexture = y%16;
                VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
            }
        }

        drawMinecraftFont(0, 384, 10, true, 8, "settings", 0xdefb, 0x39c7);

        for (int i = 0; i < Buttonlength; i++)
        {
            int extra = 0;
            char buttonText[50];
            sprintf(buttonText, "%s", text[i]);

            if(i != 0 && i != 11)
            {
                char temp[20];
                if(i == 1)
                {
                    sprintf(temp, "%d", renderDistance);
                    extra = countDigits(renderDistance);
                }
                if(i == 2)
                {
                    if(compressVertices == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 3)
                {
                    if(renderUnderwater == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 4)
                {
                    if(pixelSize == 2){
                        sprintf(temp, "Half");
                        extra = 4;
                    }
                    else{
                        sprintf(temp, "Full");
                        extra = 4;
                    }
                }
                if(i == 5)
                {
                    if(renderTextured == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 6)
                {
                    if(lighting == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 7)
                {
                    if(stopChunkLoading == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 8)
                {
                    if(renderingMode == 0){
                        sprintf(temp, "Rasterization");
                        extra = 13;
                    }
                    if(renderingMode == 1){
                        sprintf(temp, "Wireframe");
                        extra = 9;
                    }
                    if(renderingMode == 2){
                        sprintf(temp, "Both");
                        extra = 4;
                    }
                }
				if(i == 9)
				{
					if(redstoneRenderDistance > 0)
					{
						sprintf(temp, "%d", redstoneRenderDistance-1);
                    	extra = countDigits(redstoneRenderDistance-1);
					}
					else
					{
						sprintf(temp, "OFF");
                    	extra = 3;
					}
				}
				if(i == 10)
                {
                    if(beautifulSky== true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }

                strcat(buttonText, temp);
            }

            int startX = (textWidth[i].y - textWidth[i].x - 160) / 2 + textWidth[i].x;
            int endX = startX + 160;
            if(cursorY != i)
            {
                CopySprite(buttons[0], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 13) //game menu
    {
        int Buttonlength = 5;
        char text[5][50] = 
        {
            "Back to game", //0
            "Options...", //1
            "Save game", //2
            "Save and quit to title", //3
            "Quit without saving" //4
        };
        int textLength[5] = {12, 10, 9, 22, 19};
        int textHeight[5] = {30, 60, 90, 120, 150};
        Vector2I textWidth[5] = {{0, 383}, {0, 383}, {0, 383}, {0, 383}, {0, 383}};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = 0;
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 0)
            {
                UIState = 0;
                cursorY = 0;
            }

            if(cursorY == 1)
            {
                UIState = 14;
                cursorY = 0;
            }
            
            if(cursorY == 2)
            {   
                saveAllChunk(currentSelectedWorld, worldExists[currentSelectedWorld-1]);
                worldExists[currentSelectedWorld-1] = 1;
                worldVersion[currentSelectedWorld-1] = currentVersion;

                savegameData();
            }
            
            if(cursorY == 3)
            {
                saveAllChunk(currentSelectedWorld, worldExists[currentSelectedWorld-1]);
                worldExists[currentSelectedWorld-1] = 1;
                worldVersion[currentSelectedWorld-1] = currentVersion;

                savegameData();

                clearAllChunks();
				clearAllEntitys();
                UIState = 9;
                cursorY = 0;

                SEED = 0;
                for (int yC = 0; yC < totalChunkWidth; yC++)
                {
                    for (int xC = 0; xC < totalChunkWidth; xC++)
                    {
                        int chunkIndex = yC * totalChunkWidth + xC;
                        for (int x = 0; x < width; x++)
                        {
                            for (int z = 0; z < width; z++)
                            {
                                for (int y = 0; y < height; y++)
                                blocks[chunkIndex][x + z * width + y * width * width] = 0;
                            }
                        }
                        blocksRenderd[chunkIndex] = false;
                    }
                }
                dayTime = 1200;
                dayTimeChange = 0;
                skyBrightness = 8;

                for (int i = 0; i < 16; i++)
                screenColor[i] = skyColors[skyBrightness-1];

                int currentSlot = 0;
                health = 20;
                startFallY = 0;
                for (int i = 0; i < 9; i++)
                {
                    hotbarBlockTypes[i] =  -1;
                    hotbarBlockAmount[i] = 0;
                }

                for (int i = 0; i < 27; i++)
                {
                    inventoryBlockTypes[i] = -1;
                    inventoryBlockAmount[i] = 0;
					inventoryBlockHp[i] = 0;
                }
            }
            
            if(cursorY == 4)
            {
                clearAllChunks();
				clearAllEntitys();
                UIState = 9;
                cursorY = 0;

                SEED = 0;
                for (int yC = 0; yC < totalChunkWidth; yC++)
                {
                    for (int xC = 0; xC < totalChunkWidth; xC++)
                    {
                        int chunkIndex = yC * totalChunkWidth + xC;
                        for (int x = 0; x < width; x++)
                        {
                            for (int z = 0; z < width; z++)
                            {
                                for (int y = 0; y < height; y++)
                                blocks[chunkIndex][x + z * width + y * width * width] = 0;
                            }
                        }
                        blocksRenderd[chunkIndex] = false;
                    }
                }
                dayTime = 1200;
                dayTimeChange = 0;
                skyBrightness = 8;

                health = 20;
                startFallY = 0;

                for (int i = 0; i < 16; i++)
                screenColor[i] = skyColors[skyBrightness-1];

                int currentSlot = 0;
                for (int i = 0; i < 9; i++)
                {
                    hotbarBlockTypes[i] =  -1;
                    hotbarBlockAmount[i] = 0;
                }

                for (int i = 0; i < 27; i++)
                {
                    inventoryBlockTypes[i] = -1;
                    inventoryBlockAmount[i] = 0;
					inventoryBlockHp[i] = 0;
                }
            }
        }

        renderMenuEffect();

        drawMinecraftFont(0, 384, 10, true, 9, "Game menu", 0xdefb, 0x39c7);

        for (int i = 0; i < Buttonlength; i++)
        {
            int startX = (textWidth[i].y - textWidth[i].x - 160) / 2 + textWidth[i].x;
            int endX = startX + 160;
            if(cursorY != i)
            {
                CopySprite(buttons[0], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i], text[i], 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i], text[i], 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 14) //settings in game
    {
        int Buttonlength = 12;
        char text[12][50] = 
        {
            "back", //0
            "Render distance: ", //1
            "Compress vertices: ", //2
            "Render underwater: ", //3
            "Render resolution: ", //4
            "Render textured: ", //5
            "Lighting: ", //6
            "Stop chunk loading: ", //7
            "Render mode: ", //8
			"Chunk update distance: ", //9
			"Beautiful Sky: ", //10
            "Texture packs", //11
        };
        int textLength[12] = {4, 17, 19, 19, 19, 17, 10, 20, 13, 23, 15, 13};
        int textHeight[12] = {30, 55, 55, 80, 80, 105, 105, 130, 130, 155, 155, 180};
        Vector2I textWidth[12] = {{0, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 191}, {192, 383}, {0, 383}};

        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = Buttonlength-1;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = 13;
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != Buttonlength-1)
            cursorY++;
            else
            cursorY = 0;
        }
        if(cursorY == 1)
        {
            if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
            {
                if(renderDistance != 8)
                renderDistance += 1;

				saveSettings();
            }
            if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
            {
                if(renderDistance != 0)
                renderDistance -= 1;

				saveSettings();
            }
        }
		if(cursorY == 9)
        {
            if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
            {
                if(redstoneRenderDistance != 4)
                redstoneRenderDistance += 1;

				saveSettings();
            }
            if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
            {
                if(redstoneRenderDistance != 0)
                redstoneRenderDistance -= 1;

				saveSettings();
            }
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 0)
            {
                UIState = 13;
                cursorY = 0;
            }

            if(cursorY == 2)
            {
                compressVertices = !compressVertices;
                ResetAllChunks();
            }

            if(cursorY == 3)
            renderUnderwater = !renderUnderwater;

            if(cursorY == 4)
            {
                if(isOncCG50 == true)
                {
                    if(pixelSize == 1)
                    pixelSize = 2;
                    else
                    pixelSize = 1;

                    resXZBuffer = resX / pixelSize;
                    resYZBuffer = resY / pixelSize;
                }
            }

            if(cursorY == 5)
            {
                renderTextured = !renderTextured;
                ResetAllChunks();
            }

            if(cursorY == 6)
            {
                if(isOncCG50 == true)
                {
                    lighting = !lighting;

                    if(lighting == true)
                    loadLighting2();
                    else
                    resetLightMapAll();

                    ResetAllChunks();
                }
            }

            if(cursorY == 7)
            stopChunkLoading = !stopChunkLoading;

            if(cursorY == 8)
            {
                renderingMode++;
                if(renderingMode > 2)
                renderingMode = 0;
            }

			if(cursorY == 10)
            beautifulSky = !beautifulSky;

            if(cursorY == 11)
            {
                UIState = 19;
                UIstateOld = 14;
                cursorY = 0;
            }

			saveSettings();
        }

        renderMenuEffect();

        drawMinecraftFont(0, 384, 10, true, 8, "settings", 0xdefb, 0x39c7);

        for (int i = 0; i < Buttonlength; i++)
        {
            int extra = 0;
            char buttonText[50];
            sprintf(buttonText, "%s", text[i]);

            if(i > 0)
            {
                char temp[20];
                if(i == 1)
                {
                    sprintf(temp, "%d", renderDistance);
                    extra = countDigits(renderDistance);
                }
                if(i == 2)
                {
                    if(compressVertices == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 3)
                {
                    if(renderUnderwater == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 4)
                {
                    if(pixelSize == 2){
                        sprintf(temp, "Half");
                        extra = 4;
                    }
                    else{
                        sprintf(temp, "Full");
                        extra = 4;
                    }
                }
                if(i == 5)
                {
                    if(renderTextured == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 6)
                {
                    if(lighting == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 7)
                {
                    if(stopChunkLoading == true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }
                if(i == 8)
                {
                    if(renderingMode == 0){
                        sprintf(temp, "Rasterization");
                        extra = 13;
                    }
                    if(renderingMode == 1){
                        sprintf(temp, "Wireframe");
                        extra = 9;
                    }
                    if(renderingMode == 2){
                        sprintf(temp, "Both");
                        extra = 4;
                    }
                }
				if(i == 9)
				{
					if(redstoneRenderDistance > 0)
					{
						sprintf(temp, "%d", redstoneRenderDistance-1);
                    	extra = countDigits(redstoneRenderDistance-1);
					}
					else
					{
						sprintf(temp, "OFF");
                    	extra = 3;
					}
				}

				if(i == 10)
                {
                    if(beautifulSky== true){
                        sprintf(temp, "ON");
                        extra = 2;
                    }
                    else{
                        sprintf(temp, "OFF");
                        extra = 3;
                    }
                }

                strcat(buttonText, temp);
            }

            int startX = (textWidth[i].y - textWidth[i].x - 160) / 2 + textWidth[i].x;
            int endX = startX + 160;
            if(cursorY != i)
            {
                CopySprite(buttons[0], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xdefb, 0x39c7);
            }
            else
            {
                CopySprite(buttons[1], startX, textHeight[i], 160, 20, 0x001);
                drawMinecraftFont(startX, endX, textHeight[i]+6, true, textLength[i]+extra, buttonText, 0xfff3, 0x4205);
            }
        }
    }
    if(UIState == 16) //inventory
    {
        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorX >= 0)
            {
                cursorY--;
                if(cursorY == -1)
                cursorY = 3;
            }
            else
            {
                cursorY--;
                if(cursorY == -1)
                {
                    cursorYExtra--;
                    cursorY = 0;
                }
                if(cursorYExtra == -1)
                {
                    cursorY = 3;
                    cursorYExtra = craftingRecipeAmount/3-3;
                }
            }
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorX >= 0)
            {
                cursorY++;
                if(cursorY == 4)
                cursorY = 0;
            }
            else
            {
                cursorY++;
                if(cursorY == 4)
                {
                    cursorYExtra++;
                    cursorY = 3;
                }
                if(cursorYExtra > craftingRecipeAmount/3-3)
                {
                    cursorYExtra = 0;
                    cursorY = 0;
                }
            }
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            isInCraftingTable = false;
            UIState = 0;
            cursorStage = 0;
            cursorX = 0;
            cursorY = 0;
            cursorXSelected = 0;
            cursorYSelected = 0;
        }
        if(keydownlast(KEY_VARS) && !keydownhold(KEY_VARS))
        {
            isInCraftingTable = false;
            UIState = 0;
            cursorStage = 0;
            cursorX = 0;
            cursorY = 0;
            cursorXSelected = 0;
            cursorYSelected = 0;
        }
        if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
        {
            cursorX++;
            if(cursorX == 9)
            cursorX = -2;

            if(cursorX >= 0)
            cursorYExtra = 0;
        }
        if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
        {
            cursorX--;
            if(cursorX == -4)
            cursorX = 8;

            if(cursorX >= 0)
            cursorYExtra = 0;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorX >= 0)
            {
                if(cursorStage == 0)
                {
                    cursorXSelected = cursorX;
                    cursorYSelected = cursorY;

                    cursorStage = 1;
                }
                else
                {
                    swapItems(cursorXSelected, cursorYSelected, cursorX, cursorY);
                    cursorStage = 0;

                    if(hotbarBlockTypes[currentSlot] != -1)
                    blockSelected = hotbarBlockTypes[currentSlot];
                    else
                    blockSelected = -1;
                }
            }
            else
            {
                int craftIndex = (cursorY + cursorYExtra) * 3 + (cursorX + 3);
                int totRecourcesNeeded = craftingRecipes[craftIndex].diffrentItemAmount; 
                int continueCraft = 1;

                //addItemToHotbarInventory(craftingRecipes[craftIndex].outputItem+1, craftingRecipes[craftIndex].outputItemAmount);

                if(craftingRecipes[craftIndex].needsCraftingTable == false || isInCraftingTable == true)
                {
                    for (int i = 0; i < totRecourcesNeeded; i++)
                    {
                        if(returnTotalItemAmount(craftingRecipes[craftIndex].inputItem[i]) < craftingRecipes[craftIndex].inputItemAmount[i])
                        continueCraft = 0;
                    }

                    if(continueCraft == 1)
                    {
                        for (int i = 0; i < totRecourcesNeeded; i++)
                        removeItemFromInventoryHotbar(craftingRecipes[craftIndex].inputItem[i], craftingRecipes[craftIndex].inputItemAmount[i]);

                        addItemToHotbarInventory(craftingRecipes[craftIndex].outputItem, craftingRecipes[craftIndex].outputItemAmount);
                    }
                }
            }

            if(hotbarBlockTypes[currentSlot] != -1)
            blockSelected = hotbarBlockTypes[currentSlot];
            else
            blockSelected = -1;
        }

        renderMenuEffect();

        CopySpriteNbitMasked(inventory, 108, 42, 268, 132, inventory_palette, 0x0001, 4);  //crafting menu will be 100 pixels width
        CopySprite(recipeSelector, 8, 42, 100, 128, 0x0001);  //crafting menu will be 100 pixels width

        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 9; x++)
            {
                int xOnScreen = 116 + x*28;
                int yOnScreen = 50  + y*28;

                if(cursorX >= 0)
                {
                    if(x == cursorX && y == cursorY)
                    {
                        for (int y_ = 0; y_ < 26; y_++)
                        {
                            for (int x_ = 0; x_ < 26; x_++)
                            VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                        }
                    }
                }

                //inventoryBlockTypes[inventoryIndex], inventoryBlockAmount[inventoryIndex]
                int inventoryIndex = x + y * 9;
                int itemType = inventoryBlockTypes[inventoryIndex];
                if(itemType > itemAmount)
                itemType = 0;
                int itemAmount_ = inventoryBlockAmount[inventoryIndex];
				int itemHp = inventoryBlockHp[inventoryIndex];
                
                if(itemType != -1)
                {
                    CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                    if(itemAmount_ != 1)
                    {
                        char buffer[4];
                        sprintf(buffer, "%d", itemAmount_);
                        drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                    }

					if(itemHp != allItem[itemType].breaksIn && itemHp >= 0 && allItem[itemType].breaksIn != 0)
					drawHp(xOnScreen+3, yOnScreen+24, 2, 22, itemHp, allItem[itemType].breaksIn);
                }
            }
        }
        for (int x = 0; x < 9; x++)
        {
            int xOnScreen = 116 + x*28;
            int yOnScreen = 138;

            if(cursorX == x && cursorY == 3)
            {
                for (int y_ = 0; y_ < 26; y_++)
                {
                    for (int x_ = 0; x_ < 26; x_++)
                    VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                }
            }
            
            int itemType = hotbarBlockTypes[x];
            if(itemType > itemAmount)
            itemType = 0;
            int itemAmount_ = hotbarBlockAmount[x];
			int itemHp = hotbarBlockHp[x];
            
            if(itemType != -1)
            {
                CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                if(itemAmount_ != 1)
                {
                    char buffer[4];
                    sprintf(buffer, "%d", itemAmount_);
                    drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                }

				if(itemHp != allItem[itemType].breaksIn && itemHp >= 0 && allItem[itemType].breaksIn != 0)
				drawHp(xOnScreen+3, yOnScreen+24, 2, 22, itemHp, allItem[itemType].breaksIn);
            }
        }

        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 3; x++)
            {
                int xOnScreen = 16 + x*28;
                int yOnScreen = 50 + y*28;

                if(cursorX < 0)
                {
                    int cursorXTemp = cursorX+3;
                    if(x == cursorXTemp && y == cursorY)
                    {
                        for (int y_ = 0; y_ < 26; y_++)
                        {
                            for (int x_ = 0; x_ < 26; x_++)
                            VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                        }
                    }
                }

                //inventoryBlockTypes[inventoryIndex], inventoryBlockAmount[inventoryIndex]
                int inventoryIndex = x + (y+cursorYExtra) * 3;
                if(inventoryIndex < craftingRecipeAmount)
                {
                    int itemType = craftingRecipes[inventoryIndex].outputItem;
                    int itemAmount_ = craftingRecipes[inventoryIndex].outputItemAmount;

                    if(itemType != -1)
                    {
                        CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                        if(itemAmount_ != 1)
                        {
                            char buffer[4];
                            sprintf(buffer, "%d", itemAmount_);
                            drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                        }

                        if(isInCraftingTable == false && craftingRecipes[inventoryIndex].needsCraftingTable == true)
                        CopySprite(noCraftingTableCross, xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);
                    }
                }
            }
        }

        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 3; x++)
            {
                if(cursorX < 0)
                {
                    int cursorXTemp = cursorX+3;
                    if(x == cursorXTemp && y == cursorY)
                    {
                        int xOnScreen = 16 + x*28;
                        int yOnScreen = 50 + y*28;

                        int itemType = (cursorY + cursorYExtra) * 3 + (cursorX + 3);

                        if(itemType < craftingRecipeAmount)
                        itemDetails(xOnScreen+24, yOnScreen+4, allItem[craftingRecipes[itemType].outputItem].name);
                    }
                }
            }
        }
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 9; x++)
            {
                int xOnScreen = 116 + x*28;
                int yOnScreen = 50  + y*28;

                if(cursorX >= 0)
                {
                    if(x == cursorX && y == cursorY)
                    {
                        int inventoryIndex = x + y * 9;
                        int itemType = inventoryBlockTypes[inventoryIndex];
                        if(itemType > itemAmount)
                        itemType = 0;

                        if(itemType != -1)
                        itemDetails(xOnScreen+24, yOnScreen+4, allItem[itemType].name);
                    }
                }
            }
        }
        for (int x = 0; x < 9; x++)
        {
            int xOnScreen = 116 + x*28;
            int yOnScreen = 138;
            
            if(cursorX == x && cursorY == 3)
            {
                int itemType = hotbarBlockTypes[x];
                if(itemType > itemAmount)
                itemType = 0;
                
                if(itemType != -1)
                itemDetails(xOnScreen+24, yOnScreen+4, allItem[itemType].name);
            }
        }
    }
    if(UIState == 17) //creative menu
    {
        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorStage == 0)
            {
                cursorY--;
                if(cursorY == -1)
                {
                    cursorYExtra--;
                    cursorY = 0;
                }
                if(cursorYExtra == -1)
                {
                    cursorY = 2;
                    cursorYExtra = itemAmount/9-2;
                }
            }
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorStage == 0)
            {
                cursorY++;
                if(cursorY == 3)
                {
                    cursorYExtra++;
                    cursorY = 2;
                }
                if(cursorYExtra > itemAmount/9-2)
                {
                    cursorYExtra = 0;
                    cursorY = 0;
                }
            }
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            isInCraftingTable = false;
            UIState = 0;
            cursorStage = 0;
            cursorX = 0;
            cursorY = 0;
            cursorXSelected = 0;
            cursorYSelected = 0;
        }
        if(keydownlast(KEY_VARS) && !keydownhold(KEY_VARS))
        {
            isInCraftingTable = false;
            UIState = 0;
            cursorStage = 0;
            cursorX = 0;
            cursorY = 0;
            cursorXSelected = 0;
            cursorYSelected = 0;
        }
        if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
        {
            cursorX++;
            if(cursorX == 9)
            cursorX = 0;
        }
        if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
        {
            cursorX--;
            if(cursorX == -1)
            cursorX = 8;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorX >= 0)
            {
                if(cursorStage == 0)
                {
                    cursorTypeSelected = cursorX + (cursorY+cursorYExtra) * 9 + 1;

					if(cursorTypeSelected < itemAmount)
                    cursorStage = 1;
                }
                else
                {
                    hotbarBlockTypes[cursorX] = cursorTypeSelected;
                    hotbarBlockAmount[cursorX] = allItem[cursorTypeSelected].stackSize;
					hotbarBlockHp[cursorX] = allItem[cursorTypeSelected].breaksIn;

                    cursorStage = 0;

                    if(hotbarBlockTypes[currentSlot] != -1)
                    blockSelected = hotbarBlockTypes[currentSlot];
                    else
                    blockSelected = -1;
                }
            }
        }

        renderMenuEffect();

        //CopySprite(inventory, 58, 42, 268, 132, 0x0001);  //crafting menu will be 100 pixels width
        CopySpriteNbitMasked(inventory, 58, 42, 268, 132, inventory_palette, 0x0001, 4);  //crafting menu will be 100 pixels width

        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 9; x++)
            {
                int xOnScreen = 66 + x*28;
                int yOnScreen = 50  + y*28;

                if(cursorStage == 0)
                {
                    if(x == cursorX && y == cursorY)
                    {
                        for (int y_ = 0; y_ < 26; y_++)
                        {
                            for (int x_ = 0; x_ < 26; x_++)
                            VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                        }
                    }
                }
                
                int itemType = x + (y+cursorYExtra) * 9 + 1;
                
                if(itemType < itemAmount)
                CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);
            }
        }
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 9; x++)
            {
                int xOnScreen = 66 + x*28;
                int yOnScreen = 50  + y*28;

                if(cursorStage == 0)
                {
                    if(x == cursorX && y == cursorY)
                    {
                        int itemType = x + (y+cursorYExtra) * 9 + 1;
                
                        if(itemType < itemAmount)
                        itemDetails(xOnScreen+24, yOnScreen+4, allItem[itemType].name);
                    }
                }
            }
        }
        for (int x = 0; x < 9; x++)
        {
            int xOnScreen = 66 + x*28;
            int yOnScreen = 138;

            if(cursorStage == 1)
            {
                if(cursorX == x)
                {
                    for (int y_ = 0; y_ < 26; y_++)
                    {
                        for (int x_ = 0; x_ < 26; x_++)
                        VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                    }
                }
            }
            
            int itemType = hotbarBlockTypes[x];
            if(itemType > itemAmount)
            itemType = 0;
            int itemAmount_ = hotbarBlockAmount[x];
            
            if(itemType != -1)
            {
                CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                if(itemAmount_ != 1)
                {
                    char buffer[4];
                    sprintf(buffer, "%d", itemAmount_);
                    drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                }
            }
        }
    }
    if(UIState == 18) //inventory chest
    {
        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            cursorY--;
            if(cursorY == -1)
            cursorY = 6;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            cursorY++;
            if(cursorY == 7)
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            isInCraftingTable = false;
            UIState = 0;
            cursorStage = 0;
            cursorX = 0;
            cursorY = 0;
            cursorXSelected = 0;
            cursorYSelected = 0;
        }
        if(keydownlast(KEY_VARS) && !keydownhold(KEY_VARS))
        {
            isInCraftingTable = false;
            UIState = 0;
            cursorStage = 0;
            cursorX = 0;
            cursorY = 0;
            cursorXSelected = 0;
            cursorYSelected = 0;
        }
        if(keydownlast(KEY_PRGM_RIGHT) && !keydownhold(KEY_PRGM_RIGHT))
        {
            cursorX++;
            if(cursorX == 9)
            cursorX = 0;
        }
        if(keydownlast(KEY_PRGM_LEFT) && !keydownhold(KEY_PRGM_LEFT))
        {
            cursorX--;
            if(cursorX == -1)
            cursorX = 8;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorStage == 0)
            {
                cursorXSelected = cursorX;
                cursorYSelected = cursorY;

                cursorStage = 1;
            }
            else
            {
                swapItemsChest(cursorXSelected, cursorYSelected, cursorX, cursorY);
                cursorStage = 0;

                if(hotbarBlockTypes[currentSlot] != -1)
                blockSelected = hotbarBlockTypes[currentSlot];
                else
                blockSelected = -1;
            }

            if(hotbarBlockTypes[currentSlot] != -1)
            blockSelected = hotbarBlockTypes[currentSlot];
            else
            blockSelected = -1;
        }

        renderMenuEffect();

        CopySpriteNbitMasked(chestInv, 58, 0, 268, 216, chestInv_palette, 0x0001, 4);  //crafting menu will be 100 pixels width

        int chestIndex = -1;
        for (int i = 0; i < chestAmount; i++)
        {
            if(allchest[i].used == true)
            {
                if(removeBlock.x == allchest[i].position.x && removeBlock.y == allchest[i].position.y && removeBlock.z == allchest[i].position.z)
                {
                    chestIndex = i;
                    break;
                }
            }
        }

        if(chestIndex == -1) //chest does not exist exit
        UIState = 0;
        else
        {
            currentChestIndex = chestIndex;
            for (int y = 0; y < 3; y++) //chest
            {
                for (int x = 0; x < 9; x++)
                {
                    int xOnScreen = 66 + x*28;
                    int yOnScreen = 7  + y*28;

                    if(cursorY >= 0 && cursorY < 3)
                    {
                        if(x == cursorX && y == cursorY)
                        {
                            for (int y_ = 0; y_ < 26; y_++)
                            {
                                for (int x_ = 0; x_ < 26; x_++)
                                VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                            }
                        }
                    }

                    //inventoryBlockTypes[inventoryIndex], inventoryBlockAmount[inventoryIndex]
                    int inventoryIndex = x + y * 9;
                    int itemType = allchest[chestIndex].chestBlockTypes[inventoryIndex];
                    if(itemType > itemAmount)
                    itemType = 0;
                    int itemAmount_ = allchest[chestIndex].chestBlockAmount[inventoryIndex];
					int itemHp = allchest[chestIndex].chestBlockHp[inventoryIndex];
                    
                    if(itemType != -1)
                    {
                        CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                        if(itemAmount_ != 1)
                        {
                            char buffer[4];
                            sprintf(buffer, "%d", itemAmount_);
                            drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                        }

						if(itemHp != allItem[itemType].breaksIn && itemHp >= 0 && allItem[itemType].breaksIn != 0)
						drawHp(xOnScreen+3, yOnScreen+24, 2, 22, itemHp, allItem[itemType].breaksIn);
                    }
                }
            }

            for (int y = 0; y < 3; y++) //inventory
            {
                for (int x = 0; x < 9; x++)
                {
                    int xOnScreen = 66 + x*28;
                    int yOnScreen = 93  + y*28;

                    if(cursorY >= 3 && cursorY < 6)
                    {
                        if(x == cursorX && (y + 3) == cursorY)
                        {
                            for (int y_ = 0; y_ < 26; y_++)
                            {
                                for (int x_ = 0; x_ < 26; x_++)
                                VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                            }
                        }
                    }

                    //inventoryBlockTypes[inventoryIndex], inventoryBlockAmount[inventoryIndex]
                    int inventoryIndex = x + y * 9;
                    int itemType = inventoryBlockTypes[inventoryIndex];
                    if(itemType > itemAmount)
                    itemType = 0;
                    int itemAmount_ = inventoryBlockAmount[inventoryIndex];
					int itemHp = inventoryBlockHp[inventoryIndex];

                    if(itemType != -1)
                    {
                        CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                        if(itemAmount_ != 1)
                        {
                            char buffer[4];
                            sprintf(buffer, "%d", itemAmount_);
                            drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                        }

						if(itemHp != allItem[itemType].breaksIn && itemHp >= 0 && allItem[itemType].breaksIn != 0)
						drawHp(xOnScreen+3, yOnScreen+24, 2, 22, itemHp, allItem[itemType].breaksIn);
                    }
                }
            }
            for (int x = 0; x < 9; x++) //hotbar
            {
                int xOnScreen = 66 + x*28;
                int yOnScreen = 181;

                if(cursorX == x && cursorY == 6)
                {
                    for (int y_ = 0; y_ < 26; y_++)
                    {
                        for (int x_ = 0; x_ < 26; x_++)
                        VRAMAddress[xOnScreen+x_+1 + (yOnScreen+y_+1) * 384] = 0xc638;
                    }
                }

                int itemType = hotbarBlockTypes[x];
                if(itemType > itemAmount)
                itemType = 0;
                int itemAmount_ = hotbarBlockAmount[x];
				int itemHp = hotbarBlockHp[x];

                if(itemType != -1)
                {
                    CopySpriteIcon(&itemIcons2[676 * allItem[itemType].iconIndex], xOnScreen+1, yOnScreen+1, 26, 26, 0x0000);

                    if(itemAmount_ != 1)
                    {
                        char buffer[4];
                        sprintf(buffer, "%d", itemAmount_);
                        drawMinecraftFont(xOnScreen+16, xOnScreen+16, yOnScreen+15, false, countDigits(itemAmount_), buffer, 0xdefb, 0x39c7);
                    }

					if(itemHp != allItem[itemType].breaksIn && itemHp >= 0 && allItem[itemType].breaksIn != 0)
					drawHp(xOnScreen+3, yOnScreen+24, 2, 22, itemHp, allItem[itemType].breaksIn);
                }
            }

            for (int y = 0; y < 3; y++) //chest
            {
                for (int x = 0; x < 9; x++)
                {
                    int xOnScreen = 66 + x*28;
                    int yOnScreen = 7 + y*28;

                    if(cursorY >= 0 && cursorY < 3)
                    {
                        if(x == cursorX && y == cursorY)
                        {
                            int inventoryIndex = x + y * 9;
                            int itemType = allchest[chestIndex].chestBlockTypes[inventoryIndex];
                            if(itemType > itemAmount)
                            itemType = 0;

                            if(itemType != -1)
                            itemDetails(xOnScreen+24, yOnScreen+4, allItem[itemType].name);
                        }
                    }
                }
            }
            for (int y = 0; y < 3; y++) //inventory
            {
                for (int x = 0; x < 9; x++)
                {
                    int xOnScreen = 66 + x*28;
                    int yOnScreen = 93 + y*28;

                    if(cursorY >= 3 && cursorY < 6)
                    {
                        if(x == cursorX && (y+3) == cursorY)
                        {
                            int inventoryIndex = x + y * 9;
                            int itemType = inventoryBlockTypes[inventoryIndex];
                            if(itemType > itemAmount)
                            itemType = 0;

                            if(itemType != -1)
                            itemDetails(xOnScreen+24, yOnScreen+4, allItem[itemType].name);
                        }
                    }
                }
            }
            for (int x = 0; x < 9; x++) //hotbar 
            {
                int xOnScreen = 66  + x*28;
                int yOnScreen = 181;

                if(cursorX == x && cursorY == 6)
                {
                    int itemType = hotbarBlockTypes[x];
                    if(itemType > itemAmount)
                    itemType = 0;

                    if(itemType != -1)
                    itemDetails(xOnScreen+24, yOnScreen+4, allItem[itemType].name);
                }
            }
        }
    }
    if(UIState == 19) //texture pack selector
    {
        keyupdate();
        if(keydownlast(KEY_PRGM_UP) && !keydownhold(KEY_PRGM_UP))
        {
            if(cursorY != 0)
            cursorY--;
            else
            cursorY = 5;
        }
        if(keydownlast(KEY_PRGM_DOWN) && !keydownhold(KEY_PRGM_DOWN))
        {
            if(cursorY != 5)
            cursorY++;
            else
            cursorY = 0;
        }
        if(keydownlast(KEY_PRGM_MENU) && !keydownhold(KEY_PRGM_MENU))
        {
            UIState = UIstateOld;
            cursorY = 0;
        }
        if(keydownlast(KEY_EXE) && !keydownhold(KEY_EXE))
        {
            if(cursorY == 5)
            {
                UIState = UIstateOld;
                cursorY = 0;
            }
            else
            {
                if(cursorY < maxTextureIndex && currentTexturePackIndex != cursorY)
                {
                    currentTexturePackIndex = cursorY;
                    loadTextureAssets();
                    loadIconAssets();

                    saveSettings();
                }
            }
        }

        for (int y = 0; y < 216; y++)
        {
            for (int x = 0; x < 384; x++)
            {
                int xOnTexture = x%16;
                int yOnTexture = y%16;
                VRAMAddress[y*384+x] = textures2[xOnTexture+yOnTexture*16 + 3*256];
            }
        }

        for (int y = 5; y < 185; y++)
        {
            for (int x = 128; x < 256; x++)
            VRAMAddress[y*384+x] = combineColors(VRAMAddress[y*384+x], 0x0000, 50);
        }

        for (int i = 0; i < 6; i++)
        {
            if(i == 5)
            {
                if(cursorY != i)
                {
                    CopySprite(buttons[0], 112, 190, 160, 20, 0x001);
                    drawMinecraftFont(112, 272, 196, true, 4, "back", 0xdefb, 0x39c7);
                }
                else
                {
                    CopySprite(buttons[1], 112, 190, 160, 20, 0x001);
                    drawMinecraftFont(112, 272, 196, true, 4, "back", 0xfff3, 0x4205);
                }
            }
            else
            {
                if(i < maxTextureIndex)
                {
                    if(texturePackVersion[i] != currentVersion)
                    {
                        for (int y = 6+36*i; y < 40+36*i; y++)
                        {
                            for (int x = 129; x < 255; x++)
                            VRAMAddress[y*384+x] = 0x7000;
                        }
                    }

                    CopySprite(texturePackIcon[i], 130, 7+36*i, 32, 32, 0xffff);                                                                    //draw icon
                    drawMinecraftFont(164, 164, 7+36*i, false, getTextLength(texturePackName[i], 24), texturePackName[i], 0xdefb, 0x39c7);          //draw name
                    drawMinecraftFont(164, 164, 17+36*i, false, getTextLength(texturePackCreator[i], 24), texturePackCreator[i], 0xdefb, 0x39c7);   //draw creator

                    char versionStr[12] = "";
                    memset(versionStr, 0, 12);
                    sprintf(versionStr, "V%d", texturePackVersion[i]);
                    drawMinecraftFont(164, 164, 27+36*i, false, getTextLength(versionStr, 12), versionStr, 0xdefb, 0x39c7);                         //draw version

                    if(currentTexturePackIndex == i)
                    drawMinecraftFont(216, 216, 27+36*i, false, 6, "active", 0xdefb, 0x39c7);                                                       //draw active
                }

                if(cursorY == i)
                {
                    for (int y = 6+36*i; y < 40+36*i; y++)
                    {
                        for (int x = 129; x < 255; x++)
                        VRAMAddress[y*384+x] = combineColors(VRAMAddress[y*384+x], 0x0000, 25);
                    }
                }
            }
        }
    }

    if(keydownlast(KEY_PRGM_0) && !keydownhold(KEY_PRGM_0))
    {
        if(isOncCG50 == true)
        convertToBitmap16bit();
    }
    if(keydownlast(KEY_PLUS) && !keydownhold(KEY_PLUS))
    {
        UIState = 19;
    }
}
