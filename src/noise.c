#include "utils.h"
#include "noise.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int perm[PERM_SIZE * 2];

void initNoise() {
    srand(12);

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

double perlinNoise3D(double x, double y, double z) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(perm[AA], x, y, z),
                                   grad(perm[BA], x - 1, y, z)),
                           lerp(u, grad(perm[AB], x, y - 1, z),
                                   grad(perm[BB], x - 1, y - 1, z))),
                   lerp(v, lerp(u, grad(perm[AA + 1], x, y, z - 1),
                                   grad(perm[BA + 1], x - 1, y, z - 1)),
                           lerp(u, grad(perm[AB + 1], x, y - 1, z - 1),
                                   grad(perm[BB + 1], x - 1, y - 1, z - 1))));
}

float* generateNoiseMap(int width, int height, int depth, int offsetX, int offsetY, int offsetZ, int octaves, float persistence, float lacunarity, float noiseScale) {
    // Allocate memory for noise values and normalized values
    float* noiseValues = (float*)malloc(width * height * depth * sizeof(float));
    float* normalizedNoiseValues = (float*)malloc(width * height * depth * sizeof(float));

    // Precompute maximum possible height for normalization
    float amp = 1.0f;
    float maxPossibleHeight = 0.0f;
    for (int i = 0; i < octaves; i++) {
        maxPossibleHeight += amp;
        amp *= persistence;
    }

    // Get permutation vector (or random gradients)
    initNoise();  // Call your noise initialization function

    // Iterate through each point on the noise map
    for(int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float amplitude = 1.0f;
                float frequency = 1.0f;
                float noiseHeight = 0.0f;

                // Loop through octaves
                for (int i = 0; i < octaves; i++) {
                    float sampleX = (x + offsetX * (width - 1)) / noiseScale * frequency;
                    float sampleY = (y + offsetY * (height - 1)) / noiseScale * frequency;
                    float sampleZ = (z + offsetZ * (depth - 1)) / noiseScale * frequency;

                    // Get the Perlin noise value
                    float perlinValue = perlinNoise3D(sampleX, sampleY, sampleZ);

                    // Accumulate the noise value for this octave
                    noiseHeight += perlinValue * amplitude;

                    // Update amplitude and frequency for next octave
                    amplitude *= persistence;
                    frequency *= lacunarity;
                }

                // Store the noise value for the current point
                noiseValues[z * (width * height) + y * width + x] = noiseHeight;
            }
        }
    }

    // Normalize the noise values to a 0-1 range
    for (int i = 0; i < width * height * depth; i++) {
        normalizedNoiseValues[i] = (noiseValues[i] + 1) / maxPossibleHeight;
    }

    // Free the unnormalized noise values array
    free(noiseValues);

    // Return the normalized values
    return normalizedNoiseValues;
}

