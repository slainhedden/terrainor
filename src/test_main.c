#include "noise.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

int main() {
    initNoise();  // Initialize the noise system (random permutation vector, etc.)

    int gridSize = 16;  // A manageable size for debugging
    int depthSize = 4;  // Add depth to traverse multiple slices in Z-direction
    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;
    float sum = 0.0f;

    // Loop through different Z-slices
    for (int z = 0; z < depthSize; z++) {
        float z_value = (float)z / depthSize;  // Normalize Z value for each slice

        printf("Noise Values (slice at z = %f):\n", z_value);

        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                float nx = (float)x / gridSize;
                float ny = (float)y / gridSize;
                float nz = z_value;  // Current slice in the Z dimension

                // Generate 3D noise
                float noiseValue = perlinNoise3D(nx, ny, nz);

                // Clamp to [0, 1] for easier visualization
                noiseValue = (noiseValue + 1.0f) / 2.0f;

                // Print noise value for this point
                printf("%0.2f ", noiseValue);

                // Track min, max, and average
                if (noiseValue < minValue) minValue = noiseValue;
                if (noiseValue > maxValue) maxValue = noiseValue;
                sum += noiseValue;
            }
            printf("\n");
        }

        printf("\n");  // Separate slices for clarity
    }

    float averageValue = sum / (gridSize * gridSize * depthSize);
    printf("Noise Statistics:\n");
    printf("Min Value: %0.4f\n", minValue);
    printf("Max Value: %0.4f\n", maxValue);
    printf("Average Value: %0.4f\n", averageValue);

    return 0;
}

