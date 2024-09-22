// terrain.c
#include "terrain.h"
#include "noise.h"  // Include noise header for generating heights
#include <stdlib.h>  // For malloc and free
#include <stdio.h>
#include <float.h>   // For FLT_MAX and FLT_MIN

#define MAX_HEIGHT 50.0f  // Define maximum possible height for scaling

// Function to create and initialize a terrain structure with a 2D heightmap
Terrain* createTerrain(int width, int depth) {
    // Allocate memory for the Terrain structure
    Terrain* terrain = (Terrain*)malloc(sizeof(Terrain));
    if (!terrain) {
        return NULL;  // Return NULL if memory allocation fails
    }

    terrain->width = width;
    terrain->depth = depth;

    // Allocate memory for the height map (2D array stored as 1D)
    terrain->heights = (float*)malloc(width * depth * sizeof(float));
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

// Function to generate terrain height data using 2D Perlin noise
void generateTerrain(Terrain* terrain) {
    if (!terrain || !terrain->heights) return;

    int octaves = 6;  
    float persistence = 0.5f;   
    float lacunarity = 1.8f;
    float noiseScale = 70.0f; // Increased scale for broader features

    // Ensure that width and depth are valid
    if (terrain->width <= 0 || terrain->depth <= 0) return;

    // Generate the noise map for the entire terrain grid
    float* noiseMap = generateNoiseMap2D(terrain->width, terrain->depth, 0, 0, octaves, persistence, lacunarity, noiseScale);

    if (!noiseMap) {
        printf("Failed to generate noise map.\n");
        return;
    }

    // Copy generated noise values into the terrain heights array
    for (int z = 0; z < terrain->depth; z++) {
        for (int x = 0; x < terrain->width; x++) {
            terrain->heights[z * terrain->width + x] = noiseMap[z * terrain->width + x];
        }
    }

    // Free the noise map after use
    free(noiseMap);
}

