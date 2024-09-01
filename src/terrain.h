// terrain.h
#ifndef TERRAIN_H
#define TERRAIN_H

// Define a structure for Terrain
typedef struct {
    int width;   // Width of the terrain grid
    int height;  // Height of the terrain grid
    float* heights;  // Pointer to an array storing height values
} Terrain;

// Function prototypes
Terrain* createTerrain(int width, int height);  // Function to create and initialize a terrain
void destroyTerrain(Terrain* terrain);  // Function to free the memory of a terrain
void generateTerrain(Terrain* terrain);  // Function to generate terrain height data using noise

#endif // TERRAIN_H
