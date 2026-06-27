/* src/world/noise.c */
/* src/world/noise.c - Particle creation, Perlin noise, terrain height */
#include "engine.h"
#include "chunk_constants.h"

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

//perlin noise
int SEED = 0;

#define terrainHeight 0
#define height 40
#define width 12
#define totalChunkWidth 8

bool blocksRenderd[totalChunkWidth*totalChunkWidth];
unsigned char maxChunkHight[totalChunkWidth*totalChunkWidth];

const unsigned char hash[256] = { 208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                        185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                        9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                        70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                        203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                        164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                        228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                        232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                        193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                        101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                        135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                        114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}
float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}
float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}
float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int+1, y_int);
    int u = noise2(x_int, y_int+1);
    int v = noise2(x_int+1, y_int+1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}
float perlin2d(float x, float y, float freq, int depth)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}

int noise2I(int x, int y)
{
    int tmp = hash[(y/1000 + SEED) % 256];
    return hash[(tmp + x/1000) % 256];
}
int lin_interI(int x, int y, int s)
{
    return x + s * (y-x) / 1000;
}
int smooth_interI(int x, int y, int s)
{
    return lin_interI(x, y, s * s * (3000-2000*s/1000) / 1000000);
}
int noise2dI(int x, int y)
{
    int x_int = x/1000;
    int y_int = y/1000;

    x_int *= 1000;
    y_int *= 1000;

    int x_frac = x - x_int;
    int y_frac = y - y_int;

    int s = noise2I(x_int, y_int) * 1000;
    int t = noise2I(x_int+1000, y_int) * 1000;
    int u = noise2I(x_int, y_int+1000) * 1000;
    int v = noise2I(x_int+1000, y_int+1000) * 1000;

    int low = smooth_interI(s, t, x_frac);
    int high = smooth_interI(u, v, x_frac);

    return smooth_interI(low, high, y_frac);
}

int getHeight(int x, int z)
{
    int chunkX = x/width;
    int chunkZ = z/width;
    int posX = x%width;
    int posZ = z%width;

    int y = 0;
    for (int i = 0; i < height; i++)
	{
        if(blocks[chunkX+chunkZ*totalChunkWidth][posX+posZ*width+i*width*width] == 1)
        {
            y = i;
            break;
        }
    }
    return y;
}
int GetPerlin(int x, int z)
{
    int Y1 = perlin2d(x * 2, z * 2, 0.032, 1) * 20;
    int Y2 = perlin2d(x * 2, z * 2, 0.014, 1) * 70;
    int Y3 = perlin2d(x * 2, z * 2, 0.006, 1) * 48;

    return ((Y1 + Y2 + Y3) / 3) + terrainHeight;
}
int Perlin3D(int x, int y, int z) 
{
    int ab = noise2dI(x, y) / 256;
    int bc = noise2dI(y, z) / 256;
    int ac = noise2dI(x, z) / 256;

    int ba = noise2dI(y, x) / 256;
    int cb = noise2dI(z, y) / 256;
    int ca = noise2dI(z, x) / 256;

    int abc = ab + bc + ac + ba + cb + ca;

    return abc / 6;
}

// Function to generate a random number based on current seed
#define MULTIPLIER 1664525
#define INCREMENT 1013904223
#define MULTIPLIER_X 2654435761
#define MULTIPLIER_Y 32452843
#define MULTIPLIER_Z 2654435769

