#pragma once

void loadTextureAssets();
void loadIconAssets();
void loadTexturePackData(char *texturePackPath, char *creator, char *name,
                         int *version);
void loadTexturePackIcon(char *texturePackPath, color_t *icon);
void loadAvailableTexturePacks();
void convertToBitmap16bit();