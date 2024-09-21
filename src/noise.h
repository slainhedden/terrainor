#ifndef NOISE_H
#define NOISE_H

#define PERM_SIZE 256

void initNoise(); // Function to initialize the permutation table
double perlinNoise3D(double x, double y, double z); // Function to generate Perlin noise
float* generateNoiseMap(int width, int height, int depth, int offsetX, int offsetY, int offsetZ, int octaves, float persistence, float lacunarity, float noiseScale);

#endif
