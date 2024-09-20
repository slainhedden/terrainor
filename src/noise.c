#include "noise.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG_DETAIL 0

int perm[PERM_SIZE * 2];

void initNoise() {
    // Seed the random number generator for consistent results during debugging
    srand(time(NULL));

    // Initialize the permutation array with values 0 to PERM_SIZE - 1
    for (int i = 0; i < PERM_SIZE; i++) {
        perm[i] = i;
    }

    // Shuffle the permutation array to ensure random distribution of gradients
    for (int i = 0; i < PERM_SIZE; i++) {
        int j = rand() % PERM_SIZE;

        // Swap the values at indices i and j to shuffle the array
        int temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }
  
    // Duplicate the permutation array to avoid overflow when accessing perm[X + 1] or perm[Y + 1]
    for (int i = 0; i < PERM_SIZE; i++) {
        perm[PERM_SIZE + i] = perm[i];
    }
}

float perlinNoise2D(float x, float y) {
    int X = (int)floor(x) & (PERM_SIZE - 1);
    int Y = (int)floor(y) & (PERM_SIZE - 1);
  
    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int aa = perm[perm[X] + Y];
    int ab = perm[perm[X] + Y + 1];
    int ba = perm[perm[X + 1] + Y];
    int bb = perm[perm[X + 1] + Y + 1];

    float gradAA = grad(aa, x, y);
    float gradBA = grad(ba, x - 1, y);
    float gradAB = grad(ab, x, y - 1);
    float gradBB = grad(bb, x - 1, y - 1);

    // Interpolate across the x-axis first
    float lerpX1 = lerp(u, gradAA, gradBA);
    float lerpX2 = lerp(u, gradAB, gradBB);

    // Final interpolation across the y-axis
    float noiseValue = lerp(v, lerpX1, lerpX2);

    // NO normalization here! We handle this in octavePerlinNoise2D
    return noiseValue;
}

float octavePerlinNoise2D(float x, float y, int octaves, float persistence) {
    float total = 0.0f;
    float maxAmplitude = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;

    // Iterate through octaves
    for (int i = 0; i < octaves; i++) {
        // Calculate noise for this octave
        float noiseValue = perlinNoise2D(x * frequency, y * frequency);
    
        // Accumulate noise value and amplitude
        total += noiseValue * amplitude;
        maxAmplitude += amplitude;
    
        // Adjust frequency and amplitude for next octave
        amplitude *= persistence;
        frequency *= 0.1f;
    }

    // Normalize final total by maxAmplitude
    float result = total / maxAmplitude;
    // Clamp the result to [-1, 1]
    if (result < -1.0f) result = -1.0f;
    if (result > 1.0f) result = 1.0f;

    return result;
}



