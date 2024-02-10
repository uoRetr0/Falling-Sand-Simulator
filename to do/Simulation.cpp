#include "Simulation.h"
#include "Utilities.h"
#include "Config.h"
#include "raygui.h"
#include "Simulation.h"
#include "Globals.h"
#include <cmath>

void mouseDrag(vector<vector<Particle*>>& grid) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        int extend = floor(mouseGrid / 2);

        for (int i = -extend; i <= extend; i++) {
            for (int j = -extend; j <= extend; j++) {
                int gridX = (mouseX / pixelSize) + i;
                int gridY = (mouseY / pixelSize) + j;

                if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
                    delete grid[gridX][gridY]; // Free existing particle
                    grid[gridX][gridY] = nullptr; // Prevent dangling pointer
                    
                    switch (currentElement) {
                        case ELEMENT_ERASE:
                            // Already set to nullptr above
                            break;
                        case ELEMENT_STONE:
                            grid[gridX][gridY] = new Stone();
                            break;
                        case ELEMENT_SAND:
                            grid[gridX][gridY] = new Sand();
                            break;
                        case ELEMENT_WATER:
                            grid[gridX][gridY] = new Water();
                            break;
                        // Add cases for other elements as needed
                        default:
                            break;
                    }
                }
            }
        }
    }
}


void clear(vector<vector<Particle*>>& grid) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            delete grid[x][y];
            grid[x][y] = nullptr;
        }
    }
}

void updateSimulation(vector<vector<Particle*>>& grid) {
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            if (grid[x][y] != nullptr) {
                grid[x][y]->update(grid, x, y);
            }
        }
    }
}

void initializeGrids(vector<vector<Particle*>>& currentGrid, vector<vector<Particle*>>& nextGrid) {
    currentGrid.resize(gridWidth, vector<Particle*>(gridHeight, nullptr));
    nextGrid.resize(gridWidth, vector<Particle*>(gridHeight, nullptr));
}


void handleGui(vector<vector<Particle*>>& grid) {
    if (GuiButton((Rectangle){10, 10, 80, 30}, "Erase")) {
        currentElement = ELEMENT_ERASE;
    }
    if (GuiButton((Rectangle){100, 10, 80, 30}, "Stone")) {
        currentElement = ELEMENT_STONE;
    }
    if (GuiButton((Rectangle){190, 10, 80, 30}, "Sand")) {
        currentElement = ELEMENT_SAND;
    }
    if (GuiButton((Rectangle){280, 10, 80, 30}, "Water")) {
        currentElement = ELEMENT_WATER;
    }
}
