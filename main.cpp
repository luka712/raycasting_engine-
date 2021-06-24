#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL.h>
#include "constants.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] =
{
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

struct Player
{
	float x;
	float y;
	float width;
	float height;

	float rotationAngle;
	float walkSpeed;
	float turnSpeed;

	int turnDirection; // -1 for left, +1 for right
	int walkDirection;  // -1 for backwad, +1 forwards
} player;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool isGameRunning = false;
int ticksLastFrame = 0;

bool initializeWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);

	if (!window)
	{
		std::cout << "Error creating SDL window:" << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer)
	{
		std::cout << "Error creating SDL renderer." << SDL_GetError() << std::endl;
		return false;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return true;
}

void destroyWindow()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void setup()
{
	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.width = 1;
	player.height = 1;

	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngle = PI / 2;

	player.walkSpeed = 100;
	player.turnSpeed = RADIANS(45);
}

bool mapHasWallAt(float x, float y)
{
	if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
	{
		return true;
	}

	int mapGridIndexX = floor(x / TILE_SIZE);
	int mapGridIndexY = floor(y / TILE_SIZE);

	bool result = map[mapGridIndexY][mapGridIndexX] != 0;
	return result;
}

void movePlayer(float dt)
{
	player.rotationAngle += player.turnDirection * player.turnSpeed * dt;

	float moveStep = player.walkDirection * player.walkSpeed * dt;

	float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
	float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

	if (!mapHasWallAt(newPlayerX, newPlayerY)) {
		player.x = newPlayerX;
		player.y = newPlayerY;
	}


}

void renderPlayer()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	int playerX = static_cast<int>(player.x * MINIMAP_SCALE_FACTOR);
	int playerY = static_cast<int>(player.y * MINIMAP_SCALE_FACTOR);
	int width = static_cast<int>(player.width * MINIMAP_SCALE_FACTOR);
	int height = static_cast<int>(player.height * MINIMAP_SCALE_FACTOR);

	SDL_Rect playerRect =
	{
		playerX, playerY, width, height
	};

	SDL_RenderFillRect(renderer, &playerRect);

	int lineEndX = playerX + cos(player.rotationAngle) * 40;
	int lineEndY = playerY + sin(player.rotationAngle) * 40;

	SDL_RenderDrawLine(renderer,
		playerX,
		playerY,
		lineEndX,
		lineEndY
	);
}

void renderMap()
{
	for (int i = 0; i < MAP_NUM_ROWS; i++)
	{
		for (int j = 0; j < MAP_NUM_COLS; j++)
		{
			int tileX = j * TILE_SIZE;
			int tileY = i * TILE_SIZE;

			// while or black, depending on tile being 1 or 0
			int tileColor = map[i][j] != 0 ? 255 : 0;

			SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);

			SDL_Rect mapTileRect = {
				static_cast<int>(tileX * MINIMAP_SCALE_FACTOR),
				static_cast<int>(tileY * MINIMAP_SCALE_FACTOR),
				static_cast<int>(TILE_SIZE * MINIMAP_SCALE_FACTOR + 1),
				static_cast<int>(TILE_SIZE * MINIMAP_SCALE_FACTOR + 1) };
			SDL_RenderFillRect(renderer, &mapTileRect);
		}
	}
}

void processInput()
{
	SDL_Event evt;
	SDL_PollEvent(&evt);
	switch (evt.type)
	{
	case SDL_QUIT:
		isGameRunning = false;
		break;
	case SDL_KEYDOWN:
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			isGameRunning = false;
		}
		if (evt.key.keysym.sym == SDLK_UP) {
			player.walkDirection = 1;
		}
		if (evt.key.keysym.sym == SDLK_DOWN) {
			player.walkDirection = -1;
		}
		if (evt.key.keysym.sym == SDLK_RIGHT) {
			player.turnDirection = 1;
		}
		if (evt.key.keysym.sym == SDLK_LEFT) {
			player.turnDirection = -1;
		}

		break;
	case SDL_KEYUP:
		if (evt.key.keysym.sym == SDLK_UP) {
			player.walkDirection = 0;
		}
		if (evt.key.keysym.sym == SDLK_DOWN) {
			player.walkDirection = 0;
		}
		if (evt.key.keysym.sym == SDLK_RIGHT) {
			player.turnDirection = 0;
		}
		if (evt.key.keysym.sym == SDLK_LEFT) {
			player.turnDirection = 0;
		}
		break;
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
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	renderMap();
	// renderRays();
	renderPlayer();

	SDL_RenderPresent(renderer);
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

int main()
{
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

	destroyWindow();

	return 0;
}