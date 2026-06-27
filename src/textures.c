/* src/textures.c */
#include "config.h"
#include "engine.h"

void loadTextureAssets() {
  char fileLocation[50] = "";
  sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\textures",
          texturePackTotalPath[currentTexturePackIndex]);

  unsigned short pFile[sizeof(fileLocation) * 2];
  Bfile_StrToName_ncpy(pFile, (unsigned char *)fileLocation,
                       sizeof(fileLocation));

  int size = textureBlockAmount * 256 * 2;

  for (int i = 0; i < textureBlockAmount; i++) {
    for (int j = 0; j < 256; j++)
      textures2[i * 256 + j] = missingTexture[j];
  }

  int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
  if (hFile > 0) {
    int length = Bfile_GetFileSize_OS(hFile);

    Bfile_ReadFile_OS(hFile, assetsInputBuffer, length, 0);
    Bfile_CloseFile_OS(hFile);

    LZ4_decompress_safe(assetsInputBuffer, textures2, length, 0x40000);
  } else
    renderTextUpdate(5, 5, 1000, "No texture found");
}
void loadIconAssets() {
  char fileLocation[50] = "";
  sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\icons",
          texturePackTotalPath[currentTexturePackIndex]);

  unsigned short pFile[sizeof(fileLocation) * 2];
  Bfile_StrToName_ncpy(pFile, (unsigned char *)fileLocation,
                       sizeof(fileLocation));

  int size = itemAmount * 676 * 2;

  for (int i = 0; i < itemAmount; i++) {
    for (int j = 0; j < 676; j++)
      itemIcons2[i * 676 + j] = missingIcon[j];
  }

  int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
  if (hFile > 0) {
    int length = Bfile_GetFileSize_OS(hFile);

    Bfile_ReadFile_OS(hFile, assetsInputBuffer, length, 0);
    Bfile_CloseFile_OS(hFile);

    LZ4_decompress_safe(assetsInputBuffer, itemIcons2, length, 0x40000);
  } else
    renderTextUpdate(5, 5, 1000, "No texture found");
}
void loadTexturePackData(char *texturePackPath, char *creator, char *name,
                         int *version) {
  char fileLocation[100];
  sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\info", texturePackPath);

  unsigned short pFile[sizeof(fileLocation) * 2];
  Bfile_StrToName_ncpy(pFile, (unsigned char *)fileLocation,
                       sizeof(fileLocation));

  int size = 52;
  char data[size];

  int hFile = Bfile_OpenFile_OS(
      pFile, 3, NULL); // Get handle          //0=read, 1=read_share, 2=write,
                       // 3=readwrite, 4=readwriteshare
  if (hFile > 0) {
    int length = Bfile_GetFileSize_OS(hFile);

    Bfile_ReadFile_OS(hFile, data, length, 0);
    Bfile_CloseFile_OS(hFile);

    int ver = 0;
    ver = 0;
    for (int i = 0; i < 4; i++)
      ver |= ((data[i] & 0xff) << i * 8);
    *version = ver;

    for (int i = 0; i < 24; i++)
      name[i] = data[4 + i];

    for (int i = 0; i < 24; i++)
      creator[i] = data[28 + i];
  } else {
    char txt[5] = "error";

    for (int i = 0; i < 5; i++) {
      creator[i] = txt[i];
      name[i] = txt[i];
      *version = 0;
    }
  }
}
void loadTexturePackIcon(char *texturePackPath, color_t *icon) {
  char fileLocation[100];
  sprintf(fileLocation, "\\\\fls0\\fxcraft\\assets\\%s\\icon", texturePackPath);

  unsigned short pFile[sizeof(fileLocation) * 2];
  Bfile_StrToName_ncpy(pFile, (unsigned char *)fileLocation,
                       sizeof(fileLocation));

  int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
  if (hFile > 0) {
    int length = Bfile_GetFileSize_OS(hFile);

    Bfile_ReadFile_OS(hFile, (unsigned char *)icon, 2048, 0);
    Bfile_CloseFile_OS(hFile);
  } else {
    for (int i = 0; i < 1024; i++)
      icon[i] = 0xffff;

    renderTextUpdate(5, 5, 1000, "no found");
  }
}
void loadAvailableTexturePacks() {
  char fileLocation[50] = "\\\\fls0\\fxcraft\\assets\\tex_*";

  unsigned short found[100];
  unsigned short pFile[sizeof(fileLocation) * 2];

  int ret, handle;
  file_type_t info;
  char location[50] = "";
  maxTextureIndex = 0;
  Bfile_StrToName_ncpy(pFile, (unsigned char *)fileLocation,
                       sizeof(fileLocation)); // Overkill
  ret = Bfile_FindFirst(pFile, &handle, found, &info);
  if (ret == 0) {
    Bfile_NameToStr_ncpy(location, found, 50);
    loadTexturePackData(location, texturePackCreator[maxTextureIndex],
                        texturePackName[maxTextureIndex],
                        &texturePackVersion[maxTextureIndex]);
    loadTexturePackIcon(location, texturePackIcon[maxTextureIndex]);

    for (int i = 0; i < 50; i++) {
      if (location[i] != 0)
        texturePackTotalPath[maxTextureIndex][i] = location[i];
    }

    maxTextureIndex++;
  }

  while (ret == 0 && maxTextureIndex < 5) {

    ret = Bfile_FindNext(handle, found, &info);
    if (ret == 0) {
      Bfile_NameToStr_ncpy(location, found, 50);
      loadTexturePackData(location, texturePackCreator[maxTextureIndex],
                          texturePackName[maxTextureIndex],
                          &texturePackVersion[maxTextureIndex]);
      loadTexturePackIcon(location, texturePackIcon[maxTextureIndex]);

      for (int i = 0; i < 50; i++) {
        if (location[i] != 0)
          texturePackTotalPath[maxTextureIndex][i] = location[i];
      }

      maxTextureIndex++;
    }
  }
  Bfile_FindClose(handle);
}

