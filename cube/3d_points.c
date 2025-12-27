#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> 

#define WIDTH 80
#define HEIGHT 24

void clearScreen(char screen[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screen[i][j] = ' ';
        }
    }
}

void drawPixel(char screen[HEIGHT][WIDTH], int x, int y, char c) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        screen[y][x] = c;
    }
}

void drawLine(char screen[HEIGHT][WIDTH], int x0, int y0, int x1, int y1, char c) {
    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        drawPixel(screen, x0, y0, c);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void displayScreen(char screen[HEIGHT][WIDTH]) {
    printf("\033[2J\033[H");
    
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            putchar(screen[i][j]);
        }
        putchar('\n');
    }
}

void project3D(float x, float y, float z, int *screenX, int *screenY) {
    // Perspective X' = X/Z, Y' = Y/Z
    // Adjusting the offset will move our "eyes" from camera
    float zOffset = z + 5.0f; 
    
    if (zOffset > 0.1f) {
        float xProj = x / zOffset;
        float yProj = y / zOffset;
        
        // Scale and center on screen
        *screenX = (int)(WIDTH / 2 + xProj * 20);
        *screenY = (int)(HEIGHT / 2 - yProj * 10); 
    } else {
        *screenX = -1;
        *screenY = -1;
    }
}

void drawCube(char screen[HEIGHT][WIDTH], float cube[][3], float translateZ) {
    int edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Front face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting edges
    };
    
    int numVertices = 8;
    int numEdges = sizeof(edges) / sizeof(edges[0]);
    
    // Why: have vertices and add translation
    int projected[8][2];
    for (int i = 0; i < numVertices; i++) {
        project3D(cube[i][0], cube[i][1], cube[i][2] + translateZ, 
                  &projected[i][0], &projected[i][1]);
    }
    
    for (int i = 0; i < numEdges; i++) {
        int v0 = edges[i][0];
        int v1 = edges[i][1];
        drawLine(screen, 
                 projected[v0][0], projected[v0][1],
                 projected[v1][0], projected[v1][1], 
                 '#');
    }
    
    for (int i = 0; i < numVertices; i++) {
        drawPixel(screen, projected[i][0], projected[i][1], '*');
    }
}

int main() {
    char screen[HEIGHT][WIDTH];
    
    float cube[][3] = {
        {-1.0f, -1.0f, -1.0f},  // 0: back bottom left
        { 1.0f, -1.0f, -1.0f},  // 1: back bottom right
        { 1.0f,  1.0f, -1.0f},  // 2: back top right
        {-1.0f,  1.0f, -1.0f},  // 3: back top left
        {-1.0f, -1.0f,  1.0f},  // 4: front bottom left
        { 1.0f, -1.0f,  1.0f},  // 5: front bottom right
        { 1.0f,  1.0f,  1.0f},  // 6: front top right
        {-1.0f,  1.0f,  1.0f},  // 7: front top left
    };
    
    printf("Press Ctrl+C to stop\n");
    usleep(1000000);
    
    for (int frame = 0; frame < 100; frame++) {
        clearScreen(screen);
        
        // Over time we want to move away
        float translateZ = -frame * 0.1f;
        
        drawCube(screen, cube, translateZ);
        
        displayScreen(screen);
        
        printf("\nFrame %d - Z offset: %.1f\n", frame, translateZ);
        printf("The cube is moving away from the camera!\n");
        
        usleep(100000);
    }
    
    printf("\nAnimation complete!\n");
    
    return 0;
}