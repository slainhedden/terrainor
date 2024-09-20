#include "noise.h"
#include <stdio.h>
#include <math.h>  // Include math.h for fminf and fmaxf
#include <float.h>  // Include float.h for FLT_MAX and FLT_MIN

// Declare the getTerrainChar function
char getTerrainChar(float noiseValue) {
    if (noiseValue < 0.2f) return '~';  // Water
    if (noiseValue < 0.4f) return '.';  // Sand
    if (noiseValue < 0.6f) return '^';  // Grassland
    if (noiseValue < 0.8f) return '#';  // Mountain
    return '@';  // Peak
}

int main() {
    initNoise();  // Initialize the noise

    int gridSize = 20;  // Increase the grid size for more detailed testing
    float minValue = FLT_MAX;
    float maxValue = FLT_MIN;
    float sum = 0.0f;

    printf("Noise Values:\n");
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            float nx = (float)x / (float)gridSize;  // Normalize coordinates
            float ny = (float)y / (float)gridSize;
            float noiseValue = octavePerlinNoise2D(nx, ny, 4, 0.5f);
            
            // Clamp to [0, 1]
            noiseValue = fminf(fmaxf(noiseValue, 0.0f), 1.0f);

            // Print raw noise value for debugging

            // Update min and max values
            if (noiseValue < minValue) {
                minValue = noiseValue;
               // printf("Updated minValue: %f\n", minValue);  // Debugging
            }
            if (noiseValue > maxValue) {
                maxValue = noiseValue;
                // printf("Updated maxValue: %f\n", maxValue);  // Debugging
            }
            // Accumulate sum for average calculation
            sum += noiseValue;

            // Print terrain symbol
            printf("%c ", getTerrainChar(noiseValue));
        }
        printf("\n");
    }

    // Calculate average noise value
    float averageValue = sum / (gridSize * gridSize);

    // Print statistics
    printf("\nNoise Statistics:\n");
    printf("Min Value: %0.4f\n", minValue);
    printf("Max Value: %0.4f\n", maxValue);
    printf("Average Value: %0.4f\n", averageValue);

    return 0;
}


