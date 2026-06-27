/* src/save.c - World/chunk/chest/entity/player/settings save and load */
#include "engine.h"
#include "chunk_constants.h"


			if(blocks[i][j] == 6)
			blockData[i*width*width*height + j] = 0b10000001;

			if(blocks[i][j] == 24)
			blockData[i*width*width*height + j] = 0b10000001;
		}

		loadingScreen(i, totalChunkWidth*totalChunkWidth, "Convering old water...", 22);
	}
}

//file operations
void makeStartFolder()
{
    char dirLocation1[50] = "\\\\fls0\\fxcraft";

    unsigned short pDir1[sizeof(dirLocation1)*2]; // Make buffer
    Bfile_StrToName_ncpy(pDir1, (unsigned char*)dirLocation1, sizeof(dirLocation1)); 
    Bfile_CreateEntry_OS(pDir1, 5, NULL);
}
void saveWorldData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\worldData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 16;
    char worlData[size];

    for (int j = 0; j < 4; j++)
    worlData[j] = (SEED >> (j*8)) & 0xFF;
    
    worlData[4] = advancedTerrain;
    worlData[5] = survival;
    worlData[6] = caves;
    worlData[7] = dayNightCycle;
    worlData[8] = isinfinite;
    worlData[9] = mobs;

    for (int j = 0; j < 4; j++)
    worlData[10+j] = (dayTime >> (j*8)) & 0xFF;

    worlData[14] = dayTimeChange;
    worlData[15] = skyBrightness;

    Bfile_CreateEntry_OS(pFile, 1, &size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    Bfile_WriteFile_OS(hFile, worlData, size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "gameData failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadWorldData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\worldData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 16;
    char worlData[16] = {0};
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, worlData, length, 0); 
        Bfile_CloseFile_OS(hFile);

        SEED = 0;
        for (int j = 0; j < 4; j++)
        SEED |= ((worlData[j] & 0xff) << j*8);

        advancedTerrain = worlData[4];
        survival = worlData[5];
        caves = worlData[6];
        dayNightCycle = worlData[7];
        isinfinite = worlData[8];
        mobs = worlData[9];
        
        dayTime = 0;
        for (int j = 0; j < 4; j++)
        dayTime |= ((worlData[10+j] & 0xff) << j*8);

        dayTimeChange = worlData[14];
        skyBrightness = worlData[15];
    }
    else
    savegameData();
}
void saveChestData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 175*chestAmount; //used=1, position=12, type=54, amount=54
    char chestData_[size];

    for (int chestI = 0; chestI < chestAmount; chestI++)
    {
        int i = 175*chestI;

        chestData_[i] = allchest[chestI].used;

        for (int j = 0; j < 4; j++)
        chestData_[i+1+j] = (allchest[chestI].position.x >> (j*8)) & 0xFF;
        for (int j = 0; j < 4; j++)
        chestData_[i+5+j] = (allchest[chestI].position.y >> (j*8)) & 0xFF;
        for (int j = 0; j < 4; j++)
        chestData_[i+9+j] = (allchest[chestI].position.z >> (j*8)) & 0xFF;

        for (int k = 0; k < 27; k++)
        {
            for (int j = 0; j < 2; j++)
            chestData_[i+13+j+(k*2)] = (allchest[chestI].chestBlockTypes[k] >> (j*8)) & 0xFF;
        }
        for (int k = 0; k < 27; k++)
        {
            for (int j = 0; j < 2; j++)
            chestData_[i+67+j+(k*2)] = (allchest[chestI].chestBlockAmount[k] >> (j*8)) & 0xFF;
        }
        for (int k = 0; k < 27; k++)
        {
            for (int j = 0; j < 2; j++)
            chestData_[i+121+j+(k*2)] = (allchest[chestI].chestBlockHp[k] >> (j*8)) & 0xFF;
        }
    }

    Bfile_CreateEntry_OS(pFile, 1, &size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    Bfile_WriteFile_OS(hFile, chestData_, size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "chestData failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadChestData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        char chestData_[length];

        Bfile_ReadFile_OS(hFile, chestData_, length, 0); 
        Bfile_CloseFile_OS(hFile);

        for (int chestI = 0; chestI < chestAmount; chestI++)
        {
            int i = 175*chestI;

            allchest[chestI].position.x = 0;
            allchest[chestI].position.y = 0;
            allchest[chestI].position.z = 0;
            allchest[chestI].used = 0;

            allchest[chestI].used = chestData_[i];

            for (int j = 0; j < 4; j++)
            allchest[chestI].position.x |= ((chestData_[i+1+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            allchest[chestI].position.y |= ((chestData_[i+5+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            allchest[chestI].position.z |= ((chestData_[i+9+j] & 0xff) << j*8);

            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockTypes[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockTypes[k] |= ((chestData_[i+13+j+(k*2)] & 0xff) << j*8);
            }
            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockAmount[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockAmount[k] |= ((chestData_[i+67+j+(k*2)] & 0xff) << j*8);
            }
            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockHp[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockHp[k] |= ((chestData_[i+121+j+(k*2)] & 0xff) << j*8);
            }
        }
    }
    else
    saveChestData(world);
}
void loadChestDataV4(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        char chestData_[length];

        Bfile_ReadFile_OS(hFile, chestData_, length, 0); 
        Bfile_CloseFile_OS(hFile);

        for (int chestI = 0; chestI < chestAmount; chestI++)
        {
            int i = 121*chestI;

            allchest[chestI].position.x = 0;
            allchest[chestI].position.y = 0;
            allchest[chestI].position.z = 0;
            allchest[chestI].used = 0;

            allchest[chestI].used = chestData_[i];

            for (int j = 0; j < 4; j++)
            allchest[chestI].position.x |= ((chestData_[i+1+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            allchest[chestI].position.y |= ((chestData_[i+5+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            allchest[chestI].position.z |= ((chestData_[i+9+j] & 0xff) << j*8);

            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockTypes[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockTypes[k] |= ((chestData_[i+13+j+(k*2)] & 0xff) << j*8);
            }
            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockAmount[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockAmount[k] |= ((chestData_[i+67+j+(k*2)] & 0xff) << j*8);
            }
        }
    }
    else
    saveChestData(world);
}
void savegameData()
{
    char fileLocation[50] = "\\\\fls0\\fxcraft\\gameData";

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 25;
    char playerData[size];

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        playerData[i*4+j] = (worldVersion[i] >> (j*8)) & 0xFF;
    }
    
    for (int i = 0; i < 5; i++)
    playerData[20+i] = worldExists[i];

    Bfile_CreateEntry_OS(pFile, 1, &size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    Bfile_WriteFile_OS(hFile, playerData, size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "gameData failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadgameData()
{
    char fileLocation[50] = "\\\\fls0\\fxcraft\\gameData";

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 25;
    char playerData[25] = {0};
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, playerData, length, 0); 
        Bfile_CloseFile_OS(hFile);

        for (int i = 0; i < 5; i++)
        {
            worldVersion[i] = 0;
            for (int j = 0; j < 4; j++)
            worldVersion[i] |= ((playerData[i*4+j] & 0xff) << j*8);
        }
        for (int i = 0; i < 5; i++)
        worldExists[i] = playerData[20+i];
    }
    else
    savegameData();
}
void saveSettings()
{
    char fileLocation[50] = "\\\\fls0\\fxcraft\\settings";

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 64;
    char settingData[size];

    for (int i = 0; i < size; i++)
    settingData[i] = 0;

    settingData[0] = (char)renderDistance;
	settingData[1] = (char)compressVertices;
	settingData[2] = (char)renderUnderwater;
	settingData[3] = (char)pixelSize;
	settingData[4] = (char)renderTextured;
	settingData[5] = (char)lighting;
	settingData[6] = 0;
	settingData[7] = (char)stopChunkLoading;
	settingData[8] = (char)renderingMode;
	settingData[9] = (char)redstoneRenderDistance;
	settingData[10] = (char)beautifulSky;
    settingData[11] = (char)currentTexturePackIndex;

    Bfile_CreateEntry_OS(pFile, 1, &size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    Bfile_WriteFile_OS(hFile, settingData, size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "settings failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadSettings()
{
    char fileLocation[50] = "\\\\fls0\\fxcraft\\settings";

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    char settingData[64] = {0};
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

		if(length == 10) //if old settings update to new
		{
			Bfile_CloseFile_OS(hFile);
			
			loadSettingsO();
			Bfile_DeleteEntry(pFile);
			saveSettings();
		}
		else
		{
    		Bfile_ReadFile_OS(hFile, settingData, length, 0); 
    		Bfile_CloseFile_OS(hFile);
			
			renderDistance = 		    settingData[0];
			compressVertices = 		    settingData[1];
			renderUnderwater = 		    settingData[2];
			pixelSize = 			    settingData[3];
			renderTextured = 		    settingData[4];
			lighting = 				    settingData[5];
            //6=free
			stopChunkLoading = 		    settingData[7];
			renderingMode = 		    settingData[8];
			redstoneRenderDistance =    settingData[9];
			beautifulSky = 			    settingData[10];
            currentTexturePackIndex =   settingData[11];
		}
    }
    else
    saveSettings();
}
void loadSettingsO()
{
    char fileLocation[50] = "\\\\fls0\\fxcraft\\settings";

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 10;
    char settingData[10] = {0};
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, settingData, length, 0); 
        Bfile_CloseFile_OS(hFile);
		
		renderDistance = 		 settingData[0];
		compressVertices = 		 settingData[1];
		renderUnderwater = 		 settingData[2];
		pixelSize = 			 settingData[3];
		renderTextured = 		 settingData[4];
		lighting = 				 settingData[5];
		//6=free
		stopChunkLoading = 		 settingData[7];
		renderingMode = 		 settingData[8];
		redstoneRenderDistance = settingData[9];
    }
    else
    saveSettings();
}
void saveChunk(int index, int world, bool exists)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chunk%d", world, index);

    //getting entry for chunkFile
    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    //make file
    if(exists == false)
    Bfile_CreateEntry_OS(pFile, 1, sizeof(blocks[index]));

    //writing data to file 
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    Bfile_WriteFile_OS(hFile, blocks[index], sizeof(blocks[index])); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "Error: chunk not found: ";
        char buffer2[10];
        sprintf(buffer2, "%d", index);
        strcat(buffer1, buffer2);
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadChunk(int index, int world)
{
    char worldString[5];
    sprintf(worldString, "%d", world);

    char fileLocation[50] = "\\\\fls0\\fxcraft\\world";
    char dirLocation[50] = "\\\\fls0\\fxcraft\\world";

    strcat(dirLocation, worldString);
    strcat(fileLocation, worldString);

    strcat(fileLocation, "\\chunk");

    char numberBuffer[7];
    sprintf(numberBuffer, "%d", index);
    strcat(fileLocation, numberBuffer);

    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    int length = Bfile_GetFileSize_OS(hFile);

    Bfile_ReadFile_OS(hFile, blocks[index], length, 0); 
    Bfile_CloseFile_OS(hFile);

    //for (int x = 0; x < width; x++)
    //{
    //    for (int z = 0; z < width; z++)
    //    {
    //        for (int y = 0; y < height; y++)
    //        {
    //            if(y == 0)
    //            blocks[index][x+z*width+y*width*width] = 9;
    //            else if(y < 8)
    //            blocks[index][x+z*width+y*width*width] = 3;
    //        }
    //    }
    //}
    

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "Error: chunk not found: ";
        char buffer2[10];
        sprintf(buffer2, "%d", index);
        strcat(buffer1, buffer2);
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void saveChunkExtraData(int index, int world, bool exists)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chunkED%d", world, index);

    //getting entry for chunkFile
    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int sizeOfFile = width*width*height;

    //make file
    if(exists == false)
    Bfile_CreateEntry_OS(pFile, 1, &sizeOfFile);

    //writing data to file 
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    Bfile_WriteFile_OS(hFile, &blockData[index*width*width*height], sizeOfFile); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "Error: chunk not found: ";
        char buffer2[10];
        sprintf(buffer2, "%d", index);
        strcat(buffer1, buffer2);
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadChunkExtraData(int index, int world)
{
    char worldString[5];
    sprintf(worldString, "%d", world);

    char fileLocation[50] = "\\\\fls0\\fxcraft\\world";
    char dirLocation[50] = "\\\\fls0\\fxcraft\\world";

    strcat(dirLocation, worldString);
    strcat(fileLocation, worldString);

    strcat(fileLocation, "\\chunkED");

    char numberBuffer[7];
    sprintf(numberBuffer, "%d", index);
    strcat(fileLocation, numberBuffer);

    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    int length = Bfile_GetFileSize_OS(hFile);

    //unsigned char buffer[length];

    Bfile_ReadFile_OS(hFile, &blockData[index*width*width*height], length, 0); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "Error: chunk not found: ";
        char buffer2[10];
        sprintf(buffer2, "%d", index);
        strcat(buffer1, buffer2);
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void saveCompressedChunk(int index, int world, bool exists)
{
	char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chunk%d", world, index);

    //getting entry for chunkFile
    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int sizeOfFile = width*width*height*2;
	char data[sizeOfFile];
	memcpy(data, blocks[index], width*width*height);
	memcpy(data+width*width*height, &blockData[index*width*width*height], width*width*height);

	char compressedData[sizeOfFile];
	int compressedSize = lz77_compress(data, sizeOfFile, compressedData, 8);

    //make file
    if(exists == false)
    Bfile_CreateEntry_OS(pFile, 1, &compressedSize);

    //writing data to file 
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare

    if(exists == true)
    {
        int length = Bfile_GetFileSize_OS(hFile);
        if(length != compressedSize)
        {
            Bfile_CloseFile_OS(hFile);
            Bfile_DeleteEntry(pFile);
            Bfile_CreateEntry_OS(pFile, 1, &compressedSize);
            hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
        }
    }

    Bfile_WriteFile_OS(hFile, compressedData, compressedSize);
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "Error: chunk not found: ";
        char buffer2[10];
        sprintf(buffer2, "%d", index);
        strcat(buffer1, buffer2);
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadCompressedChunk(int index, int world)
{
	char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chunk%d", world, index);

    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    int length = Bfile_GetFileSize_OS(hFile);

	char data[length];

    Bfile_ReadFile_OS(hFile, data, length, 0); 
    Bfile_CloseFile_OS(hFile);

	char decompressedData[width*width*height*3];
	int decompressedSize = lz77_decompress(data, decompressedData);

	//if(decompressedSize != width*width*height*2)
	//return;

	memcpy(blocks[index], decompressedData, width*width*height);
	memcpy(&blockData[index*width*width*height], decompressedData+width*width*height, width*width*height);

    if(hFile < 0)
    {
        clearAll(0xf800);
		char buffer[50];
		sprintf(buffer, "Error: chunk%d not found", index);
        renderTextUpdate(5, 5, 200, buffer);
    }
}
void saveChestDataCompressed(int world, int exists)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 121*chestAmount; //used=1, position=12, type=54, amount=54
    char chestData_[size];

    for (int chestI = 0; chestI < chestAmount; chestI++)
    {
        int i = 121*chestI;

        chestData_[i] = allchest[chestI].used;

        for (int j = 0; j < 4; j++)
        chestData_[i+1+j] = (allchest[chestI].position.x >> (j*8)) & 0xFF;
        for (int j = 0; j < 4; j++)
        chestData_[i+5+j] = (allchest[chestI].position.y >> (j*8)) & 0xFF;
        for (int j = 0; j < 4; j++)
        chestData_[i+9+j] = (allchest[chestI].position.z >> (j*8)) & 0xFF;

        for (int k = 0; k < 27; k++)
        {
            for (int j = 0; j < 2; j++)
            chestData_[i+13+j+(k*2)] = (allchest[chestI].chestBlockTypes[k] >> (j*8)) & 0xFF;
        }
        for (int k = 0; k < 27; k++)
        {
            for (int j = 0; j < 2; j++)
            chestData_[i+67+j+(k*2)] = (allchest[chestI].chestBlockTypes[k] >> (j*8)) & 0xFF;
        }
    }

    char compressedData[size];
    int compressedSize = lz77_compress(chestData_, size, compressedData, 8);

    if(exists == false)
    Bfile_CreateEntry_OS(pFile, 1, &compressedSize);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);

    if(exists == true)
    {
        int length = Bfile_GetFileSize_OS(hFile);
        if(length != compressedSize)
        {
            Bfile_CloseFile_OS(hFile);
            Bfile_DeleteEntry(pFile);
            Bfile_CreateEntry_OS(pFile, 1, &compressedSize);
            hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
        }
    }

    Bfile_WriteFile_OS(hFile, compressedData, compressedSize); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "chestData failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadChestDataCompressed(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        char chestData_[length];

        Bfile_ReadFile_OS(hFile, chestData_, length, 0); 
        Bfile_CloseFile_OS(hFile);

        char decompressedData[121*chestAmount];
        int decompressedSize = lz77_decompress(chestData_, decompressedData);

        for (int chestI = 0; chestI < chestAmount; chestI++)
        {
            int i = 121*chestI;

            allchest[chestI].position.x = 0;
            allchest[chestI].position.y = 0;
            allchest[chestI].position.z = 0;
            allchest[chestI].used = 0;

            allchest[chestI].used = decompressedData[i];

            for (int j = 0; j < 4; j++)
            allchest[chestI].position.x |= ((decompressedData[i+1+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            allchest[chestI].position.y |= ((decompressedData[i+5+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            allchest[chestI].position.z |= ((decompressedData[i+9+j] & 0xff) << j*8);

            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockTypes[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockTypes[k] |= ((decompressedData[i+13+j+(k*2)] & 0xff) << j*8);
            }
            for (int k = 0; k < 27; k++)
            {
                allchest[chestI].chestBlockAmount[k] = 0;

                for (int j = 0; j < 2; j++)
                allchest[chestI].chestBlockAmount[k] |= ((decompressedData[i+67+j+(k*2)] & 0xff) << j*8);
            }
        }
    }
    else
    saveChestData(world);
}
void saveEntityData(int world, int exists)
{
	char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\entityData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = entityLength*16; //12(pos)+1(active)+1(type)+1(state)+1(hasfur)
    char entityData_[size];

	for (int i = 0; i < entityLength; i++)
	{
		int posX = allObj[entityList[i].locationOnAllObj].position.x*10;
		int posY = allObj[entityList[i].locationOnAllObj].position.y*10;
		int posZ = allObj[entityList[i].locationOnAllObj].position.z*10;

		int currentIndex = i*16;

		for (int j = 0; j < 16; j++)
		entityData_[currentIndex+j] = 0;

		if(entityList[i].type != 255)
		{
			entityData_[currentIndex] = entityList[i].active;

            for (int j = 0; j < 4; j++)
            entityData_[currentIndex+j+1] = (posX >> (j*8)) & 0xFF;

            for (int j = 0; j < 4; j++)
            entityData_[currentIndex+j+5] = (posY >> (j*8)) & 0xFF;
            
            for (int j = 0; j < 4; j++)
            entityData_[currentIndex+j+9] = (posZ >> (j*8)) & 0xFF;

			entityData_[currentIndex+13] = entityList[i].type;
			entityData_[currentIndex+14] = entityList[i].state;

			if(entityList[i].type == 1)
			entityData_[currentIndex+15] = entityList[i].hasFur;
			else
			entityData_[currentIndex+15] = 0;
		}
	}

    if(exists == false)
    Bfile_CreateEntry_OS(pFile, 1, &size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);

    Bfile_WriteFile_OS(hFile, entityData_, size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "chestData failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadEntityData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\entityData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        char entityData_[length];

        Bfile_ReadFile_OS(hFile, entityData_, length, 0); 
        Bfile_CloseFile_OS(hFile);

		for (int i = 0; i < entityLength; i++)
		{
			entityList[i].active = false;

			int currentIndex = i*16;

			entityList[i].active = entityData_[currentIndex];

			int posX = 0;
			int posY = 0;
			int posZ = 0;

            for (int j = 0; j < 4; j++)
            posX |= ((entityData_[currentIndex+1+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            posY |= ((entityData_[currentIndex+5+j] & 0xff) << j*8);
            for (int j = 0; j < 4; j++)
            posZ |= ((entityData_[currentIndex+9+j] & 0xff) << j*8);

            entityList[i].position.x = posX;
            entityList[i].position.y = posY;
            entityList[i].position.z = posZ;

			entityList[i].type = entityData_[currentIndex+13];
			entityList[i].state = entityData_[currentIndex+14];
			entityList[i].hasFur = entityData_[currentIndex+15];
		}
    }
    else
    saveEntityData(world, false);
}
void deleteOldWorld(int world)
{
	for (int i = 0; i < totalChunkWidth*totalChunkWidth; i++)
	{
		char fileLocation[50];
   		sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chunk%d", world, i);

   		//getting entry for chunkFile
   		unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
   		Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
		Bfile_DeleteEntry(pFile);

		loadingScreen(i + 1, 129, "converting...", 13);
	}

	for (int i = 0; i < totalChunkWidth*totalChunkWidth; i++)
	{
		char fileLocation[50];
   		sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chunkED%d", world, i);

   		//getting entry for chunkFile"\\\\fls0\\fxcraft\\world%d\\chunkED%d", world, i
   		unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
   		Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
		Bfile_DeleteEntry(pFile);

		loadingScreen(i + 65, 129, "converting...", 13);
	}

    //for (int i = 0; i < 1; i++)
    //{
    //    char fileLocation[50];
   	//	sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

   	//	//getting entry for chunkFile
   	//	unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
   	//	Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
	//	Bfile_DeleteEntry(pFile);

	//	loadingScreen(i + 128, 129, "converting...", 13);
    //}
	
}
void savePlayerData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\playerData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 12 + 8 + (18 + 18 + 18) + (54 + 54 + 54);
    char playerData[size];

    int pPosI[5];
    pPosI[0] = PPosX * 100;
    pPosI[1] = PPosY * 100;
    pPosI[2] = PPosZ * 100;
    pPosI[3] = rotationX * 100;
    pPosI[4] = rotationY * 100;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        playerData[i*4+j] = (pPosI[i] >> (j*8)) & 0xFF;
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 2; j++)
        playerData[20+i*6+j] = (hotbarBlockTypes[i] >> j*8) & 0xFF;
        for (int j = 2; j < 4; j++)
        playerData[20+i*6+j] = (hotbarBlockAmount[i] >> (j-2)*8) & 0xFF;
		for (int j = 4; j < 6; j++)
        playerData[20+i*6+j] = (hotbarBlockHp[i] >> (j-4)*8) & 0xFF;
    }

    for (int i = 0; i < 27; i++)
    {
        for (int j = 0; j < 2; j++)
        playerData[74+i*6+j] = (inventoryBlockTypes[i] >> j*8) & 0xFF;
        for (int j = 2; j < 4; j++)
        playerData[74+i*6+j] = (inventoryBlockAmount[i] >> (j-2)*8) & 0xFF;
		for (int j = 4; j < 6; j++)
        playerData[74+i*6+j] = (inventoryBlockHp[i] >> (j-4)*8) & 0xFF;
    }
    

    Bfile_CreateEntry_OS(pFile, 1, &size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    Bfile_WriteFile_OS(hFile, playerData, size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "playerData failure.";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}
void loadPlayerData(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\playerData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 236;
    char playerData[236] = {0};
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        //unsigned char buffer[length];

        Bfile_ReadFile_OS(hFile, playerData, length, 0); 
        Bfile_CloseFile_OS(hFile);

        int pPosI[5] = {0};

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 4; j++)
            pPosI[i] |= ((playerData[i*4+j] & 0xff) << j*8);
        }
        if(pPosI[0] != 0)
        PPosX = (float)pPosI[0] / 100;
        if(pPosI[1] != 0)
        PPosY = (float)pPosI[1] / 100;
        if(pPosI[2] != 0)
        PPosZ = (float)pPosI[2] / 100;
        if(pPosI[3] != 0)
        rotationX = (float)pPosI[3] / 100;
        if(pPosI[4] != 0)
        rotationY = (float)pPosI[4] / 100;

        for (int i = 0; i < 9; i++)
        {
            hotbarBlockTypes[i] = 0;
            hotbarBlockAmount[i] = 0;
			hotbarBlockHp[i] = 0;

            for (int j = 0; j < 2; j++)
            hotbarBlockTypes[i] |= ((playerData[20+i*6+j] & 0xFF) << j*8);
            for (int j = 2; j < 4; j++)
            hotbarBlockAmount[i] |= ((playerData[20+i*6+j] & 0xFF) << (j-2)*8);
			for (int j = 4; j < 6; j++)
            hotbarBlockHp[i] |= ((playerData[20+i*6+j] & 0xFF) << (j-4)*8);
        }

        for (int i = 0; i < 27; i++)
        {
            inventoryBlockTypes[i] = 0;
            inventoryBlockAmount[i] = 0;
			inventoryBlockHp[i] = 0;

            for (int j = 0; j < 2; j++)
            inventoryBlockTypes[i] |= ((playerData[74+i*6+j] & 0xFF) << j*8);
            for (int j = 2; j < 4; j++)
            inventoryBlockAmount[i] |= ((playerData[74+i*6+j] & 0xFF) << (j-2)*8);
			for (int j = 4; j < 6; j++)
            inventoryBlockHp[i] |= ((playerData[74+i*6+j] & 0xFF) << (j-4)*8);
        }
    }
}
void loadPlayerDataV4(int world)
{
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\world%d\\playerData", world);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 164;
    char playerData[164] = {0};
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        //unsigned char buffer[length];

        Bfile_ReadFile_OS(hFile, playerData, length, 0); 
        Bfile_CloseFile_OS(hFile);

        int pPosI[5] = {0};

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 4; j++)
            pPosI[i] |= ((playerData[i*4+j] & 0xff) << j*8);
        }
        if(pPosI[0] != 0)
        PPosX = (float)pPosI[0] / 100;
        if(pPosI[1] != 0)
        PPosY = (float)pPosI[1] / 100;
        if(pPosI[2] != 0)
        PPosZ = (float)pPosI[2] / 100;
        if(pPosI[3] != 0)
        rotationX = (float)pPosI[3] / 100;
        if(pPosI[4] != 0)
        rotationY = (float)pPosI[4] / 100;

        for (int i = 0; i < 9; i++)
        {
            hotbarBlockTypes[i] = 0;
            hotbarBlockAmount[i] = 0;

            for (int j = 0; j < 2; j++)
            hotbarBlockTypes[i] |= ((playerData[20+i*4+j] & 0xFF) << j*8);
            for (int j = 2; j < 4; j++)
            hotbarBlockAmount[i] |= ((playerData[20+i*4+j] & 0xFF) << (j-2)*8);
        }

        for (int i = 0; i < 27; i++)
        {
            inventoryBlockTypes[i] = 0;
            inventoryBlockAmount[i] = 0;

            for (int j = 0; j < 2; j++)
            inventoryBlockTypes[i] |= ((playerData[56+i*4+j] & 0xFF) << j*8);
            for (int j = 2; j < 4; j++)
            inventoryBlockAmount[i] |= ((playerData[56+i*4+j] & 0xFF) << (j-2)*8);
        }
    }
}
void saveAllChunk(int world, bool exists)
{
    DmaWaitNext();

    if(exists == false) //cleating dir
    {
        char dirLocation1[50] = "\\\\fls0\\fxcraft";

        unsigned short pDir1[sizeof(dirLocation1)*2]; // Make buffer
        Bfile_StrToName_ncpy(pDir1, (unsigned char*)dirLocation1, sizeof(dirLocation1)); 
        Bfile_CreateEntry_OS(pDir1, 5, NULL);

        char worldString[5];
        sprintf(worldString, "%d", world);

        char dirLocation2[50] = "\\\\fls0\\fxcraft\\world";
        strcat(dirLocation2, worldString);

        unsigned short pDir[sizeof(dirLocation2)*2]; // Make buffer
        Bfile_StrToName_ncpy(pDir, (unsigned char*)dirLocation2, sizeof(dirLocation2)); 
        Bfile_CreateEntry_OS(pDir, 5, NULL);
    }

    savePlayerData(world);
    saveWorldData(world);
    //saveChestDataCompressed(world, exists);
	saveChestData(world);
	saveEntityData(world, exists);

    for (int i = 0; i < totalChunkWidth*totalChunkWidth; i++)
    {
		if(worldVersion[world-1] < 4)
		{
        	saveChunk(i, world, exists);
        	if(worldVersion[world-1] < 2)
        	saveChunkExtraData(i, world, false);
        	else
        	saveChunkExtraData(i, world, exists);
		}
		else
		saveCompressedChunk(i, world, exists);

        loadingScreen(i + 1, 64, "Saving...", 9);
    }
	
}
void loadAllChunk(int world)
{
    DmaWaitNext();
    
    if (worldVersion[world-1] <= currentVersion)
    {
        loadWorldData(world);

		if(worldVersion[world-1] < 5)
        {
            loadPlayerDataV4(world);
            loadChestDataV4(world);
        }
		else
        {
		    loadPlayerData(world);
            loadChestData(world);
        }

		if(worldVersion[world-1] >= 4)
		loadEntityData(world);

        for (int i = 0; i < 16; i++)
        screenColor[i] = skyColors[skyBrightness-1];

        for (int i = 0; i < totalChunkWidth*totalChunkWidth; i++)
        {
        	if(worldVersion[world-1] < 4)
        	{
            	loadChunk(i, world);

            	if(worldVersion[world-1] >= 2) //only load if its added as a new version
            	loadChunkExtraData(i, world);
        	}
        	else
        	loadCompressedChunk(i, world);

            loadingScreen(i + 1, 64, "Loading...", 10);
        }

        if(lighting == true)
        {
            resetLightMapAll();
            loadLighting2();
        }

        updateItemSlected();

        if(worldVersion[world-1] < 4) //convert world to compressed version
        {
        	deleteOldWorld(world);
        	worldVersion[world-1] = 4; //old 2min new 
        	saveAllChunk(world, false);
            //saveChestDataCompressed(world, false);
        	savegameData();
			saveEntityData(world, false);
        }

		if(worldVersion[world-1] < 5) //save the health of tools
        {
        	char fileLocation1[50];
			sprintf(fileLocation1, "\\\\fls0\\fxcraft\\world%d\\playerData", world);

   			unsigned short pFile1[sizeof(fileLocation1)*2];
   			Bfile_StrToName_ncpy(pFile1, (unsigned char*)fileLocation1, sizeof(fileLocation1)); 
			Bfile_DeleteEntry(pFile1);

            char fileLocation2[50];
			sprintf(fileLocation2, "\\\\fls0\\fxcraft\\world%d\\chestData", world);

   			unsigned short pFile2[sizeof(fileLocation2)*2];
   			Bfile_StrToName_ncpy(pFile2, (unsigned char*)fileLocation2, sizeof(fileLocation2)); 
			Bfile_DeleteEntry(pFile2);

            saveChestData(world);
			savePlayerData(world);
			replaceOldWater();
			saveAllChunk(world, false);
			worldVersion[world-1] = 5;
			savegameData();
        }

		loadAllEntitys();
    }
    else
    {
        clearAll(0xf800);
        renderTextUpdate(5, 5, 500, "Error: world version not supported.");
    }
}

bool useCurrentVersion = false;
char texturePackCreator[5][24];
char texturePackName[5][24];
char texturePackTotalPath[5][50];
color_t texturePackIcon[5][1024];
int texturePackVersion[5];
int maxTextureIndex = 0;

void loadTextureAssets()
{
    char fileLocation[50] = "";
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\textures", texturePackTotalPath[currentTexturePackIndex]);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = textureBlockAmount*256*2;

    for (int i = 0; i < textureBlockAmount; i++)
    {
        for (int j = 0; j < 256; j++)
        textures2[i*256+j] = missingTexture[j];
    }
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, assetsInputBuffer, length, 0); 
        Bfile_CloseFile_OS(hFile);

        LZ4_decompress_safe(assetsInputBuffer, textures2, length, 0x40000);
    }
    else
    renderTextUpdate(5, 5, 1000, "No texture found");
}
void loadIconAssets()
{
    char fileLocation[50] = "";
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\icons", texturePackTotalPath[currentTexturePackIndex]);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = itemAmount*676*2;

    for (int i = 0; i < itemAmount; i++)
    {
        for (int j = 0; j < 676; j++)
        itemIcons2[i*676+j] = missingIcon[j];
    }
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, assetsInputBuffer, length, 0); 
        Bfile_CloseFile_OS(hFile);

        LZ4_decompress_safe(assetsInputBuffer, itemIcons2, length, 0x40000);
    }
    else
    renderTextUpdate(5, 5, 1000, "No texture found");
}
void loadTexturePackData(char *texturePackPath, char *creator, char *name, int* version)
{
    char fileLocation[100];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\info", texturePackPath);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 

    int size = 52;
    char data[size];
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write, 3=readwrite, 4=readwriteshare
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, data, length, 0); 
        Bfile_CloseFile_OS(hFile);

        int ver = 0;
        ver = 0;
        for (int i = 0; i < 4; i++)
        ver |= ((data[i] & 0xff) << i*8);
        *version = ver;

        for (int i = 0; i < 24; i++)
        name[i] = data[4+i];

        for (int i = 0; i < 24; i++)
        creator[i] = data[28+i];
    }
    else
    {
        char txt[5] = "error";

        for (int i = 0; i < 5; i++)
        {
            creator[i] = txt[i];
            name[i] = txt[i];
            *version = 0;
        }
    }
}
void loadTexturePackIcon(char *texturePackPath, color_t *icon)
{
    char fileLocation[100];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\icon", texturePackPath);

    unsigned short pFile[sizeof(fileLocation)*2];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
    
    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    if(hFile > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile);

        Bfile_ReadFile_OS(hFile, (unsigned char*)icon, 2048, 0); 
        Bfile_CloseFile_OS(hFile);
    }
    else
    {
        for (int i = 0; i < 1024; i++)
        icon[i] = 0xffff;

        renderTextUpdate(5, 5, 1000, "no found");
    }
}
void loadAvailableTexturePacks()
{
    char fileLocation[50] = "\\\\fls0\\fxcraft\\assets\\tex_*";

    unsigned short found[100];
    unsigned short pFile[sizeof(fileLocation)*2];

    int ret, handle;
    file_type_t info;
    char location[50] = "";
    maxTextureIndex = 0;
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); // Overkill
    ret = Bfile_FindFirst(pFile, &handle, found, &info);
    if(ret == 0){
        Bfile_NameToStr_ncpy(location, found, 50);
        loadTexturePackData(location, texturePackCreator[maxTextureIndex], texturePackName[maxTextureIndex], &texturePackVersion[maxTextureIndex]);
        loadTexturePackIcon(location, texturePackIcon[maxTextureIndex]);

        for (int i = 0; i < 50; i++)
        {
            if(location[i] != 0)
            texturePackTotalPath[maxTextureIndex][i] = location[i];
        }

        maxTextureIndex++;
    }

    while(ret == 0 && maxTextureIndex < 5){

        ret = Bfile_FindNext(handle, found, &info);
        if(ret == 0){
            Bfile_NameToStr_ncpy(location, found, 50);
            loadTexturePackData(location, texturePackCreator[maxTextureIndex], texturePackName[maxTextureIndex], &texturePackVersion[maxTextureIndex]);
            loadTexturePackIcon(location, texturePackIcon[maxTextureIndex]);

            for (int i = 0; i < 50; i++)
            {
                if(location[i] != 0)
                texturePackTotalPath[maxTextureIndex][i] = location[i];
            }
    
            maxTextureIndex++;
        }
    }
    Bfile_FindClose(handle);
}

void convertToBitmap16bit() 
{
    DmaWaitNext();
    
    int imageWidth = 384;
    int imageHeight = 216;
    
    // Bitmap header (14 bytes)
    unsigned char header[14] = {
        'B', 'M',               // Signature
        0, 0, 0, 0,             // File size (to be calculated)
        0, 0,                   // Reserved
        0, 0,                   // Reserved
        138, 0, 0, 0            // Offset to pixel data (14 + 40 + 12)
    };

	unsigned int imageSize = imageWidth * imageHeight * 2;

	unsigned char v5_header[124] = {
        124, 0, 0, 0,           // Header size (124 bytes)
        imageWidth & 0xFF, (imageWidth >> 8) & 0xFF, (imageWidth >> 16) & 0xFF, (imageWidth >> 24) & 0xFF,
        imageHeight & 0xFF, (imageHeight >> 8) & 0xFF, (imageHeight >> 16) & 0xFF, (imageHeight >> 24) & 0xFF,
        1, 0,                   // Number of color planes (must be 1)
        16, 0,                  // Bits per pixel (16-bit)
        3, 0, 0, 0,             // Compression (BI_BITFIELDS)
        imageSize & 0xFF, (imageSize >> 8) & 0xFF, (imageSize >> 16) & 0xFF, (imageSize >> 24) & 0xFF,
        0, 0, 0, 0,             // Horizontal resolution (pixels per meter)
        0, 0, 0, 0,             // Vertical resolution (pixels per meter)
        0, 0, 0, 0,             // Number of colors in the palette
        0, 0, 0, 0,             // Important colors

        // Color Masks (RGB565)
        0x00, 0xF8, 0x00, 0x00, // Red mask:   0xF800
        0xE0, 0x07, 0x00, 0x00, // Green mask: 0x07E0
        0x1F, 0x00, 0x00, 0x00, // Blue mask:  0x001F
        0x00, 0x00, 0x00, 0x00, // Alpha mask: 0 (unused)

        // Color space type (LCS_WINDOWS_COLOR_SPACE)
        0x20, 0x6E, 0x69, 0x57, // 'Win ' in little endian

        // CIEXYZTRIPLE endpoints (unused, set to zero)
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        // Gamma values (unused, set to zero)
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        // Rendering intent (LCS_GM_IMAGES)
        4, 0, 0, 0,

        // Color profile data (unused)
        0, 0, 0, 0, 0, 0, 0, 0
    };

    int file_size = imageWidth * imageHeight * 2 + 14 + 124;
    header[2] = file_size & 0xFF;
    header[3] = (file_size >> 8) & 0xFF;
    header[4] = (file_size >> 16) & 0xFF;
    header[5] = (file_size >> 24) & 0xFF;

    unsigned char *bitmap_image = (unsigned char *)(0x8C200000 + ALLOC_OFFSET);

    memcpy(bitmap_image, header, 14);
    memcpy(bitmap_image + 14, v5_header, 124);

    // Write pixel data
    unsigned char *pixel_data = (unsigned char *)(bitmap_image + 138);

	for (int i = 0; i < imageHeight; i++) 
	{
	    for (int j = 0; j < imageWidth; j++) 
		{
	        // Flip vertically: write from bottom to top
	        unsigned short color = *(VRAMAddress + i * resX + j);

	        // Convert to little-endian: swap bytes
	        unsigned char low_byte = color & 0xFF;          // Least significant byte
	        unsigned char high_byte = (color >> 8) & 0xFF;  // Most significant byte

	        pixel_data[((imageHeight - 1 - i) * imageWidth + j) * 2]     = low_byte;   // LSB first
	        pixel_data[((imageHeight - 1 - i) * imageWidth + j) * 2 + 1] = high_byte;  // MSB second
	    }
	}

	//get the current screenshot position
	int amount = 1;
	int fileSizeSize = 1;

	char fileLocationSize[50] = "\\\\fls0\\fxcraft\\screenshots\\amount";
    unsigned short pFileSize[sizeof(fileLocationSize)*2];
    Bfile_StrToName_ncpy(pFileSize, (unsigned char*)fileLocationSize, sizeof(fileLocationSize)); 

    int hFile2 = Bfile_OpenFile_OS(pFileSize, 3, NULL);
    if(hFile2 > 0)
    {
        int length = Bfile_GetFileSize_OS(hFile2);
        char fileData[length];

        Bfile_ReadFile_OS(hFile2, fileData, length, 0); 
        Bfile_CloseFile_OS(hFile2);
		amount = fileData[0];
	}
	else
    Bfile_CreateEntry_OS(pFileSize, 1, &fileSizeSize);

	char fileData[1];
	fileData[0] = amount+1;
    int hFile3 = Bfile_OpenFile_OS(pFileSize, 3, NULL);
    Bfile_WriteFile_OS(hFile3, fileData, fileSizeSize); 
    Bfile_CloseFile_OS(hFile3);

	//save the actual screenshot
    char fileLocation[50];
    sprintf(fileLocation, "\\\\fls0\\fxcraft\\screenshots\\image%d.bmp", amount);

    char fileLocation2[50] = "\\\\fls0\\fxcraft\\screenshots";

    unsigned short pFile1[sizeof(fileLocation2)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile1, (unsigned char*)fileLocation2, sizeof(fileLocation2)); 

    Bfile_CreateEntry_OS(pFile1, 5, NULL);


    unsigned short pFile[sizeof(fileLocation)*2]; // Make buffer
    Bfile_StrToName_ncpy(pFile, (unsigned char*)fileLocation, sizeof(fileLocation)); 
    Bfile_CreateEntry_OS(pFile, 1, &file_size);

    int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
    Bfile_WriteFile_OS(hFile, bitmap_image, file_size); 
    Bfile_CloseFile_OS(hFile);

    if(hFile < 0)
    {
        clearAll(0xf800);
        char buffer1[40] = "Error";
        renderTextColord(5, 5, buffer1, 0xffff);
        OS_InnerWait_ms(200);
    }
}

//collision
