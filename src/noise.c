#include "utils.h"
#include "noise.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

int perm[PERM_SIZE * 2];

void initNoise() {
    srand(time(NULL));

    for (int i = 0; i < PERM_SIZE; i++) {
        perm[i] = i;
    }

    for (int i = 0; i < PERM_SIZE; i++) {
        int j = rand() % PERM_SIZE;
        int temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }

    for (int i = 0; i < PERM_SIZE; i++) {
        perm[PERM_SIZE + i] = perm[i];
    }
}

// 2D Perlin Noise function
double perlinNoise2D(double x, double y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    double u = fade(x);
    double v = fade(y);

    int A = perm[X] + Y;
    int AA = perm[A];
    int AB = perm[A + 1];
    int B = perm[X + 1] + Y;
    int BA = perm[B];
    int BB = perm[B + 1];

    return lerp(v, 
                lerp(u, grad(perm[AA], x, y),
                        grad(perm[BA], x - 1, y)),
                lerp(u, grad(perm[AB], x, y - 1),
                        grad(perm[BB], x - 1, y - 1)));
}

// Generate a 2D noise map for the heightmap
float* generateNoiseMap2D(int width, int depth, int offsetX, int offsetZ, int octaves, float persistence, float lacunarity, float noiseScale) {
    // Allocate memory for noise values and normalized values
    float* noiseValues = (float*)malloc(width * depth * sizeof(float));
    float* normalizedNoiseValues = (float*)malloc(width * depth * sizeof(float));
    if (!noiseValues) {
        fprintf(stderr, "Failed to allocate memory for noiseValues.\n");
        return NULL;
    }

    // Precompute maximum possible height for normalization
    float amp = 1.0f;
    float maxPossibleHeight = 0.0f;
    for (int i = 0; i < octaves; i++) {
        maxPossibleHeight += amp;
        amp *= persistence;
    }

    // Initialize noise
    initNoise();  // Initialize permutation array

    // Iterate through each point on the noise map
    for(int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float noiseHeight = 0.0f;

            // Loop through octaves
            for (int i = 0; i < octaves; i++) {
                float sampleX = (x + offsetX) / noiseScale * frequency;
                float sampleZ = (z + offsetZ) / noiseScale * frequency;

                // Get the Perlin noise value
                float perlinValue = (float)perlinNoise2D(sampleX, sampleZ); // Cast to float

                // Accumulate the noise value for this octave
                noiseHeight += perlinValue * amplitude;

                // Update amplitude and frequency for next octave
                amplitude *= persistence;
                frequency *= lacunarity;
            }

            // Store the noise value for the current point
            noiseValues[z * width + x] = noiseHeight;
        }
    }

    // Normalize the noise values to a 0-1 range
    for (int i = 0; i < width * depth; i++) {
        normalizedNoiseValues[i] = (noiseValues[i] + maxPossibleHeight) / (2 * maxPossibleHeight);
        // Clamp to [0, 1]
        if (normalizedNoiseValues[i] < 0.0f) normalizedNoiseValues[i] = 0.0f;
        if (normalizedNoiseValues[i] > 1.0f) normalizedNoiseValues[i] = 1.0f;
    }

    // Free the unnormalized noise values array
    free(noiseValues);

    // Return the normalized values
    return normalizedNoiseValues;
}