void convertToBitmap16bit() {
  DmaWaitNext();

  int imageWidth = 384;
  int imageHeight = 216;

  // Bitmap header (14 bytes)
  unsigned char header[14] = {
      'B', 'M',       // Signature
      0,   0,   0, 0, // File size (to be calculated)
      0,   0,         // Reserved
      0,   0,         // Reserved
      138, 0,   0, 0  // Offset to pixel data (14 + 40 + 12)
  };

  unsigned int imageSize = imageWidth * imageHeight * 2;

  unsigned char v5_header[124] = {
      124, 0, 0, 0, // Header size (124 bytes)
      imageWidth & 0xFF, (imageWidth >> 8) & 0xFF, (imageWidth >> 16) & 0xFF,
      (imageWidth >> 24) & 0xFF, imageHeight & 0xFF, (imageHeight >> 8) & 0xFF,
      (imageHeight >> 16) & 0xFF, (imageHeight >> 24) & 0xFF, 1,
      0,          // Number of color planes (must be 1)
      16, 0,      // Bits per pixel (16-bit)
      3, 0, 0, 0, // Compression (BI_BITFIELDS)
      imageSize & 0xFF, (imageSize >> 8) & 0xFF, (imageSize >> 16) & 0xFF,
      (imageSize >> 24) & 0xFF, 0, 0, 0,
      0,          // Horizontal resolution (pixels per meter)
      0, 0, 0, 0, // Vertical resolution (pixels per meter)
      0, 0, 0, 0, // Number of colors in the palette
      0, 0, 0, 0, // Important colors

      // Color Masks (RGB565)
      0x00, 0xF8, 0x00, 0x00, // Red mask:   0xF800
      0xE0, 0x07, 0x00, 0x00, // Green mask: 0x07E0
      0x1F, 0x00, 0x00, 0x00, // Blue mask:  0x001F
      0x00, 0x00, 0x00, 0x00, // Alpha mask: 0 (unused)

      // Color space type (LCS_WINDOWS_COLOR_SPACE)
      0x20, 0x6E, 0x69, 0x57, // 'Win ' in little endian

      // CIEXYZTRIPLE endpoints (unused, set to zero)
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Gamma values (unused, set to zero)
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Rendering intent (LCS_GM_IMAGES)
      4, 0, 0, 0,

      // Color profile data (unused)
      0, 0, 0, 0, 0, 0, 0, 0};

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

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      // Flip vertically: write from bottom to top
      unsigned short color = *(VRAMAddress + i * resX + j);

      // Convert to little-endian: swap bytes
      unsigned char low_byte = color & 0xFF;         // Least significant byte
      unsigned char high_byte = (color >> 8) & 0xFF; // Most significant byte

      pixel_data[((imageHeight - 1 - i) * imageWidth + j) * 2] =
          low_byte; // LSB first
      pixel_data[((imageHeight - 1 - i) * imageWidth + j) * 2 + 1] =
          high_byte; // MSB second
    }
  }

  // get the current screenshot position
  int amount = 1;
  int fileSizeSize = 1;

  char fileLocationSize[50] = "\\\\fls0\\fxcraft\\screenshots\\amount";
  unsigned short pFileSize[sizeof(fileLocationSize) * 2];
  Bfile_StrToName_ncpy(pFileSize, (unsigned char *)fileLocationSize,
                       sizeof(fileLocationSize));

  int hFile2 = Bfile_OpenFile_OS(pFileSize, 3, NULL);
  if (hFile2 > 0) {
    int length = Bfile_GetFileSize_OS(hFile2);
    char fileData[length];

    Bfile_ReadFile_OS(hFile2, fileData, length, 0);
    Bfile_CloseFile_OS(hFile2);
    amount = fileData[0];
  } else
    Bfile_CreateEntry_OS(pFileSize, 1, &fileSizeSize);

  char fileData[1];
  fileData[0] = amount + 1;
  int hFile3 = Bfile_OpenFile_OS(pFileSize, 3, NULL);
  Bfile_WriteFile_OS(hFile3, fileData, fileSizeSize);
  Bfile_CloseFile_OS(hFile3);

  // save the actual screenshot
  char fileLocation[50];
  sprintf(fileLocation, "\\\\fls0\\fxcraft\\screenshots\\image%d.bmp", amount);

  char fileLocation2[50] = "\\\\fls0\\fxcraft\\screenshots";

  unsigned short pFile1[sizeof(fileLocation2) * 2]; // Make buffer
  Bfile_StrToName_ncpy(pFile1, (unsigned char *)fileLocation2,
                       sizeof(fileLocation2));

  Bfile_CreateEntry_OS(pFile1, 5, NULL);

  unsigned short pFile[sizeof(fileLocation) * 2]; // Make buffer
  Bfile_StrToName_ncpy(pFile, (unsigned char *)fileLocation,
                       sizeof(fileLocation));
  Bfile_CreateEntry_OS(pFile, 1, &file_size);

  int hFile = Bfile_OpenFile_OS(pFile, 3, NULL);
  Bfile_WriteFile_OS(hFile, bitmap_image, file_size);
  Bfile_CloseFile_OS(hFile);

  if (hFile < 0) {
    clearAll(0xf800);
    char buffer1[40] = "Error";
    renderTextColord(5, 5, buffer1, 0xffff);
    OS_InnerWait_ms(200);
  }
}
