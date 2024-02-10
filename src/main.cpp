#include <iostream>
#include "raylib.h"
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include <array>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


using namespace std;

const int screenWidth = 1000;
const int screenHeight = 800;
const int pixelSize = 4; // min 2
const int gridWidth = screenWidth / pixelSize;
const int gridHeight = screenHeight / pixelSize;
const int mouseGrid = 5;
const float gravity = 2;
const int viscosity = 8; // for liquids only
const int frequency = 15;

struct Particle {
    int state; // -1:empty, 0:stone, 1:sand, 2:water, 3:wet sand
    Color color;
    int velocity;
    int life;
};

enum ElementType {
    ELEMENT_NONE,
    ELEMENT_ERASE,
    ELEMENT_STONE,
    ELEMENT_SAND,
    ELEMENT_WATER
};

ElementType currentElement = ELEMENT_NONE;

string dir() {
    int dir = rand() % 2 + 1;
    string direction = (dir == 1) ? "LEFT" : "RIGHT";

    return direction;
}

Color stoneColor() {
    vector<array<int, 3>> colors = {
        {71, 71, 71},
        {81, 81, 81},
        {66, 66, 66},

    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color sandColor() {
    vector<array<int, 3>> colors = {
        {194, 178, 128},
        {200, 184, 134},
        {189, 173, 123},
        {194, 183, 128},
        {194, 178, 138},
        {204, 188, 128},
        {184, 168, 128},
        {194, 178, 118}

    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color wetSandColor() {
    vector<array<int, 3>> colors = {
        {174, 143, 96},
        {184, 153, 106},
        {164, 133, 86},
        {179, 148, 101},
        {169, 138, 91},
        {189, 158, 111}
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color waterColor() {
    vector<array<int, 3>> colors = {
        {35, 137, 218},
        {45, 147, 228},
        {25, 127, 208},
        {30, 137, 213},
        {40, 137, 223},
        {50, 152, 218},
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

void updateStone(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 0) continue;
            
            if (grid[x][y].state == 0 && grid[x][y + 1].state == -1) { // If there's space below
                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }
                grid[x][y + tmp] = grid[x][y];
                grid[x][y].state = -1;
            }

            if (grid[x][y + 1].state == 2) {
                Particle tmp = grid[x][y];
                grid[x][y] = grid[x][y + 1];
                grid[x][y + 1] = tmp;
            }
        }
    }
}


void updateSand(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 1) continue;

            if (grid[x][y - 1].state == 2) {
                grid[x][y] = {3, wetSandColor(), 0, 1};
                if (y + 1 <= gridHeight - 1) {
                    grid[x][y + 1] = {3, wetSandColor(), 0, 1};
                }
                if (y + 2 <= gridHeight - 1) {
                    grid[x][y + 2] = {3, wetSandColor(), 0, 1};
                }
            }
            else if (grid[x][y + 1].state == 2) {
                grid[x][y] = {3, wetSandColor(), 0, 1};
            }

            if (grid[x][y].state == 1 && grid[x][y + 1].state == -1) { // If there's space below
                
                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }

                // grid[x][y].velocity += gravity;

                // if (y + grid[x][y].velocity < gridHeight) {
                //     grid[x][y + grid[x][y].velocity] = grid[x][y]; // Move sand down
                // }

                // grid[x][y].state = -1;

                grid[x][y + tmp] = grid[x][y];

                if (tmp != 0) {
                    grid[x][y].state = -1;
                }
            }

            else if (grid[x][y].state == 1 && grid[x][y + 1].state != -1) {

                if (dir() == "LEFT" && x > 0 && grid[x - 1][y + 1].state == -1) {
                    grid[x - 1][y + 1] = grid[x][y]; // Move sand down left
                    grid[x][y].state = -1; 
                }
                else if (dir() == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == -1) {
                    grid[x + 1][y + 1] = grid[x][y]; // Move sand down right
                    grid[x][y].state = -1; 
                }
            }
        }
    }
}


