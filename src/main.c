#include "terrain.h"
#include "render.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
    // Create a terrain of 100x100 grid points
    Terrain* terrain = createTerrain(512, 512);

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

    destroyTerrain(terrain);
    return 0;
}

