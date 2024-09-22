#ifndef NOISE_H
#define NOISE_H

#define PERM_SIZE 256

void initNoise(); // Function to initialize the permutation table
double perlinNoise2D(double x, double y); // Function to generate Perlin noise
float* generateNoiseMap2D(int width, int depth, int offsetX, int offsetZ, int octaves, float persistence, float lacunarity, float noiseScale);

#endif
