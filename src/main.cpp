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

const int screenWidth = 1498;
const int screenHeight = 756;
const int pixelSize = 4; // min 2
const int gridWidth = screenWidth / pixelSize;
const int gridHeight = screenHeight / pixelSize;
const int gravity = 2; // const float gravity = 0.2f;
const int viscosity = 8; // for liquids only
int mouseGrid = 10;
int frequency = 15;

struct Particle {
    int state; // -1:empty, 0:stone, 1:sand, 2:water, 3:wet sand, 4:suspended stone, 5:dirt, 6:plant, 7:wet dirt, 8:fire, 9:smoke, 10:coal
    Color color;
    float velocity;
    int life;
    bool source;
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

            if (grid[x][y + 1].state == 8 || grid[x][y + 1].state == 9) {
                grid[x][y + 1] = grid[x][y];
                grid[x][y] = {-1, BLACK, 0, 0};
            }
            
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
            
            if (grid[x][y + 1].state == 8 || grid[x][y + 1].state == 9) {
                grid[x][y + 1] = grid[x][y];
                grid[x][y] = {-1, BLACK, 0, 0};
            }

            if (grid[x][y].state == 1 && grid[x][y + 1].state == -1) { // If there's space below
                
                // grid[x][y].velocity += gravity; // Accumulate velocity due to gravity

                // // Calculate how far the particle should move based on its velocity
                // int moveDistance = static_cast<int>(grid[x][y].velocity);

                // if (moveDistance > 0) {
                //     int newY = y + moveDistance; // Calculate the new Y position based on velocity
                //     if (newY >= gridHeight) newY = gridHeight - 1; // Clamp within grid bounds

                //     if (grid[x][newY].state == -1) { // Check if the new position is empty
                //         grid[x][newY] = grid[x][y]; // Move the particle to the new position
                //         grid[x][y] = {-1, BLACK, 0, 0}; // Reset the original position
                //     }
                //     else {
                //         grid[x][y].velocity = 0; // Reset velocity if it can't move
                //     }
                // }

                int tmp = 0;
                for (int i = 1; i <= gravity; i++) {
                    if (grid[x][y + i].state == -1 && y + i < gridHeight ) {
                        tmp = i;
                    }
                }
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

            if (grid[x][y + 1].state == 8 || grid[x][y + 1].state == 9) {
                grid[x][y + 1] = grid[x][y];
                grid[x][y] = {-1, BLACK, 0, 0};
                movedDown = true;
            }

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

            if (grid[x][y + 1].state == 8 || grid[x][y + 1].state == 9) {
                grid[x][y + 1] = grid[x][y];
                grid[x][y] = {-1, BLACK, 0, 0};
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
    const int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1, -2, 2, 0, 0, -2, -2, 2, 2}; 
    const int dy[] = {0, 0, -1, 1, -1, 1, -1, 1, 0, 0, -2, 2, -2, 2, -2, 2}; 
    const int spreadChance = 5; 
    const int maxLifeAir = 20; 
    const int maxLifePlant = 50; 

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            if (grid[x][y].state != 8) continue; 

            bool isOnCoal = (grid[x][y + 1].state == 10);
            grid[x][y].life++;
            if (!isOnCoal && (grid[x][y].life > maxLifeAir || (grid[x][y + 1].state == 6 && grid[x][y].life > maxLifePlant))) {
                grid[x][y] = {-1, BLACK, 0, 0};
                continue;
            }

            if (!isOnCoal) {
                if (grid[x][y].life <= maxLifeAir / 3 || (grid[x][y + 1].state == 6 && grid[x][y].life <= maxLifePlant / 3)) {
                    grid[x][y].color = newFireColor();
                } else if (grid[x][y].life <= 2 * maxLifeAir / 3 || (grid[x][y + 1].state == 6 && grid[x][y].life <= 2 * maxLifePlant / 3)) {
                    grid[x][y].color = mediumFireColor();
                } else {
                    grid[x][y].color = oldFireColor();
                }
            }

            for (int direction = 0; direction < 16; direction++) {
                int nx = x + dx[direction];
                int ny = y + dy[direction];

                if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight && (grid[nx][ny].state == 6) && rand() % 100 < spreadChance) {                    
                    grid[nx][ny] = {8, newFireColor(), 0, 0, true};
                }
            }


            if (grid[x][y].source == true) {
                int maxHeight = rand() % 7 + 2; 
                for (int offset = 1; offset <= maxHeight; offset++) { 
                    int targetY = y - offset;
                    // Randomly decide direction: straight up, up-right, or up-left
                    int direction = rand() % 3; // 0: Up, 1: Up-Right, 2: Up-Left
                    int targetX = x; // Default to straight up

                    if (direction == 1 && x < gridWidth - 1) { // Move up-right if not at right edge
                        targetX = x + 1;
                    } else if (direction == 2 && x > 0) { // Move up-left if not at left edge
                        targetX = x - 1;
                    }

                    if (targetY >= 0 && grid[targetX][targetY].state == -1 && rand() % 100 < 15) { 
                        grid[targetX][targetY] = {8, newFireColor(), 0, 0, false}; 
                    }
                }
            }
            
