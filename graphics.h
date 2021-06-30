#pragma once
#include <cstdint>
#include "defs.h"

#ifndef GRAPHICS_H

#define GRAPHICS_H

bool initializeWindow(void);
void destroyWindow(void);
void clearColorBuffer(color_t color);
void renderColorBuffer(void);
void drawPixel(int x, int y, color_t color);
void drawRect(int x, int y, int w, int h, color_t color);
void drawLine(int x0, int y0, int x1, int y1, color_t color);

#endif // !GRAPHICS_H
