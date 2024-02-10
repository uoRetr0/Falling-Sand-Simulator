#include "raylib.h"
#include "Config.h"
#include "Simulation.h"
#include "Utilities.h"
#include "Globals.h" // Ensure this is where ElementType and currentElement are declared
#include <vector>

// Assuming ElementType currentElement is defined in Globals.h
// Remove the definition of currentElement from here if it's in Globals.h

int main() {
    InitWindow(screenWidth, screenHeight, "SAND SIMULATOR");
    SetTargetFPS(60);

    std::vector<std::vector<Particle*>> grid(gridWidth, std::vector<Particle*>(gridHeight, nullptr));

    // Assuming initializeSimulation function is defined properly in Simulation.cpp
    // Uncomment the following line if you have implemented initializeSimulation
    // initializeSimulation(grid);

    while (!WindowShouldClose()) {
        updateSimulation(grid);
        
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < gridWidth; i++) {
            for (int j = 0; j < gridHeight; j++) {
                if (grid[i][j] != nullptr) {
                    DrawRectangle(i * pixelSize, j * pixelSize, pixelSize, pixelSize, grid[i][j]->color);
                }
            }
        }

        handleGui(grid); // handleGui doesn't need currentElement as a parameter anymore
        EndDrawing();
    }

    clear(grid); // Make sure to properly deallocate memory for particles
    CloseWindow();

    return 0;
}