// terrain.c
#include "terrain.h"
#include "noise.h"  // Include noise header for generating heights
#include <stdlib.h>  // For malloc and free

// Function to create and initialize a terrain structure
Terrain* createTerrain(int width, int height) {
    // Allocate memory for the Terrain structure
    Terrain* terrain = (Terrain*)malloc(sizeof(Terrain));
    if (!terrain) {
        return NULL;  // Return NULL if memory allocation fails
    }

    terrain->width = width;
    terrain->height = height;
    
    // Allocate memory for the height map (2D array stored as 1D)
    terrain->heights = (float*)malloc(width * height * sizeof(float));
    if (!terrain->heights) {
        free(terrain);  // Free terrain memory if height allocation fails
        return NULL;
    }

    return terrain;
}

// Function to free the memory allocated for a terrain
void destroyTerrain(Terrain* terrain) {
    if (terrain) {
        free(terrain->heights);  // Free the height map array
        free(terrain);  // Free the Terrain structure itself
    }
}

// Function to generate terrain height data using Perlin noise
void generateTerrain(Terrain* terrain) {
    if (!terrain || !terrain->heights) return;  // Check if terrain is valid

    // Loop through each grid point in the terrain
    for (int y = 0; y < terrain->height; y++) {
        for (int x = 0; x < terrain->width; x++) {
            // Generate height using Perlin noise function from noise.h
            float nx = (float)x / terrain->width;  // Normalize x coordinate
            float ny = (float)y / terrain->height; // Normalize y coordinate
            terrain->heights[y * terrain->width + x] = perlinNoise2D(nx, ny);
        }
    }
}
