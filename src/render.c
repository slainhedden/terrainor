#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include "render.h"
#include "stb_image.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Define the window dimensions
#define WINDOW_WIDTH 2048
#define WINDOW_HEIGHT 2048

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

// Texture IDs
GLuint textureWater, textureSand, textureGrass, textureMountain, textureSnow;

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

// Function to load a texture using stb_image
GLuint loadTexture(const char* filename) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip vertically to match OpenGL's coordinate system
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        fprintf(stderr, "Failed to load texture: %s\n", filename);
        return 0;
    }

    printf("Loaded texture: %s (Width: %d, Height: %d, Channels: %d)\n", filename, width, height, nrChannels);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Repeat horizontally
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// Repeat vertically
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               // Magnification

    // Determine the image format
    GLenum format;
    if (nrChannels == 1) {
        format = GL_RED;
    }
    else if (nrChannels == 3) {
        format = GL_RGB;
    }
    else if (nrChannels == 4) {
        format = GL_RGBA;
    }
    else {
        fprintf(stderr, "Unsupported number of channels: %d in texture %s\n", nrChannels, filename);
        stbi_image_free(data);
        return 0;
    }

    // Upload the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image memory
    stbi_image_free(data);

    printf("Texture loaded successfully: %s (ID: %u)\n", filename, textureID);

    return textureID;
}

