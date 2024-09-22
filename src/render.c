#include "render.h"
#include "utils.h"
#include <GL/gl.h>
#include <stdio.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <math.h>

// Define the window dimensions
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

// Define Voxel Threshold for Culling
#define VOXEL_THRESHOLD 0.1f  // Adjust as necessary

// Define Voxel Size
#define VOXEL_SIZE 1.0f  // Adjust to scale individual voxels

// Declare a global pointer to the GLFW window
static GLFWwindow* window = NULL;

// Camera parameters
float yaw = -90.0f;    // Yaw starts pointing towards negative Z
float pitch = 0.0f;    // Pitch starts at 0
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float fov = 45.0f;

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

// Function to determine if a voxel is occupied based on its y-coordinate and heightmap
bool isVoxelOccupied(int x, int y, int z, Terrain* terrain) {
    if (x < 0 || x >= terrain->width || z < 0 || z >= terrain->depth || y < 0 || y >= terrain->height) {
        return false; // Out of bounds
    }
    // Assume terrain->heights[z * terrain->width + x] gives the maximum y for (x, z)
    return y <= (int)(terrain->heights[z * terrain->width + x] * (terrain->height - 1));
}

// Function to set up basic lighting
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Enable the first light source

    // Define light properties
    GLfloat lightPos[] = { 0.0f, 100.0f, 100.0f, 1.0f };  // Elevated light position
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };    // White light

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
    gluLookAt(50.0f, 50.0f, 100.0f,  // Eye position (x, y, z)
              0.0f, 0.0f, 0.0f,     // Look-at point (center)
              0.0f, 1.0f, 0.0f);    // Up vector

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // Enable face culling to avoid rendering back faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set up basic lighting
    setupLighting();

    // Enable Color Material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Set a distinct clear color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark gray background
}

// Implement the renderFace function
void renderFace(float xpos, float ypos, float zpos, float cellWidth, float cellHeight, float depthStep, RGB color, FaceType face) {
    glBegin(GL_QUADS);
        glColor3f(color.r, color.g, color.b);
        
        switch(face) {
            case FRONT:
                // Front face (towards negative Z)
                glNormal3f(0.0f, 0.0f, -1.0f);  // Normal pointing outwards
                glVertex3f(xpos, ypos, zpos);
                glVertex3f(xpos + cellWidth, ypos, zpos);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);
                glVertex3f(xpos, ypos + cellHeight, zpos);
                break;
            
            case BACK:
                // Back face (towards positive Z)
                glNormal3f(0.0f, 0.0f, 1.0f);
                glVertex3f(xpos, ypos, zpos + depthStep);
                glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);
                glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                break;
            
            case LEFT:
                // Left face (towards negative X)
                glNormal3f(-1.0f, 0.0f, 0.0f);
                glVertex3f(xpos, ypos, zpos);
                glVertex3f(xpos, ypos + cellHeight, zpos);
                glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                glVertex3f(xpos, ypos, zpos + depthStep);
                break;
            
            case RIGHT:
                // Right face (towards positive X)
                glNormal3f(1.0f, 0.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos, zpos);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);
                glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                break;
            
            case TOP:
                // Top face (towards positive Y)
                glNormal3f(0.0f, 1.0f, 0.0f);
                glVertex3f(xpos, ypos, zpos);
                glVertex3f(xpos + cellWidth, ypos, zpos);
                glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                glVertex3f(xpos, ypos, zpos + depthStep);
                break;
            
            case BOTTOM:
                // Bottom face (towards negative Y)
                glNormal3f(0.0f, -1.0f, 0.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);
                glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                break;
        }
    glEnd();
}

