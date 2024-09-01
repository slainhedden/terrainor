// utils.c
#include "utils.h"

float lerp(float a, float b, float t) {
    return a + t * (b - a);  // Linear interpolation
}

float fade(float t) {
    return t * t * t * ((t * 6 - 15) + 10); // Smooths the interpolation by adjusting how the interpolation progresses between two values
}

float grad(int hash, float x, float y) { 
    int h = hash & 7; // Convert low 3 bits of hash code
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
