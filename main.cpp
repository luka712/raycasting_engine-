#define SDL_MAIN_HANDLED

#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"
#include "wall.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>


bool isGameRunning = false;
int ticksLastFrame;

void setup()
{
	loadWallTextures();
}


void processInput() {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT: {
		isGameRunning = false;
		break;
	}
	case SDL_KEYDOWN: {
		if (event.key.keysym.sym == SDLK_ESCAPE)
			isGameRunning = false;
		if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
			player.walkDirection = +1;
		if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
			player.walkDirection = -1;
		if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
			player.turnDirection = +1;
		if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
			player.turnDirection = -1;
		break;
	}
	case SDL_KEYUP: {
		if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
			player.walkDirection = 0;
		if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
			player.walkDirection = 0;
		if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
			player.turnDirection = 0;
		if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
			player.turnDirection = 0;
		break;
	}
	}
}

void update() 
{
	// skip if emscripten, since it's using request animationframe.
#ifndef __EMSCRIPTEN__
	// delay execution until targetet framerate is hit
	int timeToWait = MS_PER_FRAME - (SDL_GetTicks() - ticksLastFrame);

	if (timeToWait > 0 && timeToWait <= MS_PER_FRAME)
	{
		SDL_Delay(timeToWait);
	}
#endif

	// deltatime in seconds.
	float dt = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	ticksLastFrame = SDL_GetTicks();

	movePlayer(dt);
	castAllRays();
}


void render()
{
	clearColorBuffer(0xFF000000);

	renderWallProjection();

	renderMap();
	renderRays();
	renderPlayer();

	renderColorBuffer();
}

void releaseResources()
{
	freeWallTextures();
	destroyWindow();
}

#if __EMSCRIPTEN__
#include <emscripten.h>
void requestAnimationFrameCallback()
{
	if (isGameRunning)
	{
		processInput();
		update();
		render();
	}
}
#endif 


int main() {
	isGameRunning = initializeWindow();

	setup();

#if __EMSCRIPTEN__
	emscripten_set_main_loop(requestAnimationFrameCallback, 0, 1);
#else 
	while (isGameRunning)
	{
		processInput();
		update();
		render();
	}
#endif 

	releaseResources();

	return 0;
}
