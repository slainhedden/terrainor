// main.c
#include "terrain.h"
#include "noise.h"
#include <stdio.h>

int main() {
    // Initialize the noise system
    initNoise();
    
    // Create a terrain of 100x100 grid points
    Terrain* terrain = createTerrain(100, 100);
    if (!terrain) {
        printf("Failed to create terrain.\n");
        return 1;
    }

    // Generate the terrain heights using Perlin noise
    generateTerrain(terrain);

    // Print some of the height values to check if it works
    printf("Sample terrain heights:\n");
    for (int y = 0; y < terrain->height; y += 20) {
        for (int x = 0; x < terrain->width; x += 20) {
            printf("%0.2f ", terrain->heights[y * terrain->width + x]);
        }
        printf("\n");
    }

    // Clean up
    destroyTerrain(terrain);
    return 0;
}
