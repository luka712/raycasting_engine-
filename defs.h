#pragma once

#ifndef DEFS_H

#define DEFS_H

#include <stdint.h>

#define PI 3.14159265f
#define TWO_PI (PI * 2.0f)

#define TILE_SIZE 64
#define NUM_TEXTURES 9

#define MINIMAP_SCALE_FACTOR 0.1f

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 448

#define RADIANS(val) (val * (PI / 180.0f))

#define FOV_ANGLE RADIANS(60)

// 1 ray per column
#define NUM_RAYS WINDOW_WIDTH

#define DIST_PROJ_PLANE ((WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2))

#define FPS 60
#define MS_PER_FRAME (1000 / FPS)

typedef uint32_t color_t;

#endif // !CONSTANTS_H
