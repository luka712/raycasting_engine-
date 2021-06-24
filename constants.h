#pragma once

#ifndef CONSTANTS_H

#define CONSTANTS_H

#define PI 3.14159265f
#define TWO_PI (PI * 2.0f)

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

#define MINIMAP_SCALE_FACTOR 0.3f

#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

#define RADIANS(val) (val * (PI / 180.0f))

#define FOV_ANGLE RADIANS(60)

// 1 ray per column
#define NUM_RAYS WINDOW_WIDTH

#define FPS 60
#define MS_PER_FRAME (1000 / FPS)

#endif // !CONSTANTS_H
