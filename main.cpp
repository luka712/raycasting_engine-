#define SDL_MAIN_HANDLED

#include <iostream>
#include <limits>
#include <SDL2/SDL.h>
#include "constants.h"
#include "textures.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] =
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
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

struct Ray
{
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;
	int wasHitVertical;
	int isRayFacingUp;
	int isRayFacingDown;
	int isRayFacingLeft;
	int isRayFacingRight;
	int wallHitContent;
} rays[NUM_RAYS];

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool isGameRunning = false;
int ticksLastFrame = 0;

Uint32* colorBuffer = nullptr;

SDL_Texture* colorBufferTexture;

Uint32* textures[NUM_TEXTURES];

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
	free(colorBuffer);
	SDL_DestroyTexture(colorBufferTexture);
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

	// allocate bytes for color buffer.
	colorBuffer = static_cast<Uint32*>(malloc(static_cast<Uint32>(WINDOW_WIDTH) * static_cast<Uint32>(WINDOW_HEIGHT) * sizeof(Uint32)));

	// Create SDL_Texture to display color buffer.
	colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

	textures[0] = (Uint32*)REDBRICK_TEXTURE;
	textures[1] = (Uint32*)PURPLESTONE_TEXTURE;
	textures[2] = (Uint32*)MOSSYSTONE_TEXTURE;
	textures[3] = (Uint32*)GRAYSTONE_TEXTURE;
	textures[4] = (Uint32*)COLORSTONE_TEXTURE;
	textures[5] = (Uint32*)BLUESTONE_TEXTURE;
	textures[6] = (Uint32*)WOOD_TEXTURE;
	textures[7] = (Uint32*)EAGLE_TEXTURE;
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

float normalizeAngle(float angle)
{
	angle = remainder(angle, TWO_PI);

	// If negative, move it to positive range
	if (angle < 0)
	{
		angle = TWO_PI + angle;
	}

	return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2)
{
	float result = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	return result;
}

void castRay(float rayAngle, int stripId)
{
	rayAngle = normalizeAngle(rayAngle);

	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;

	int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
	int isRayFacingLeft = !isRayFacingRight;

	float xintercept, yintercept;
	float xstep, ystep;

	///////////////////////////////////////////
	// HORIZONTAL RAY-GRID INTERSECTION CODE
	///////////////////////////////////////////
	int foundHorzWallHit = false;
	float horzWallHitX = 0;
	float horzWallHitY = 0;
	int horzWallContent = 0;

	// Find the y-coordinate of the closest horizontal grid intersection
	yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yintercept += isRayFacingDown ? TILE_SIZE : 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

	// Calculate the increment xstep and ystep
	ystep = TILE_SIZE;
	ystep *= isRayFacingUp ? -1 : 1;

	xstep = TILE_SIZE / tan(rayAngle);
	xstep *= (isRayFacingLeft && xstep > 0) ? -1 : 1;
	xstep *= (isRayFacingRight && xstep < 0) ? -1 : 1;

	float nextHorzTouchX = xintercept;
	float nextHorzTouchY = yintercept;

	// Increment xstep and ystep until we find a wall
	while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH && nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT) {
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

		if (mapHasWallAt(xToCheck, yToCheck)) {
			// found a wall hit
			horzWallHitX = nextHorzTouchX;
			horzWallHitY = nextHorzTouchY;
			horzWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
			foundHorzWallHit = true;
			break;
		}
		else {
			nextHorzTouchX += xstep;
			nextHorzTouchY += ystep;
		}
	}

	///////////////////////////////////////////
	// VERTICAL RAY-GRID INTERSECTION CODE
	///////////////////////////////////////////
	int foundVertWallHit = false;
	float vertWallHitX = 0;
	float vertWallHitY = 0;
	int vertWallContent = 0;

	// Find the x-coordinate of the closest vertical grid intersection
	xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xintercept += isRayFacingRight ? TILE_SIZE : 0;

	// Find the y-coordinate of the closest vertical grid intersection
	yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

	// Calculate the increment xstep and ystep
	xstep = TILE_SIZE;
	xstep *= isRayFacingLeft ? -1 : 1;

	ystep = TILE_SIZE * tan(rayAngle);
	ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
	ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

	float nextVertTouchX = xintercept;
	float nextVertTouchY = yintercept;

	// Increment xstep and ystep until we find a wall
	while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH && nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) {
		float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
		float yToCheck = nextVertTouchY;

		if (mapHasWallAt(xToCheck, yToCheck)) {
			// found a wall hit
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			vertWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
			foundVertWallHit = true;
			break;
		}
		else {
			nextVertTouchX += xstep;
			nextVertTouchY += ystep;
		}
	}

	// Calculate both horizontal and vertical hit distances and choose the smallest one
	float horzHitDistance = foundHorzWallHit
		? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
		: WINDOW_WIDTH;
	float vertHitDistance = foundVertWallHit
		? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
		: WINDOW_HEIGHT;

	if (vertHitDistance < horzHitDistance) {
		rays[stripId].distance = vertHitDistance;
		rays[stripId].wallHitX = vertWallHitX;
		rays[stripId].wallHitY = vertWallHitY;
		rays[stripId].wallHitContent = vertWallContent;
		rays[stripId].wasHitVertical = true;
	}
	else {
		rays[stripId].distance = horzHitDistance;
		rays[stripId].wallHitX = horzWallHitX;
		rays[stripId].wallHitY = horzWallHitY;
		rays[stripId].wallHitContent = horzWallContent;
		rays[stripId].wasHitVertical = false;
	}
	rays[stripId].rayAngle = rayAngle;
	rays[stripId].isRayFacingDown = isRayFacingDown;
	rays[stripId].isRayFacingUp = isRayFacingUp;
	rays[stripId].isRayFacingLeft = isRayFacingLeft;
	rays[stripId].isRayFacingRight = isRayFacingRight;
}