void initializeGraphics() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    
    // Request OpenGL version 3.3 Compatibility Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // Use Compatibility Profile

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Terrain Generator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to create GLFW window\n");
        exit(EXIT_FAILURE);
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Enable experimental features for core profiles
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Clear any OpenGL errors that might have been caused by GLEW
    glGetError();

    // Check OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    printf("OpenGL Version: %s\n", version);

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


    // Set up basic lighting
    setupLighting();
    // Enable Color Material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Set a distinct clear color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark gray background

    // Enable texture mapping
    glEnable(GL_TEXTURE_2D);

    // Load textures
    textureWater    = loadTexture("textures/water.jpeg"); // Currently commented out
    textureSand     = loadTexture("textures/sand.png");
    textureGrass    = loadTexture("textures/grass.png");
    textureMountain = loadTexture("textures/mountain.png");
    textureSnow     = loadTexture("textures/snow.png");

    // Check if all textures loaded successfully
    // Adjust the condition since textureWater is commented out
    if (!textureWater || !textureSand || !textureGrass || !textureMountain || !textureSnow) {
        fprintf(stderr, "One or more textures failed to load. Exiting.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    printf("All textures loaded successfully.\n");
}

bool isVoxelOccupied(int x, int z, int y, Terrain* terrain) {
    if (x < 0 || x >= terrain->width || z < 0 || z >= terrain->depth || y < 0) {
        return false; // Out of bounds
    }
    // Retrieve the height at (x, z)
    float heightValue = terrain->heights[z * terrain->width + x];
    return y <= (int)(heightValue * (MAX_HEIGHT)); // Define MAX_HEIGHT as the maximum possible Y value
}

// Function to choose texture based on height value
GLuint chooseTexture(float heightValue) {
    if (heightValue < 0.2f) { // Water
        return textureWater;
    } else if (heightValue < 0.4f) { // Sand
        return textureSand;
    } else if (heightValue < 0.6f) { // Grassland
        return textureGrass;
    } else if (heightValue < 0.8f) { // Mountain
        return textureMountain;
    } else { // Snow/Mountain Peaks
        return textureSnow;
    }
}

// Implement the renderFace function with texture mapping
void renderFace(float xpos, float ypos, float zpos, float cellWidth, float cellHeight, float depthStep, GLuint textureID, FaceType face) { 
    glBindTexture(GL_TEXTURE_2D, textureID);  // Bind the texture before drawing

    glBegin(GL_QUADS);
        switch(face) {
            case FRONT:
                // Front face (towards negative Z)
                glNormal3f(0.0f, 0.0f, -1.0f);  // Normal pointing outwards

                // Specify texture coordinates and vertices
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(xpos, ypos, zpos);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos, zpos);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos);
                break;
            
            case BACK:
                // Back face (towards positive Z)
                glNormal3f(0.0f, 0.0f, 1.0f);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(xpos, ypos, zpos + depthStep);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                break;
            
            case LEFT:
                // Left face (towards negative X)
                glNormal3f(-1.0f, 0.0f, 0.0f);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(xpos, ypos, zpos);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(xpos, ypos, zpos + depthStep);
                break;
            
            case RIGHT:
                // Right face (towards positive X)
                glNormal3f(1.0f, 0.0f, 0.0f);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos, zpos);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(xpos + cellWidth, ypos, zpos + depthStep);
                break;
            
            case TOP:
                // Top face (towards positive Y)
                glNormal3f(0.0f, 1.0f, 0.0f);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos + depthStep);
                break;   

            case BOTTOM:
                // Bottom face (towards negative Y)
                glNormal3f(0.0f, -1.0f, 0.0f);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(xpos, ypos + cellHeight, zpos);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(xpos + cellWidth, ypos + cellHeight, zpos + depthStep);

                glTexCoord2f(0.0f, 1.0f);
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
    float startZ = -(terrain->depth / 2.0f) * depthStep;

    // Loop through 2D terrain grid, rendering only the visible voxel faces
    for (int z = 0; z < terrain->depth; z++) {
        for (int x = 0; x < terrain->width; x++) {
            // Determine the maximum y for this (x, z) based on heightmap
            int maxY = (int)(terrain->heights[z * terrain->width + x] * MAX_HEIGHT);

            for (int y = 0; y <= maxY; y++) {
                // Calculate position of the voxel in 3D space
                float xpos = startX + (x * cellWidth);
                float ypos = y * cellHeight; // Y position is based on voxel height
                float zpos = startZ + (z * depthStep);

                // Current voxel height
                float heightValue = terrain->heights[z * terrain->width + x];
                if (!isVoxelOccupied(x, z, y, terrain)) continue;  // Skip rendering if voxel is not occupied

                // Choose the appropriate texture based on height
                GLuint textureID = chooseTexture(heightValue);
                if (textureID == 0) continue; // Skip if texture not found

                // Check neighboring voxels for face culling
                bool frontNeighborExists = (z > 0) && isVoxelOccupied(x, z - 1, y, terrain);
                bool backNeighborExists = (z < terrain->depth - 1) && isVoxelOccupied(x, z + 1, y, terrain);
                bool leftNeighborExists = (x > 0) && isVoxelOccupied(x - 1, z, y, terrain);
                bool rightNeighborExists = (x < terrain->width - 1) && isVoxelOccupied(x + 1, z, y, terrain);
                bool topNeighborExists = (y < maxY) && isVoxelOccupied(x, z, y + 1, terrain);
                bool bottomNeighborExists = (y > 0) && isVoxelOccupied(x, z, y - 1, terrain);

                // Render faces based on neighbor occupancy
                if (!frontNeighborExists) {
                    renderFace(xpos, ypos, zpos, cellWidth, cellHeight, depthStep, textureID, FRONT);
                }

                if (!backNeighborExists) {
                    renderFace(xpos, ypos, zpos + depthStep, cellWidth, cellHeight, depthStep, textureID, BACK);
                }

                if (!leftNeighborExists) {
                    renderFace(xpos, ypos, zpos, cellWidth, cellHeight, depthStep, textureID, LEFT);
                }

                if (!rightNeighborExists) {
                    renderFace(xpos + cellWidth, ypos, zpos, cellWidth, cellHeight, depthStep, textureID, RIGHT);
                }

                if (!topNeighborExists) {
                    renderFace(xpos, ypos + cellHeight, zpos, cellWidth, cellHeight, depthStep, textureID, TOP);
                }

                if (!bottomNeighborExists) {
                    renderFace(xpos, ypos, zpos, cellWidth, cellHeight, depthStep, textureID, BOTTOM);
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
