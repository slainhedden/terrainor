// terrain.h
#ifndef TERRAIN_H
#define TERRAIN_H

// Define a structure for Terrain
typedef struct {
    int width;    // Width of the terrain grid (X-axis)
    int height;   // Height of the terrain grid (Y-axis)
    int depth;    // Depth of the terrain grid (Z-axis)
    float* heights;  // Pointer to a 1D array storing height values in a 3D grid
} Terrain;

// Function prototypes
Terrain* createTerrain(int width, int height, int depth);  // Function to create and initialize a 3D terrain
void destroyTerrain(Terrain* terrain);  // Function to free the memory of a terrain
void generateTerrain(Terrain* terrain);  // Function to generate 3D terrain height data using Perlin noise

#endif // TERRAIN_H
