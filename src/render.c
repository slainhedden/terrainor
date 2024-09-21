#include "render.h"
#include "utils.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <GL/glu.h>

// Define colors struct
typedef struct {
    float r;
    float g;
    float b;
} RGB;

// Define the window dimensions
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

// Declare a global pointer to the GLFW window
static GLFWwindow* window = NULL;

// Interpolated color chooser based on height value
RGB chooseColor(float heightValue){
    RGB color;

    if (heightValue < 0.2f){ // Water (BLUE)
        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 1.0f;
    } else if (heightValue < 0.4f){ // Sand (YELLOW)
        float t = (heightValue - 0.2f) / 0.2f; // Interpolate between water and sand
        color.r = lerp(0.0f, 0.9f, t);
        color.g = lerp(0.0f, 0.8f, t);
        color.b = lerp(1.0f, 0.4f, t);
    } else if (heightValue < 0.6f){ // Grassland (GREEN)
        float t = (heightValue - 0.4f) / 0.2f; // Interpolate between sand and grass
        color.r = lerp(0.9f, 0.0f, t);
        color.g = lerp(0.8f, 1.0f, t);
        color.b = lerp(0.4f, 0.0f, t);
    } else if (heightValue < 0.8f){ // Mountains (GRAY)
        float t = (heightValue - 0.6f) / 0.2f; // Interpolate between grass and mountains
        color.r = lerp(0.0f, 0.5f, t);
        color.g = lerp(1.0f, 0.5f, t);
        color.b = lerp(0.0f, 0.5f, t);
    } else { // Mountain Peaks (WHITE)
        float t = (heightValue - 0.8f) / 0.2f; // Interpolate between mountains and peaks
        color.r = lerp(0.5f, 1.0f, t);
        color.g = lerp(0.5f, 1.0f, t);
        color.b = lerp(0.5f, 1.0f, t);
    }

    return color;
}

// Function to set up basic lighting
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Enable the first light source

    // Define light properties
    GLfloat lightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };  // Light position
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White light

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);  // Diffuse light
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor); // Specular light
}

void initializeGraphics() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Terrain Generator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to create GLFW window\n");
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set up the viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set up perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // Set up the camera position (eye position, look-at position, and up vector)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 10.0f,  // Move the camera further back to see the whole grid
              0.0f, 0.0f, 0.0f,  // Look at the origin
              0.0f, 1.0f, 0.0f); // "Up" is in positive Y direction

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Enable face culling to avoid rendering back faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set up basic lighting
    setupLighting();
}

// 3D Bilinear interpolation between eight corner values (voxels)
float interpolate3D(float v000, float v100, float v010, float v110,
                    float v001, float v101, float v011, float v111,
                    float tx, float ty, float tz) {
    float lerp_x0 = lerp(v000, v100, tx);
    float lerp_x1 = lerp(v010, v110, tx);
    float lerp_x2 = lerp(v001, v101, tx);
    float lerp_x3 = lerp(v011, v111, tx);

    float lerp_y0 = lerp(lerp_x0, lerp_x1, ty);
    float lerp_y1 = lerp(lerp_x2, lerp_x3, ty);

    return lerp(lerp_y0, lerp_y1, tz);
}

void renderTerrain(Terrain* terrain) {
    if (!terrain || !terrain->heights) return;  // Ensure valid data exists before rendering

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cellWidth = 2.0f / (terrain->width - 1);
    float cellHeight = 2.0f / (terrain->height - 1);
    float depthStep = 2.0f / (terrain->depth - 1);

    // Loop through 3D terrain grid, rendering only the visible voxel faces
    for (int z = 0; z < terrain->depth; z++) {
        for (int y = 0; y < terrain->height; y++) {
            for (int x = 0; x < terrain->width; x++) {
                // Current voxel height
                float height = terrain->heights[z * (terrain->width * terrain->height) + y * terrain->width + x];
                RGB color = chooseColor(height);  // Choose a color based on the voxel height

                // Calculate position of the voxel in 3D space
                float xpos = (x * cellWidth) - 1.0f;
                float ypos = (y * cellHeight) - 1.0f;
                float zpos = (z * depthStep) - 1.0f;

                // Check neighboring voxels for face culling
                int leftNeighborExists = (x > 0);
                int rightNeighborExists = (x < terrain->width - 1);
                int topNeighborExists = (y > 0);
                int bottomNeighborExists = (y < terrain->height - 1);
                int frontNeighborExists = (z > 0);
                int backNeighborExists = (z < terrain->depth - 1);

                // Render front face if there is no neighbor in front
                if (!frontNeighborExists || terrain->heights[(z - 1) * (terrain->width * terrain->height) + y * terrain->width + x] == 0) {
                    glBegin(GL_QUADS);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(xpos, ypos, zpos);
                    glVertex3f(xpos + cellWidth, ypos, zpos);
                    glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);
                    glVertex3f(xpos, ypos + cellHeight, zpos);
                    glEnd();
                }

                // Render back face if there is no neighbor in back
                if (!backNeighborExists || terrain->heights[(z + 1) * (terrain->width * terrain->height) + y * terrain->width + x] == 0) {
                    glBegin(GL_QUADS);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(xpos, ypos, zpos + depthStep);
                    glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                    glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);
                    glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                    glEnd();
                }

                // Render left face if there is no neighbor to the left
                if (!leftNeighborExists || terrain->heights[z * (terrain->width * terrain->height) + y * terrain->width + (x - 1)] == 0) {
                    glBegin(GL_QUADS);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(xpos, ypos, zpos);
                    glVertex3f(xpos, ypos + cellHeight, zpos);
                    glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                    glVertex3f(xpos, ypos, zpos + depthStep);
                    glEnd();
                }

                // Render right face if there is no neighbor to the right
                if (!rightNeighborExists || terrain->heights[z * (terrain->width * terrain->height) + y * terrain->width + (x + 1)] == 0) {
                    glBegin(GL_QUADS);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(xpos + cellWidth, ypos, zpos);
                    glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);
                    glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);
                    glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                    glEnd();
                }

                // Render top face if there is no neighbor on top
                if (!topNeighborExists || terrain->heights[z * (terrain->width * terrain->height) + (y - 1) * terrain->width + x] == 0) {
                    glBegin(GL_QUADS);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(xpos, ypos, zpos);
                    glVertex3f(xpos + cellWidth, ypos, zpos);
                    glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                    glVertex3f(xpos, ypos, zpos + depthStep);
                    glEnd();
                }

                // Render bottom face if there is no neighbor below
                if (!bottomNeighborExists || terrain->heights[z * (terrain->width * terrain->height) + (y + 1) * terrain->width + x] == 0) {
                    glBegin(GL_QUADS);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(xpos, ypos + cellHeight, zpos);
                    glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);
                    glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);
                    glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                    glEnd();
                }
            }
        }
    }

    glfwSwapBuffers(window);
}

void startRenderLoop(Terrain* terrain) {
    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers
        renderTerrain(terrain);  // Render the terrain
        glfwPollEvents();  // Poll for events (keyboard, mouse, etc.)
    }
}

void cleanupGraphics() {
    if (window) {
        glfwDestroyWindow(window);
        window = NULL;  // Reset the pointer to avoid dangling references
    }

    // Terminate GLFW
    glfwTerminate();
    
    printf("Graphics cleaned up.\n");
}
