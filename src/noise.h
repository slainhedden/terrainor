#ifndef NOISE_H
#define NOISE_H

#define PERM_SIZE 256

void initNoise(); // Function to initialize the permutation table
float perlinNoise2D(float x, float y); // Function to generate Perlin noise
float octavePerlinNoise2D(float x, float y, int octaves, float persistence); // Function for multi-octave - mountains/valleys

#endif
