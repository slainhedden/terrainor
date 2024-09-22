// terrain.h
#ifndef TERRAIN_H
#define TERRAIN_H

typedef struct {
    int width;
    int depth;    // Renamed from 'height' to 'depth' for clarity
    // int height; // Removed or set to 1 if necessary
    float* heights; // 2D heightmap stored as 1D array
} Terrain;

// Function declarations
Terrain* createTerrain(int width, int depth);
void destroyTerrain(Terrain* terrain);
void generateTerrain(Terrain* terrain);

#endif // TERRAIN_H

