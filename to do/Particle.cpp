#include "Particle.h"
#include "Config.h"
#include "Utilities.h"

Stone::Stone() {
    color = stoneColor();
}

void Stone::update(vector<vector<Particle*>>& grid, int x, int y) {
        // Ensure there's a space below and within grid bounds
        if (y + 1 < gridHeight && grid[x][y + 1] == nullptr) {
            // Simple fall to the next empty space
            grid[x][y + 1] = grid[x][y];
            grid[x][y] = nullptr;
        } else if (y + 1 < gridHeight && dynamic_cast<Water*>(grid[x][y + 1]) != nullptr) {
            // Swap with water below
            std::swap(grid[x][y], grid[x][y + 1]);
        }
        // Additional logic to handle movement or interactions
}

Sand::Sand() {
    color = sandColor();
}

void Sand::update(vector<vector<Particle*>>& grid, int x, int y) {
        // Check for water below to turn into wet sand
        if (y + 1 < gridHeight && dynamic_cast<Water*>(grid[x][y + 1]) != nullptr) {
            // Assuming WetSand is another class derived from Particle
            delete grid[x][y]; // Clean up the current sand particle
            grid[x][y] = new WetSand(); // Replace with wet sand
        }

        // Gravity for sand
        int newY = y;
        while (newY + 1 < gridHeight && grid[x][newY + 1] == nullptr) {
            newY++;
        }

        if (newY != y) { // Sand has space to fall
            grid[x][newY] = grid[x][y]; // Move sand down
            grid[x][y] = nullptr; // Clear original position
        } else if (y + 1 < gridHeight) { // Check for diagonal movement if directly below is occupied
            int direction = (rand() % 2) * 2 - 1; // Randomly choose left (-1) or right (1)
            int newX = x + direction;
            if (newX >= 0 && newX < gridWidth && grid[newX][y + 1] == nullptr) { // Check bounds and if new position is empty
                grid[newX][y + 1] = grid[x][y]; // Move sand diagonally
                grid[x][y] = nullptr; // Clear original position
            }
        }

        // Additional handling for interactions with water or other particles might be added here
}

Water::Water() {
    color = waterColor();
}

void Water::update(vector<vector<Particle*>>& grid, int x, int y) {
        int gridWidth = grid.size();
        int gridHeight = grid[0].size();
        bool moved = false;

        // Handle gravity
        for (int i = 1; i <= gravity && y + i < gridHeight; ++i) {
            if (grid[x][y + i] == nullptr) { // Space below is empty
                grid[x][y + i] = grid[x][y];
                grid[x][y] = nullptr;
                moved = true;
                break; // Water moved down, stop checking further
            }
        }

        // If not moved by gravity, try to move sideways
        int direction = rand() % 2 ? 1 : -1; // Choose direction randomly: 1 for right, -1 for left
        if (!moved) {
            int newX = x + direction;
            if (newX >= 0 && newX < gridWidth && grid[newX][y] == nullptr) { // Check if the new position is within bounds and empty
                grid[newX][y] = grid[x][y];
                grid[x][y] = nullptr;
                moved = true;
            }
        }

        // Try opposite direction if the first direction did not work
        if (!moved) {
            int oppositeDirection = -direction; // Reverse direction
            int newX = x + oppositeDirection;
            if (newX >= 0 && newX < gridWidth && grid[newX][y] == nullptr) {
                grid[newX][y] = grid[x][y];
                grid[x][y] = nullptr;
            }
        }

        // Randomly change color to simulate dynamic water
        if (!moved && rand() % 100 < 1) {
            grid[x][y]->color = waterColor();
        }
    }

WetSand::WetSand() {
    color = wetSandColor();
}

void WetSand::update(vector<vector<Particle*>>& grid, int x, int y) {
        bool moved = false;
        int gridWidth = grid.size();
        int gridHeight = grid[0].size();

        // Apply gravity if there's space below
        if (y + 1 < gridHeight && grid[x][y + 1] == nullptr) {
            grid[x][y + 1] = grid[x][y];
            grid[x][y] = nullptr;
            moved = true;
        }

        // Attempt to move or swap with water if not moved by gravity
        if (!moved && y + 1 < gridHeight) {
            Water* waterBelow = dynamic_cast<Water*>(grid[x][y + 1]);
            if (waterBelow) { // If there's water below, swap positions
                std::swap(grid[x][y], grid[x][y + 1]);
                moved = true;
            }
        }

        // Random chance to move sideways if blocked
        if (!moved) {
            int dir = rand() % 2 == 0 ? -1 : 1; // Choose direction randomly
            if (x + dir >= 0 && x + dir < gridWidth && y + 1 < gridHeight && grid[x + dir][y + 1] == nullptr) {
                // Move diagonally down-left or down-right
                grid[x + dir][y + 1] = grid[x][y];
                grid[x][y] = nullptr;
            } else if (x + dir >= 0 && x + dir < gridWidth && y + 1 < gridHeight) {
                // Attempt to swap with water diagonally
                Water* waterDiag = dynamic_cast<Water*>(grid[x + dir][y + 1]);
                if (waterDiag) {
                    std::swap(grid[x][y], grid[x + dir][y + 1]);
                }
            }
        }
    }

void SolidState::update(vector<vector<Particle*>>& grid, int x, int y) {
    // Default solid state update logic
}

void LiquidState::update(vector<vector<Particle*>>& grid, int x, int y) {
    // Default liquid state update logic
}
