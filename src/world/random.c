/* src/world/random.c */
/* src/world/random.c - Random number generators and quicksort */
#include "engine.h"
#include "chunk_constants.h"

unsigned int generate_random(unsigned int seed, int range) {
    unsigned int a = 1103515245;
    unsigned int c = 12345;
    unsigned int m = 1U << 31;  // Modulus, typically a large prime or power of 2

    // Calculate the next pseudo-random number using LCG
    seed = (a * seed + c) % m;

    // Map the generated number to the specified range
    return seed % (unsigned int)range;
}
unsigned int generate_random1D(unsigned int seed, int range, int x) {
    int seed_ = seed + x * MULTIPLIER_X;

    int seedOut = (seed_ * MULTIPLIER + INCREMENT) % range;
    return seedOut;
}
unsigned int generate_random2D(unsigned int seed, int range, int x, int y) {
    unsigned int seed_ = seed + (x * MULTIPLIER_X * MULTIPLIER_Y) * (y * MULTIPLIER_Y * MULTIPLIER_Z);

    int seedOut = (seed_ * MULTIPLIER + INCREMENT) % range;
    return seedOut;
}
unsigned int generate_random2DO(unsigned int seed, int range, int x, int y) {
    int seed_ = seed;

    seed_ = seed + x * 31 + y * 71;

    int seedOut = (seed_ * MULTIPLIER + INCREMENT) % range;
    return seedOut;
}
unsigned int generate_random3D(unsigned int seed, int range, int x, int y, int z) {
    unsigned int seed_ = seed + (x * MULTIPLIER_X) * (y * MULTIPLIER_Y) * (z * MULTIPLIER_Z);

    unsigned int seedOut = (seed_ * MULTIPLIER + INCREMENT) % range;
    return seedOut;
}
unsigned int rotl(unsigned int value, int shift) {
    return (value << shift) | (value >> (32 - shift));
}
unsigned int trulyRandom2D(unsigned int seed, int range, int x, int y) {
    // XOR-shift scrambling
	seed = seed ^ (x * 2654435761u) ^ (y * 805306457u);
    x = x ^ 362437;
    y = y ^ 521288629;

    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);

    x ^= rotl(seed, 7);
    y ^= rotl(x, 11);
    
    unsigned int random_number = (x * 1664525u + y * 1013904223u) ^ seed;
    
    return random_number % range;
}

void Quick_SortInt(int *arr, int left, int right) 
{
    if (left < right)
    {
        int pivot = PartitionInt(arr, left, right);

        if (pivot > 1)
        Quick_SortInt(arr, left, pivot - 1);

        if (pivot + 1 < right)
        Quick_SortInt(arr, pivot + 1, right);
    }
}