void castAllRays()
{
	// start first ray subtracting half of our FOV
	float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

	for (int stripdId = 0; stripdId < NUM_RAYS; stripdId++)
	{
		castRay(rayAngle, stripdId);

		rayAngle += FOV_ANGLE / NUM_RAYS;
	}
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

void renderRays()
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int i = 0; i < NUM_RAYS; i++)
	{
		SDL_RenderDrawLine(
			renderer,
			player.x * MINIMAP_SCALE_FACTOR,
			player.y * MINIMAP_SCALE_FACTOR,
			rays[i].wallHitX * MINIMAP_SCALE_FACTOR,
			rays[i].wallHitY * MINIMAP_SCALE_FACTOR
		);
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
	castAllRays();
}

void generate3DProjection()
{
	for (int i = 0; i < NUM_RAYS; i++) {
		float perpDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);
		float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
		float projectedWallHeight = (TILE_SIZE / perpDistance) * distanceProjPlane;

		int wallStripHeight = (int)projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
		wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
		wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

		// set the color of the ceiling
		for (int y = 0; y < wallTopPixel; y++)
		{
			colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFF444444;
		}

		int textureOffsetX;
		if (rays[i].wasHitVertical)
		{
			textureOffsetX = static_cast<int>(rays[i].wallHitY) % TILE_SIZE;
		}
		else
		{
			textureOffsetX = static_cast<int>(rays[i].wallHitX) % TILE_SIZE;
		}

		// Get the correct texture id number from the map content.
		int texNum = rays[i].wallHitContent - 1;

		// render the wall from wallTopPixel to wallBottomPixel
		for (int y = wallTopPixel; y < wallBottomPixel; y++) 
		{
			int distanceFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
			int textureOffsetY = distanceFromTop * (static_cast<float>(TEXTURE_HEIGHT) / wallStripHeight);

			// set the color of the wall based on the color from the texture.
			Uint32 texelColor = textures[texNum][(TEXTURE_WIDTH * textureOffsetY) + textureOffsetX];
			colorBuffer[(WINDOW_WIDTH * y) + i] = texelColor;
		}

		// Set the color of the floor
		for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
		{
			colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFF888888;
		}
		
	}
}

void clearColorBuffer(Uint32 color)
{
	for (int x = 0; x < WINDOW_WIDTH; x++)
	{
		for (int y = 0; y < WINDOW_HEIGHT; y++)
		{
			colorBuffer[(WINDOW_WIDTH * y) + x] = color;
		}
	}
}

void renderColorBuffer()
{
	SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, static_cast<int>(sizeof(Uint32) * WINDOW_WIDTH));

	SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	generate3DProjection();

	renderColorBuffer();
	clearColorBuffer(0xFF000000);

	renderMap();
	renderRays();
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