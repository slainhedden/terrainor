#ifndef RENDER_H
#define RENDER_H

#include "terrain.h"  // Include to recognize Terrain type

void initializeGraphics();
void renderTerrain(Terrain* terrain);
void startRenderLoop(Terrain* terrain);
void cleanupGraphics();

#endif

