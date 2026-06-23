#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define DISPLAY_WIDTH 	320
#define DISPLAY_HEIGHT 	528

#define BIT_16  0x10000

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a number */
#define inDisplay(x, y) ((x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT)? 1, 0) /* macro to check if in display */

#define MAP_HEIGHT  32
#define MAP_WIDTH   32

#define GRID_HEIGHT  BIT_16
#define GRID_WIDTH   BIT_16

#define TILE_SIZE   10

#define PLAYER_SIZE 5
#define PLAYER_FRONT_OFFSET 1000
#define PLAYER_SENSITIVITY 10
#define PLAYER_FOV  60
#define PLAYER_MAX_VELOCITY 300

#define TOP_OFFSET_3D (MAP_HEIGHT * TILE_SIZE)
#define HEIGHT_3D 208

#define WALL_HEIGHT_CONSTANT  560000

#define TEXTURE_RES 32
#define TEXTURE_COUNT 2

#define SPRITE_MAX_COUNT 40
#define SPRITE_TEXTURE_COUNT 1
#define SPRITE_TEXTURE_RES 64

#define CEILING_COLOR 0x6E7F
#define FLOOR_COLOR   0x6620

#define BRAD_PI 0x4000
#define OCTANTIFY(_x, _y, _o)   do {                            \
    int _t; _o= 0;                                              \
    if(_y<  0)  {            _x= -_x;   _y= -_y; _o += 4; }     \
    if(_x<= 0)  { _t= _x;    _x=  _y;   _y= -_t; _o += 2; }     \
    if(_x<=_y)  { _t= _y-_x; _x= _x+_y; _y=  _t; _o += 1; }     \
} while(0);


#endif