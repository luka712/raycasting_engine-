#include <SDL2/SDL.h>
#include <cstdio>
#include "graphics.h"

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static color_t* colorBuffer = nullptr;
static SDL_Texture* colorBufferTexture = nullptr;


bool initializeWindow(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	int fullScreenWidth = display_mode.w;
	int fullScreenHeight = display_mode.h;
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		 1280, // fullScreenWidth,
		 720, // fullScreenHeight,
		SDL_WINDOW_BORDERLESS
	);
	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// allocate the total amount of bytes in memory to hold our colorbuffer
	colorBuffer = (color_t*)malloc(sizeof(color_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

	// create an SDL_Texture to display the colorbuffer
	colorBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	return true;
}

void destroyWindow(void)
{
	free(colorBuffer);
	SDL_DestroyTexture(colorBufferTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void clearColorBuffer(color_t color)
{
	for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
	{
		colorBuffer[i] = color;
	}
}


void renderColorBuffer(void)
{
	SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, (int)(WINDOW_WIDTH * sizeof(color_t)));
	SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y, color_t color)
{
	colorBuffer[(y * WINDOW_WIDTH) + x] = color;
}

void drawRect(int x, int y, int w, int h, color_t color)
{
	for (int i = x; i <= (x + w); i++)
	{
		for (int j = y; j <= (y + h); j++)
		{
			drawPixel(i, j, color);
		}
	}
}

void drawLine(int x0, int y0, int x1, int y1, color_t color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	int len = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

	float x_increment = dx / static_cast<float>(len);
	float y_increment = dy / static_cast<float>(len);

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i < len; i++)
	{
		drawPixel(round(current_x), round(current_y), color);
		current_x += x_increment;
		current_y += y_increment;
	}
}