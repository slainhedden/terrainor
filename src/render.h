// render.h

#ifndef RENDER_H
#define RENDER_H

#include "terrain.h"  // Include to recognize Terrain type
#include <GLFW/glfw3.h>

// Define colors struct
typedef struct {
    float r;
    float g;
    float b;
} RGB;

// Define face types for clarity
typedef enum {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
} FaceType;

void setupLighting();
void initializeGraphics();
void renderTerrain(Terrain* terrain);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateCamera();
void startRenderLoop(Terrain* terrain);
void cleanupGraphics();

// Declaration of the renderFace function
void renderFace(float xpos, float ypos, float zpos, float cellWidth, float cellHeight, float depthStep, RGB color, FaceType face);

#endif