void updateWetSand(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 3) continue;
            int odd = rand() % 100 + 1;

            if (grid[x][y].life == 0) {
                grid[x][y].color = wetSandColor();
                grid[x][y].life += 1;
            }
            
            if (grid[x][y].state == 3 && grid[x][y + 1].state == -1) { // If there's space below
                grid[x][y + 1] = grid[x][y];
                grid[x][y].state = -1;
            }
            
            else if (grid[x][y].state == 3 && (grid[x][y + 1].state != -1 || grid[x][y + 1].state != 2) && odd < 20) {

                if (dir() == "LEFT" && x > 0 && grid[x - 1][y + 1].state == -1) {
                    grid[x - 1][y + 1] = grid[x][y]; // Move sand down left
                    grid[x][y].state = -1; 
                }
                else if (dir() == "LEFT" && x > 0 && grid[x - 1][y + 1].state == 2) {
                    Particle tmp = grid[x - 1][y + 1];
                    grid[x - 1][y + 1] = grid[x][y]; // swap sand down left
                    grid[x][y] = tmp;
                }
                else if (dir() == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == -1) {
                    grid[x + 1][y + 1] = grid[x][y]; // Move sand down right
                    grid[x][y].state = -1; 
                }
                else if (dir() == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == 2) {
                    Particle tmp = grid[x + 1][y + 1];
                    grid[x + 1][y + 1] = grid[x][y]; // swap sand down right
                    grid[x][y] = tmp;
                }
            }

            if (grid[x][y + 1].state == 2) { // water swap
                Particle tmp = grid[x][y];
                grid[x][y] = grid[x][y + 1];
                grid[x][y + 1] = tmp;
            }
        }
    }
}


void updateWater(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 2) continue;

            bool movedDown = false;
            string side = dir();

            if (grid[x][y].state == 2 && grid[x][y + 1].state == -1) { // If there's space below
                
                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }

                grid[x][y + tmp] = grid[x][y]; // Move water down
                movedDown = true;

                if (tmp != 0) {
                    grid[x][y].state = -1;
                }
            }

            else if (!movedDown) {
                if (side == "LEFT" && x > 0 && grid[x - 1][y + 1].state == -1) {
                    
                    int tmp = 0;
                    for (int i = 1; i <= viscosity - 2; i++) {
                        if (x - i >= 0 && grid[x - i][y + 1].state == -1) {
                            // Check if the particle before the potential new position is water
                            if (grid[x - (i - 1)][y + 1].state != 2) {
                                break;
                            }
                            if (grid[x - (i + 1)][y + 1].state != -1 && grid[x - (i + 1)][y + 1].state != 2) {
                                break;
                            }
                            tmp = i;
                        }

                        if (tmp > 0) {
                            grid[x - tmp][y + 1] = grid[x][y]; // Move water left
                            grid[x][y].state = -1;
                            movedDown = true;
                        }
                    }
                }
                else if (side == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == -1) {

                    int tmp = 0;
                    for (int i = 1; i <= viscosity; i++) {
                        if (x + i < gridWidth && grid[x + i][y + 1].state == -1) {
                            // Check if the particle before the potential new position is water
                            if (grid[x + (i - 1)][y + 1].state != 2) {
                                break;
                            }
                            if (grid[x + (i + 1)][y + 1].state != -1 && grid[x + (i + 1)][y + 1].state != 2) {
                                break;
                            }
                            tmp = i;
                        }
                        if (tmp > 0) {
                            grid[x + tmp][y + 1] = grid[x][y]; // Move water left
                            grid[x][y].state = -1;
                            movedDown = true;
                        }
                    }
                }

                if (!movedDown) {
                    if (side == "LEFT" && x > 0 && grid[x - 1][y].state == -1) {
                        int tmp = 0;
                        for (int i = 1; i <= viscosity - 2; i++) {
                            if (x - i >= 0 && grid[x - i][y].state == -1) {
                                // Check if the particle before the potential new position is water
                                if (grid[x - (i - 1)][y].state != 2) {
                                    break;
                                }
                                if (grid[x - (i + 1)][y].state != -1 && grid[x - (i + 1)][y].state != 2) {
                                    break;
                                }
                                tmp = i;
                            }
                        }
                        if (tmp > 0) {
                            grid[x - tmp][y] = grid[x][y]; // Move water left
                            grid[x][y].state = -1;
                        }
                    }
                    
                    else if (side == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y].state == -1) {
                        int tmp = 0;
                        for (int i = 1; i <= viscosity; i++) {
                            if (x + i < gridWidth && grid[x + i][y].state == -1) {
                                // Check if the particle before the potential new position is water
                                if (grid[x + (i - 1)][y].state != 2) {
                                    break;
                                }
                                if (grid[x + (i + 1)][y].state != -1 && grid[x + (i + 1)][y].state != 2) {
                                    break;
                                }
                                tmp = i;
                            }
                        }
                        if (tmp > 0) {
                            grid[x + tmp][y] = grid[x][y]; // Move water right
                            grid[x][y].state = -1;
                        }
                    }
                }


            }

            if (rand() % 100 < 1) {
                grid[x][y].color = waterColor();
            }
        }
    }
}


