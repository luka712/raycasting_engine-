#pragma once 

#ifndef TEXTURES_H

#define TEXTURES_H

#include "defs.h"
#include "upng.h"

struct texture_t 
{
	upng_t* upngTexture;
	color_t* texture_buffer;
	int width;
	int height;
};

extern texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures();
void freeWallTextures();

#endif 