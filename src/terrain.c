#include "terrain.h"
#include "noise.h"  // Include noise header for generating heights
#include <stdlib.h>  // For malloc and free
#include <math.h>
#include <stdio.h>
#include <float.h>   // For FLT_MAX and FLT_MIN

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
    if (!terrain || !terrain->heights) return;

    int octaves = 6;  // Increase number of octaves for more detail
    float persistence = 0.7f;
    float frequency = 1.0f;

    float minVal = FLT_MAX;
    float maxVal = -FLT_MAX;

    // Generate noise values with increased frequency scaling
    for (int y = 0; y < terrain->height; y++) {
        for (int x = 0; x < terrain->width; x++) {
            float nx = (float)x / terrain->width;
            float ny = (float)y / terrain->height;
            float noiseValue = octavePerlinNoise2D(nx, ny, octaves, persistence);  // Update frequency each octave
            terrain->heights[y * terrain->width + x] = noiseValue;

            if (noiseValue < minVal) minVal = noiseValue;
            if (noiseValue > maxVal) maxVal = noiseValue;
        }
    }

    // Rescale values between 0 and 1 after noise generation
    for (int y = 0; y < terrain->height; y++) {
        for (int x = 0; x < terrain->width; x++) {
            float noiseValue = terrain->heights[y * terrain->width + x];
            terrain->heights[y * terrain->width + x] = (noiseValue - minVal) / (maxVal - minVal);
        }
    }
}



   