            if (y > 0 && grid[x][y - 1].state == -1 && rand() % 100 < 10) {
                grid[x][y - 1] = {9, {24, 24, 24, 255}, 0, rand() % 80 + 1};
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

bool isMouseOverGui() {
    Rectangle brushSlider = {80, 50, 120, 20}; // Position and size of the "Brush Size" slider
    Rectangle frequencySlider = {80, 80, 120, 20}; // Position and size of the "Frequency" slider
    Rectangle gridPreviewToggle = {screenWidth - 220, 10, 100, 30}; // Position and size of the "Grid Preview" toggle
    Rectangle clearButton = {screenWidth - 110, 10, 100, 30}; // Position and size of the "CLEAR" button
    Rectangle eraseButton = {screenWidth - 110, 50, 100, 30}; // Position and size of the "ERASE" button
    Rectangle sandButton = {10, 10, 80, 30}; // Position and size of the "SAND" button
    Rectangle waterButton = {100, 10, 80, 30}; // Position and size of the "WATER" button   
    Rectangle stoneButton = {190, 10, 80, 30}; // Position and size of the "STONE" button
    Rectangle susStoneButton = {280, 40, 80, 30}; // Position and size of the "SUS STONE" button
    Rectangle dirtButton = {370, 10, 80, 30}; // Position and size of the "DIRT" button
    Rectangle fireButton = {460, 10, 80, 30}; // Position and size of the "FIRE" button
    Rectangle coalButton = {550, 10, 80, 30}; // Position and size of the "COAL" button
    Rectangle bgToggle = {screenWidth - 240, 50, 120, 30}; // Position and size of the "BG" toggle

    Vector2 mousePoint = GetMousePosition();

    // Check if the mouse is over any of the GUI elements
    if (CheckCollisionPointRec(mousePoint, sandButton) ||
        CheckCollisionPointRec(mousePoint, dirtButton) ||
        CheckCollisionPointRec(mousePoint, waterButton) ||
        CheckCollisionPointRec(mousePoint, stoneButton) ||
        CheckCollisionPointRec(mousePoint, susStoneButton) ||
        CheckCollisionPointRec(mousePoint, fireButton) ||
        CheckCollisionPointRec(mousePoint, coalButton) ||
        CheckCollisionPointRec(mousePoint, brushSlider) ||
        CheckCollisionPointRec(mousePoint, frequencySlider) ||
        CheckCollisionPointRec(mousePoint, gridPreviewToggle) ||
        CheckCollisionPointRec(mousePoint, clearButton) ||
        CheckCollisionPointRec(mousePoint, eraseButton) ||
        CheckCollisionPointRec(mousePoint, bgToggle)) {
        return true;
    }

    return false; 
}


void mouseDrag(Particle grid[gridWidth][gridHeight]) {
    if (!isMouseOverGui() && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Particle particle;
        switch (currentElement) {
            case ELEMENT_ERASE:
                particle = {-1, BLACK, 0, 0};
                break;
            case ELEMENT_STONE:
                particle = {0, stoneColor(), 0, 0};
                break;
            case ELEMENT_SAND:
                particle = {1, sandColor(), 0, 0}; ///////////////////////////////////////////////////////////////////////////
                break;
            case ELEMENT_WATER:
                particle = {2, waterColor(), 0, 0};
                break;
            case ELEMENT_SUSPENDED_STONE:
                particle = {4, stoneColor(), 0, 0};
                break;
            case ELEMENT_DIRT:
                particle = {5, dirtColor(), 0, 0};
                break;
            case ELEMENT_FIRE:
                particle = {8, newFireColor(), 0, 0, true};
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

    float mouseGridFloat = static_cast<float>(mouseGrid);
    float frequencyFloat = static_cast<float>(frequency);
    bool showMouseGridPreview = true;

    // Load frame images
    Texture2D frames[4];
    for (int i = 0; i < 4; i++) {
        char fileName[32];
        sprintf(fileName, "frames/frame%d.png", i);
        frames[i] = LoadTexture(fileName);
        if (frames[i].id == 0) {
            TraceLog(LOG_ERROR, "Failed to load frame: %s", fileName);
        }
    }

    bool animateBackground = false;
    int currentFrame = 0;
    double lastFrameChange = GetTime();

    while (!WindowShouldClose()) {
        double currentTime = GetTime();

        if (animateBackground && currentTime - lastFrameChange >= 1) { // Change ever 1 second
            currentFrame = GetRandomValue(0, 3);
            lastFrameChange = currentTime;
        }

        updateSimulation();
        
        BeginDrawing();

        // Draw the background first
        if (animateBackground) {
            DrawTexture(frames[currentFrame], 0, 0, WHITE);
        } else {
            ClearBackground(BLACK);
        }

        // Then draw the grid elements
        for (int i = 0; i < gridWidth; i++) {
            for (int j = 0; j < gridHeight; j++) {
                if (currentGrid[i][j].state != -1) {
                    DrawRectangle(i * pixelSize, j * pixelSize, pixelSize, pixelSize, currentGrid[i][j].color);
                }
            }
        }

        GuiSlider((Rectangle){80, 50, 120, 20}, "BRUSH SIZE", TextFormat("%d", (int)mouseGridFloat), &mouseGridFloat, 1.0f, 20.0f);
        mouseGrid = static_cast<int>(mouseGridFloat);

        GuiSlider((Rectangle){80, 80, 120, 20}, "FREQUENCY", TextFormat("%d", (int)frequencyFloat), &frequencyFloat, 1.0f, 100.0f);
        frequency = static_cast<int>(frequencyFloat);

        if (GuiToggle((Rectangle){screenWidth - 220, 10, 100, 30}, "GRID", &showMouseGridPreview)) {
            showMouseGridPreview = !showMouseGridPreview;
        }

        if (GuiToggle((Rectangle){screenWidth - 220, 50, 100, 30}, "STARS", &animateBackground)) {
            animateBackground = !animateBackground;
        }

        if (GuiButton((Rectangle){screenWidth - 110, 10, 100, 30}, "CLEAR")) {
            clear(currentGrid);
        }

        if (GuiButton((Rectangle){screenWidth - 110, 50, 100, 30}, "ERASE")) {
            currentElement = (currentElement == ELEMENT_ERASE) ? ELEMENT_NONE : ELEMENT_ERASE;
        }  

        if (GuiButton((Rectangle){10, 10, 80, 30}, "SAND")) {
            currentElement = (currentElement == ELEMENT_SAND) ? ELEMENT_NONE : ELEMENT_SAND;
        }

        if (GuiButton((Rectangle){100, 10, 80, 30}, "WATER")) {
            currentElement = (currentElement == ELEMENT_WATER) ? ELEMENT_NONE : ELEMENT_WATER;
        }

        if (GuiButton((Rectangle){190, 10, 80, 30}, "STONE")) {
            currentElement = (currentElement == ELEMENT_STONE) ? ELEMENT_NONE : ELEMENT_STONE;
        }

        if (GuiButton((Rectangle){280, 10, 80, 30}, "SUS STONE")) {
            currentElement = (currentElement == ELEMENT_SUSPENDED_STONE) ? ELEMENT_NONE : ELEMENT_SUSPENDED_STONE;
        }

        if (GuiButton((Rectangle){370, 10, 80, 30}, "DIRT")) {
            currentElement = (currentElement == ELEMENT_DIRT) ? ELEMENT_NONE : ELEMENT_DIRT;
        }

        if (GuiButton((Rectangle){460, 10, 80, 30}, "FIRE")) {
            currentElement = (currentElement == ELEMENT_FIRE) ? ELEMENT_NONE : ELEMENT_FIRE;
        }

        if (GuiButton((Rectangle){550, 10, 80, 30}, "COAL")) {
            currentElement = (currentElement == ELEMENT_COAL) ? ELEMENT_NONE : ELEMENT_COAL;
        }

        mouseDrag(currentGrid);

        if (showMouseGridPreview) {
            // Draw mouse grid preview edges
            Vector2 mousePosition = GetMousePosition();
            int gridMouseX = static_cast<int>(mousePosition.x) / pixelSize;
            int gridMouseY = static_cast<int>(mousePosition.y) / pixelSize;
            int halfSize = mouseGrid / 2; // Assuming mouseGrid is an odd number for a centered square. Adjust if necessary.

            // Adjust for even mouseGrid sizes
            int offsetX = (mouseGrid % 2 == 0) ? pixelSize : 0;
            int offsetY = (mouseGrid % 2 == 0) ? pixelSize : 0;

            // Calculate the top-left corner of the grid area
            int topLeftX = (gridMouseX - halfSize) * pixelSize;
            int topLeftY = (gridMouseY - halfSize) * pixelSize;

            // Calculate the width and height of the grid area
            int width = mouseGrid * pixelSize + offsetX;
            int height = mouseGrid * pixelSize + offsetY;

            // Draw the perimeter
            DrawRectangleLines(topLeftX, topLeftY, width, height, RED);
        }

        EndDrawing();
    }

    // Cleanup code
    for (int i = 0; i < 4; i++) {
        UnloadTexture(frames[i]);
    }

    CloseWindow();
    return 0;
}