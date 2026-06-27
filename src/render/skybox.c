/* src/render/skybox.c - Skybox construction */
#include "engine.h"


		{-10, -10, 0}, {0, -10, -10}, {10, 0, -10}, {10, 0, 10}, {0, 10, 10}, {10, 10, 0}, {0, -10, 10}, {-10, 10, 0},
		{0, 10, -10}, {10, -10, 0}, {10, 0, 0}, {0, -10, 0}, {0, 10, 0},
	};
	Vector4B        td[12] = {
		{20, 15, 6, 12}, {19, 17, 3, 14}, {18, 13, 2, 11}, {17, 18, 11, 3}, {1, 10, 18, 17}, {10, 0, 13, 18}, {8, 19, 14, 7}, {5, 9, 19, 8},
		{9, 1, 17, 19}, {13, 20, 12, 2}, {0, 16, 20, 13}, {16, 4, 15, 20},
	};
	Vector4B          uvd[12] = {
		{0, 0, 0, 0}, {10, 10, 10, 10}, {5, 0, 0, 5}, {10, 5, 5, 10}, {10, 5, 5, 10}, {5, 0, 0, 5}, {10, 10, 10, 10}, {10, 10, 10, 10},
		{10, 10, 10, 10}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	};

	float cameraRotationX = ToRadians(rotationX);
    float cameraRotationY = ToRadians(0);

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

	for (int allVert = 0; allVert < 21; allVert++)
    {
        int x3D = (vd[allVert].z * 400);
        int y3D = (vd[allVert].y * 200);
        int z3D = (vd[allVert].x * 200);

        int z = (x3D * cosXSinY - y3D * sinRotX + z3D * cosXcosY) / 1000;

		if(z < 1)
		z = 1;

        if(z < 65000)
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
                calculatedSPos[allVert].x = xOnScreen;
                calculatedSPos[allVert].y = yOnScreen;
                calculatedSPos[allVert].z = z;
            }
            else
            calculatedSPos[allVert].z = 65001;
        }
        else
        calculatedSPos[allVert].z = 65001;
    }

    for (int alltri = 0; alltri < 12; alltri++)
    {
        Vector4B indexes1 = td[alltri];

        screenPoint v1 = calculatedSPos[indexes1.x];
        screenPoint v2 = calculatedSPos[indexes1.y];
        screenPoint v3 = calculatedSPos[indexes1.z];
        screenPoint v4 = calculatedSPos[indexes1.w];

        if(v1.z != 65001 && v2.z != 65001 && v3.z != 65001 && v4.z != 65001)
        {
            if((v1.x > 0 && v1.x < resX) || (v2.x > 0 && v2.x < resX) || (v3.x > 0 && v3.x < resX) || (v4.x > 0 && v4.x < resX))
            {
                if((v1.y > 0 && v1.y < resY) || (v2.y > 0 && v2.y < resY) || (v3.y > 0 && v3.y < resY) || (v4.y > 0 && v4.y < resY))
                {
                    Vector2S V1_ = {v1.x, v1.y};
                    Vector2S V2_ = {v2.x, v2.y};
                    Vector2S V3_ = {v3.x, v3.y};
                    Vector2S V4_ = {v4.x, v4.y};

                    Vector2S AT = {5, uvd[alltri].x};
                    Vector2S BT = {5, uvd[alltri].y};
                    Vector2S CT = {5, uvd[alltri].z};
                    Vector2S DT = {5, uvd[alltri].w};

					renderSkybox(V1_, V2_, V3_, AT, BT, CT);
                    renderSkybox(V1_, V3_, V4_, AT, CT, DT);
                }
            }
        }
    }
}

//line rendering
