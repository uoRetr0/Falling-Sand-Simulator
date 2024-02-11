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

const int screenWidth = 2000;
const int screenHeight = 1600;
const int pixelSize = 8; // min 2
const int gridWidth = screenWidth / pixelSize;
const int gridHeight = screenHeight / pixelSize;
const int mouseGrid = 10;
const float gravity = 4;
const int viscosity = 8; // for liquids only
const int frequency = 15;

struct Particle {
    int state; // -1:empty, 0:stone, 1:sand, 2:water, 3:wet sand, 4:suspended stone, 5:dirt, 6:plant, 7:wet dirt, 8:fire, 9:smoke, 10:coal
    Color color;
    int velocity;
    int life;
};

enum ElementType {
    ELEMENT_NONE,
    ELEMENT_ERASE,
    ELEMENT_STONE,
    ELEMENT_SAND,
    ELEMENT_WATER,
    ELEMENT_SUSPENDED_STONE,
    ELEMENT_DIRT,
    ELEMENT_FIRE,
    ELEMENT_COAL,
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

Color dirtColor() {
    vector<array<int, 3>> colors = {
        {44, 33, 24},
        {54, 43, 34},
        {34, 23, 14},
        {39, 28, 19},
        {59, 48, 39}
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color plantColor() {
    vector<array<int, 3>> colors = {
        {69, 171, 21},   // Original color
        {74, 176, 25},   // Slightly lighter
        {64, 166, 18},   // Slightly darker
        {69, 176, 21},   // More green
        {69, 166, 21},   // Less green
        {74, 171, 25},   // Lighter and slightly more vibrant
        {64, 171, 18},   // Darker and slightly more muted
        {69, 171, 26},   // Slightly more blue
        {69, 171, 16},   // Slightly less blue
        {74, 166, 26},   // Lighter with less green and more blue

    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color wetDirtColor() {
    vector<array<int, 3>> colors = {
        {28, 18, 15},  // Darker, with a hint of cool moisture
        {38, 28, 25},  // Slightly lighter, still retaining cool, wet undertones
        {43, 33, 30},  // A balanced mix, slightly brighter but maintaining the wet look
        {25, 15, 12},  // Very dark, emphasizing the deep moisture absorption
        
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color newFireColor() {
    vector<array<int, 3>> colors = {
        {255, 0, 0},      // Deep red
        {255, 60, 0},     // Bright orange
        {255, 120, 0}     // Brighter orange
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color mediumFireColor() {
    vector<array<int, 3>> colors = {
        {255, 90, 0},     // Orange
        {255, 154, 0},    // Yellow-orange
        {255, 206, 0}     // Bright yellow
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color oldFireColor() {
    vector<array<int, 3>> colors = {
        {255, 232, 8},    // Light yellow
        {255, 240, 60},   // Pale yellow
        {255, 245, 120}   // Very light yellow
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color smokeColor() {
    vector<array<int, 3>> colors = {
        {24, 24, 24},  
        
    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color coalColor() {
    vector<array<int, 3>> colors = {
        {21, 23, 22}, // Original color
        {19, 21, 20}, // Slightly darker
        {23, 25, 24}, // Slightly lighter
        {20, 22, 21}, // Very subtle dark variation
        {22, 24, 23}, // Very subtle light variation
        {18, 20, 19}, // Darker still
        {24, 26, 25}  // Lighter still 
        
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
                    grid[x - 1][y + 1] = grid[x][y]; // Move water down left
                    movedDown = true;
                    grid[x][y].state = -1; 
                }
                else if (side == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == -1) {
                    grid[x + 1][y + 1] = grid[x][y]; // Move water down right
                    movedDown = true;
                    grid[x][y].state = -1; 
                }

                if (!movedDown) {
                    if (side == "LEFT" && x > 0 && grid[x - 1][y].state == -1) {
                        int tmp = 0;
                        for (int i = 1; i <= viscosity - 2; i++) {
                            if (x - i >= 0 && grid[x - i][y].state == -1) {
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

void updateDirt(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 5) continue;
            int odd = rand() % 100 + 1;

            if (grid[x][y + 1].state == 2) {
                grid[x][y] = {7, wetDirtColor(), 0, 0};
            }

            if (grid[x][y].state == 5 && grid[x][y + 1].state == -1) { // If there's space below
                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }
                grid[x][y + tmp] = grid[x][y];
                grid[x][y].state = -1;
            }

            else if (grid[x][y].state == 5 && (grid[x][y + 1].state != -1 || grid[x][y + 1].state != 2) && odd < 5) {

                if (dir() == "LEFT" && x > 0 && grid[x - 1][y + 1].state == -1) {
                    grid[x - 1][y + 1] = grid[x][y]; // Move dirt down left
                    grid[x][y].state = -1; 
                }
                
                else if (dir() == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == -1) {
                    grid[x + 1][y + 1] = grid[x][y]; // Move dirt down right
                    grid[x][y].state = -1; 
                }
            }

            if (grid[x][y - 1].state == 2) {
                grid[x][y - 1].state = -1;
                grid[x][y] = {6, plantColor(), 0, 1};
                if (y + 1 <= gridHeight - 1) {
                    grid[x][y + 1] = {6, plantColor(), 0, 1};
                }
            }
            else if (grid[x][y + 1].state == 2) {
                grid[x][y] = {7, wetDirtColor(), 0, 0};
            }
        }
    }
}

void updateWetDirt(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 7) continue;
            int odd = rand() % 100 + 1;


            if (grid[x][y].state == 7 && grid[x][y + 1].state == -1) { // If there's space below
                grid[x][y + 1] = grid[x][y];
                grid[x][y].state = -1;
            }
            
            else if (grid[x][y].state == 7 && (grid[x][y + 1].state != -1 || grid[x][y + 1].state != 2) && odd < 5) {

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

void updatePlant(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 6) continue;
            int maxLife = 10;
            int odd = rand() % 100 + 1;
            
            if (odd < 2 && grid[x][y].life < maxLife) {
                grid[x][y].life += 1;
            }

            if (grid[x][y].state == 6 && grid[x][y + 1].state == -1) { // If there's space below
                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }
                grid[x][y + tmp] = grid[x][y];
                grid[x][y].state = -1;
            }
            else if (grid[x][y + 1].state == 2) {
                grid[x][y] = {-1, BLACK, 0, 0};
            }

            odd = rand() % 100 + 1;
            if (grid[x][y - 1].state == 2 && odd < 5) {
                grid[x][y - 1].state = -1;
            }
            
            if (grid[x][y].life > 5 && grid[x][y].life < maxLife && grid[x][y - 1].state == -1) {
                grid[x][y - 1] = {6, plantColor(), 0, rand() % 9 + 6};
            }
        }
    }
}

void updateFire(Particle grid[gridWidth][gridHeight]) {
    const int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1, -2, 2, 0, 0, -2, -2, 2, 2}; // Direction vectors for x, including two steps
    const int dy[] = {0, 0, -1, 1, -1, 1, -1, 1, 0, 0, -2, 2, -2, 2, -2, 2}; // Direction vectors for y, including two steps
    const int spreadChance = 5; // Chance of fire spreading to adjacent plant
    const int maxLifeAir = 20; // Max life of fire in air
    const int maxLifePlant = 250; // Max life of fire on plant

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            if (grid[x][y].state != 8) continue;

            int maxLife = (grid[x][y + 1].state == 6) ? maxLifePlant : maxLifeAir;

            grid[x][y].life++;
            if (grid[x][y].life > maxLife) {
                grid[x][y] = {-1, BLACK, 0, 0}; // Convert to air after max life
                continue;
            }

            // Color change logic based on life span
            if (grid[x][y].life <= maxLife / 3) grid[x][y].color = newFireColor();
            else if (grid[x][y].life <= 2 * maxLife / 3) grid[x][y].color = mediumFireColor();
            else grid[x][y].color = oldFireColor();

            // Spread fire logic
            for (int direction = 0; direction < 16; direction++) { 
                int nx = x + dx[direction];
                int ny = y + dy[direction];

                if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
                    if (grid[nx][ny].state == 6 && rand() % 100 < spreadChance) { // If adjacent to a plant
                        grid[nx][ny] = {8, newFireColor(), 0, 0};
                    }
                }
            }

            if (y > 0 && grid[x][y - 1].state == -1 && rand() % 100 < 10) {
                grid[x][y - 1] = {9, smokeColor(), 0, rand() % 80 + 1};
            }
        }
    }
}

void updateSmoke(Particle grid[gridWidth][gridHeight]) {
    const int maxLife = 100;

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            if (grid[x][y].state != 9) continue;

            grid[x][y].life++;
            if (grid[x][y].life > maxLife) {
                grid[x][y] = {-1, BLACK, 0, 0};
                continue;
            }

            if (y > 0 && grid[x][y - 1].state == -1) {
                grid[x][y - 1] = grid[x][y];
                grid[x][y] = {-1, BLACK, 0, 0};
            }
            else if (y > 0 && grid[x][y - 1].state != -1) {
                if (dir() == "LEFT" && x > 0 && y > 0 && grid[x - 1][y - 1].state == -1) {
                    grid[x - 1][y - 1] = grid[x][y]; // Move smoke up left
                    grid[x][y].state = -1; 
                }
                else if (dir() == "RIGHT" && x < gridWidth - 1 && y > 0 && grid[x + 1][y - 1].state == -1) {
                    grid[x + 1][y - 1] = grid[x][y]; // Move smoke up right
                    grid[x][y].state = -1; 
                }
            }
        }
    }
}

void updateCoal(Particle grid[gridWidth][gridHeight]) {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = gridHeight - 2; y >= 0; y--) {
            if (grid[x][y].state != 10) continue;
            
            if (grid[x][y + 1].state == -1) { // If there's space below
                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }
                grid[x][y + tmp] = grid[x][y];
                grid[x][y].state = -1;
            }
            else if ((grid[x][y + 1].state != -1 || grid[x][y + 1].state != 2) && rand() % 100 < 10) {

                if (dir() == "LEFT" && x > 0 && grid[x - 1][y + 1].state == -1) {
                    grid[x - 1][y + 1] = grid[x][y]; // Move coal down left
                    grid[x][y].state = -1; 
                }
                else if (dir() == "LEFT" && x > 0 && grid[x - 1][y + 1].state == 2) {
                    Particle tmp = grid[x - 1][y + 1];
                    grid[x - 1][y + 1] = grid[x][y]; // swap coal down left
                    grid[x][y] = tmp;
                }
                else if (dir() == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == -1) {
                    grid[x + 1][y + 1] = grid[x][y]; // Move coal down right
                    grid[x][y].state = -1; 
                }
                else if (dir() == "RIGHT" && x < gridWidth - 1 && grid[x + 1][y + 1].state == 2) {
                    Particle tmp = grid[x + 1][y + 1];
                    grid[x + 1][y + 1] = grid[x][y]; // swap coal down right
                    grid[x][y] = tmp;
                }
            }

            if (grid[x][y + 1].state == 2) {
                Particle tmp = grid[x][y];
                grid[x][y] = grid[x][y + 1];
                grid[x][y + 1] = tmp;
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
                particle = {2, waterColor()};
                break;
            case ELEMENT_SUSPENDED_STONE:
                particle = {4, stoneColor()};
                break;
            case ELEMENT_DIRT:
                particle = {5, dirtColor()};
                break;
            case ELEMENT_FIRE:
                particle = {8, newFireColor(), 0, 0};
                break;
            case ELEMENT_COAL:
                particle = {10, coalColor(), 0, 0};
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

                    else if ((currentElement == ELEMENT_STONE || currentElement == ELEMENT_SUSPENDED_STONE) && odd < 80) {
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
    updateDirt(currentGrid);
    updatePlant(currentGrid);
    updateWetDirt(currentGrid);
    updateFire(currentGrid);
    updateSmoke(currentGrid);
    updateCoal(currentGrid);

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

        if (GuiButton((Rectangle){370, 10, 80, 30}, "SUS STONE")) {
            currentElement = (currentElement == ELEMENT_SUSPENDED_STONE) ? ELEMENT_NONE : ELEMENT_SUSPENDED_STONE;
        }

        if (GuiButton((Rectangle){460, 10, 80, 30}, "DIRT")) {
            currentElement = (currentElement == ELEMENT_DIRT) ? ELEMENT_NONE : ELEMENT_DIRT;
        }

        if (GuiButton((Rectangle){550, 10, 80, 30}, "FIRE")) {
            currentElement = (currentElement == ELEMENT_FIRE) ? ELEMENT_NONE : ELEMENT_FIRE;
        }

        if (GuiButton((Rectangle){640, 10, 80, 30}, "COAL")) {
            currentElement = (currentElement == ELEMENT_COAL) ? ELEMENT_NONE : ELEMENT_COAL;
        }

        mouseDrag(currentGrid);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
