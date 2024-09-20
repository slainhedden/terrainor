#include "render.h"
#include "utils.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// Define colors struct
typedef struct {
    float r;
    float g;
    float b;
} RGB;

// Define the window dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

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

void initializeGraphics() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }
    
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Terrain Generator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to create GLFW window\n");
        return;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Set up the viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderTerrain(Terrain* terrain) {
    if (!terrain || !terrain->heights) return; // Ensure valid data exists before attempting to render
    
    // Clear Graphics before displaying
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate the size of each cell in the window(like a grid on a map)
    float cellWidth = (float)WINDOW_WIDTH / terrain->width;
    float cellHeight = (float)WINDOW_HEIGHT / terrain->height;

    // Render the terrain grid with interpolation
    for(int y = 0; y < terrain->height - 1; y++){
        for(int x = 0; x < terrain->width - 1; x++){
            // Get height values for the four corners of the current cell
            float h1 = terrain->heights[y * terrain->width + x];           // Top-left
            float h2 = terrain->heights[y * terrain->width + (x + 1)];     // Top-right
            float h3 = terrain->heights[(y + 1) * terrain->width + x];     // Bottom-left
            float h4 = terrain->heights[(y + 1) * terrain->width + (x + 1)]; // Bottom-right
            
            // Calculate the position of the current cell (quad)
            float xpos = x * cellWidth;
            float ypos = y * cellHeight;
            
            // Interpolate colors based on heights
            RGB c1 = chooseColor(h1);
            RGB c2 = chooseColor(h2);
            RGB c3 = chooseColor(h3);
            RGB c4 = chooseColor(h4);
            
            // Draw the quad with interpolated vertex colors
            glBegin(GL_QUADS);
                glColor3f(c1.r, c1.g, c1.b); glVertex2f(xpos, ypos);                   // Top-left
                glColor3f(c2.r, c2.g, c2.b); glVertex2f(xpos + cellWidth, ypos);       // Top-right
                glColor3f(c4.r, c4.g, c4.b); glVertex2f(xpos + cellWidth, ypos + cellHeight); // Bottom-right
                glColor3f(c3.r, c3.g, c3.b); glVertex2f(xpos, ypos + cellHeight);      // Bottom-left
            glEnd();
        }
    }
    
    glfwSwapBuffers(window);
}

void startRenderLoop(Terrain* terrain) {
    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
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