// Implement the renderTerrain function
void renderTerrain(Terrain* terrain) {
    if (!terrain || !terrain->heights) return;  // Ensure valid data exists before rendering

    // Calculate voxel dimensions based on VOXEL_SIZE
    float cellWidth = VOXEL_SIZE;
    float cellHeight = VOXEL_SIZE;
    float depthStep = VOXEL_SIZE;

    // Calculate the starting position to center the terrain
    float startX = -(terrain->width / 2.0f) * cellWidth;
    float startY = -(terrain->height / 2.0f) * cellHeight;
    float startZ = -(terrain->depth / 2.0f) * depthStep;

    // Loop through 3D terrain grid, rendering only the visible voxel faces
    for (int z = 0; z < terrain->depth; z++) {
        for (int x = 0; x < terrain->width; x++) {
            // Determine the maximum y for this (x, z) based on heightmap
            int maxY = (int)(terrain->heights[z * terrain->width + x] * (terrain->height - 1));

            for (int y = 0; y <= maxY; y++) {
                // Calculate position of the voxel in 3D space
                float xpos = startX + (x * cellWidth);
                float ypos = startY + (y * cellHeight);
                float zpos = startZ + (z * depthStep);

                // Current voxel height
                float heightValue = terrain->heights[z * terrain->width + x];
                if (!isVoxelOccupied(x, y, z, terrain)) continue;  // Skip rendering if voxel is not occupied

                RGB color = chooseColor(heightValue);  // Choose a color based on the voxel height

                // Check neighboring voxels for face culling
                bool frontNeighborExists = (z > 0) && isVoxelOccupied(x, y, z - 1, terrain);
                bool backNeighborExists = (z < terrain->depth - 1) && isVoxelOccupied(x, y, z + 1, terrain);
                bool leftNeighborExists = (x > 0) && isVoxelOccupied(x - 1, y, z, terrain);
                bool rightNeighborExists = (x < terrain->width - 1) && isVoxelOccupied(x + 1, y, z, terrain);
                bool topNeighborExists = (y < maxY) && isVoxelOccupied(x, y + 1, z, terrain);
                bool bottomNeighborExists = (y > 0) && isVoxelOccupied(x, y - 1, z, terrain);

                // Render faces based on neighbor occupancy
                if (!frontNeighborExists) {
                    renderFace(xpos, ypos, zpos, cellWidth, cellHeight, depthStep, color, FRONT);
                }

                if (!backNeighborExists) {
                    renderFace(xpos, ypos, zpos + depthStep, cellWidth, cellHeight, depthStep, color, BACK);
                }

                if (!leftNeighborExists) {
                    renderFace(xpos, ypos, zpos, cellWidth, cellHeight, depthStep, color, LEFT);
                }

                if (!rightNeighborExists) {
                    renderFace(xpos + cellWidth, ypos, zpos, cellWidth, cellHeight, depthStep, color, RIGHT);
                }

                if (!topNeighborExists) {
                    renderFace(xpos, ypos + cellHeight, zpos, cellWidth, cellHeight, depthStep, color, TOP);
                }

                if (!bottomNeighborExists) {
                    renderFace(xpos, ypos, zpos, cellWidth, cellHeight, depthStep, color, BOTTOM);
                }
            }
        }
    }
}

// Callback function for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;  // Mark as unused

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain the pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

// Callback function for scroll (zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;     // Mark as unused
    (void)xoffset;    // Mark as unused

    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f;

    // Update the projection matrix with the new fov
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Function to update the camera's view based on yaw and pitch
void updateCameraView() {
    // Calculate the new Front vector
    float radYaw = yaw * (M_PI / 180.0f);
    float radPitch = pitch * (M_PI / 180.0f);

    float frontX = cos(radPitch) * cos(radYaw);
    float frontY = sin(radPitch);
    float frontZ = cos(radPitch) * sin(radYaw);

    // Normalize the front vector
    float length = sqrt(frontX * frontX + frontY * frontY + frontZ * frontZ);
    frontX /= length;
    frontY /= length;
    frontZ /= length;

    // Update the view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        frontX * 150.0f, frontY * 150.0f, frontZ * 150.0f,   // Eye position based on front vector
        0.0f, 0.0f, 0.0f,                                 // Look-at point (center)
        0.0f, 1.0f, 0.0f                                  // Up vector
    );
}

// Function to start the rendering loop
void startRenderLoop(Terrain* terrain) {
    // Set up callbacks before the loop
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers

        // Update the camera's view based on input
        updateCameraView();

        renderTerrain(terrain);  // Render the terrain

        glfwSwapBuffers(window);  // Swap front and back buffers
        glfwPollEvents();         // Poll for events (keyboard, mouse, etc.)
    }
}

// Function to clean up graphics resources
void cleanupGraphics() {
    if (window) {
        glfwDestroyWindow(window);
        window = NULL;  // Reset the pointer to avoid dangling references
    }

    // Terminate GLFW
    glfwTerminate();
    
    printf("Graphics cleaned up.\n");
}
