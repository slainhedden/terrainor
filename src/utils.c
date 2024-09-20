// utils.c
#include "utils.h"

float lerp(float a, float b, float t) {
    return a + t * (b - a);  // Linear interpolation
}

float fade(float t) {
    // Quintic fade function for smoother transitions
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float grad(int hash, float x, float y) {
    int h = hash & 15;  // Use 4 bits to get 16 directions
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14) ? x : 0.0f;
    // Corrected the conditions for sign inversion
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


