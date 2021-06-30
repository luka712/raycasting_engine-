#pragma once

#ifndef MAP_H

#define MAP_H

#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

bool mapHasWallAt(float x, float y);
void renderMap(void);
int getMapAt(int i, int j);
bool isInsideMap(int x, int y);

#endif // !MAP_H
