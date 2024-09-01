#include "noise.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>

int perm[PERM_SIZE * 2];

void initNoise() {
    // Initialize the permutation array with values 0 to PERM_SIZE - 1
    for (int i = 0; i < PERM_SIZE; i++) perm[i] = i;

    // Shuffle the permutation array to ensure random distribution of gradients
    for (int i = 0; i < PERM_SIZE; i++) {
        // Generate a random index j within the range of the permutation size
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
    // Determine the grid cell coordinates that contain the point (x, y)
    // The floor function returns the largest integer less than or equal to x and y.
    // The '& (PERM_SIZE-1)' operation ensures the coordinates wrap around within a 256x256 grid.
    int X = (int)floor(x) & (PERM_SIZE - 1);
    int Y = (int)floor(y) & (PERM_SIZE - 1);
  
    // Calculate the relative x, y positions of (x, y) within its grid cell
    // This is done by subtracting the floor value from x and y, giving a value in [0,1).
    x -= floor(x);
    y -= floor(y);

    // Compute fade curves for x and y
    // The fade function smooths the interpolation curve, providing smoother transitions
    float u = fade(x);
    float v = fade(y);

    // Hash coordinates of the four corners of the cell
    // The permutation table 'perm' is used to shuffle gradient indices
    int aa = perm[perm[X] + Y];         // Hash coordinate of the bottom-left corner
    int ab = perm[perm[X] + Y + 1];     // Hash coordinate of the top-left corner
    int ba = perm[perm[X + 1] + Y];     // Hash coordinate of the bottom-right corner
    int bb = perm[perm[X + 1] + Y + 1]; // Hash coordinate of the top-right corner

    // Calculate the dot product of the gradient vectors with the relative position vectors
    // The grad function returns the dot product of the gradient vector determined by 'perm' with the offset vector (x, y)  
    float gradAA = grad(aa, x, y);       // Bottom-left gradient
    float gradBA = grad(ba, x - 1, y);   // Bottom-right gradient
    float gradAB = grad(ab, x, y - 1);   // Top-left gradient
    float gradBB = grad(bb, x - 1, y - 1); // Top-right gradient

    // Interpolate between the gradient results along the x-axis
    // lerp function linearly interpolates between two values based on the weight 'u'
    float lerpX1 = lerp(u, gradAA, gradBA);  // Interpolation between bottom-left and bottom-right gradients
    float lerpX2 = lerp(u, gradAB, gradBB);  // Interpolation between top-left and top-right gradients

    // Final interpolation along the y-axis
    return lerp(v, lerpX1, lerpX2);
}