void mouseDrag(Particle grid[gridWidth][gridHeight]) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Particle particle;
        switch (currentElement) {
            case ELEMENT_ERASE:
                particle = {-1, BLACK};
                break;
            case ELEMENT_STONE:
                particle = {0, stoneColor()};
                break;
            case ELEMENT_SAND:
                particle = {1, sandColor(), 0}; ///////////////////////////////////////////////////////////////////////////
                break;
            case ELEMENT_WATER:
                particle = {2, waterColor(), 0};
                break;
            default:
                return;
        }

        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        int extend = floor(mouseGrid / 2);
        

        for (int i = -extend; i <= extend; i++) {
            for (int j = -extend; j <= extend; j++) {
                int gridX = (mouseX / pixelSize) + i;
                int gridY = (mouseY / pixelSize) + j;

                if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
                    int odd = rand() % 100 + 1;
                    
                    if (currentElement == ELEMENT_ERASE) {
                        grid[gridX][gridY] = particle;
                    }

                    else if (currentElement == ELEMENT_STONE && odd < 80) {
                        grid[gridX][gridY] = particle;
                    }
                    
                    else if (currentElement != ELEMENT_ERASE && odd < frequency) {
                        grid[gridX][gridY] = particle;
                    }
                }
            }
        }
    }
}

void clear(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 1; y >= 0; y--) {
            grid[x][y].state = -1;
        }
    }
}

Particle currentGrid[gridWidth][gridHeight];
Particle nextGrid[gridWidth][gridHeight];

void initializeGrids() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            currentGrid[x][y] = {-1};
            nextGrid[x][y] = {-1};
        }
    }
}

void applyUpdates() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            currentGrid[x][y] = nextGrid[x][y];
        }
    }
}

void updateSimulation() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            nextGrid[x][y] = { .state = -1 };
        }
    }

    updateStone(currentGrid);
    updateSand(currentGrid);
    updateWetSand(currentGrid);
    updateWater(currentGrid);

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            nextGrid[x][y] = currentGrid[x][y];
        }
    }
    
    applyUpdates();
}

int main() {
    InitWindow(screenWidth, screenHeight, "SAND SIMULATOR");
    SetTargetFPS(60);

    initializeGrids();

    while (!WindowShouldClose()) {

        updateSimulation();
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        for (int i = 0; i < gridWidth; i++) {
            for (int j = 0; j < gridHeight; j++) {
                if (currentGrid[i][j].state != -1) {
                    DrawRectangle(i * pixelSize, j * pixelSize, pixelSize, pixelSize, currentGrid[i][j].color);
                }
            }
        }

        if (GuiButton((Rectangle){screenWidth - 110, 10, 100, 30}, "CLEAR")) {
            clear(currentGrid);
        }

        if (GuiButton((Rectangle){10, 10, 80, 30}, "ERASE")) {
            currentElement = (currentElement == ELEMENT_ERASE) ? ELEMENT_NONE : ELEMENT_ERASE;
        }  

        if (GuiButton((Rectangle){100, 10, 80, 30}, "SAND")) {
            currentElement = (currentElement == ELEMENT_SAND) ? ELEMENT_NONE : ELEMENT_SAND;
        }

        if (GuiButton((Rectangle){190, 10, 80, 30}, "WATER")) {
            currentElement = (currentElement == ELEMENT_WATER) ? ELEMENT_NONE : ELEMENT_WATER;
        }

        if (GuiButton((Rectangle){280, 10, 80, 30}, "STONE")) {
            currentElement = (currentElement == ELEMENT_STONE) ? ELEMENT_NONE : ELEMENT_STONE;
        }

        mouseDrag(currentGrid);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
