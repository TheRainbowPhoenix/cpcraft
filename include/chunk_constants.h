/* include/chunk_constants.h
 * Constants used by chunk/world functions.
 * These are #defines (not const) because they're used in array sizes.
 * Include this ONLY in .c files that need it (not in engine.h) to avoid
 * polluting the global namespace.
 */
#pragma once

#define TERRAINHEIGHT 0
#define HEIGHT 40
#define WIDTH 12
#define TOTALCHUNKWIDTH 8

#define MULTIPLIER 1664525
#define INCREMENT 1013904223
#define MULTIPLIER_X 2654435761
#define MULTIPLIER_Y 32452843
#define MULTIPLIER_Z 2654435769

/* World block data array */
extern unsigned char blocks[TOTALCHUNKWIDTH * TOTALCHUNKWIDTH]
                           [WIDTH * HEIGHT * WIDTH];

extern int chunkXMapRedstone;
extern int chunkZMapRedstone;
extern int cycle;