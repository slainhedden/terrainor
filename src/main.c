#include "terrain.h"
#include "noise.h"
#include "render.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
    // Initialize the noise system with a randomized seed
    initNoise();
    
    // Create a terrain of 100x100 grid points
    Terrain* terrain = createTerrain(1000, 1000);
    if (!terrain) {
        printf("Failed to create terrain.\n");
        return 1;
    }

    // Generate the terrain heights using Perlin noise
    generateTerrain(terrain);

    initializeGraphics();
    
    startRenderLoop(terrain);
    
    // Clean up
    cleanupGraphics();
    int histogram[10] = {0};  // 10 bins for ranges [0.0-0.1), [0.1-0.2), ..., [0.9-1.0]

    for (int i = 0; i < terrain->width * terrain->height; i++) {
        float value = terrain->heights[i];
        int bin = (int)(value * 10);
        if (bin >= 10) bin = 9;
        histogram[bin]++;
    }

    // Print the histogram
    for (int i = 0; i < 10; i++) {
        printf("Range [%0.1f - %0.1f): %d\n", i / 10.0f, (i + 1) / 10.0f, histogram[i]);
    }

    destroyTerrain(terrain);
    return 0;
}

