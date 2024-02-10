#include "Utilities.h"
#include <cstdlib>
#include <vector>
#include <array>
#include <string>

Color stoneColor() {
    std::vector<std::array<int, 3>> colors = {
        {71, 71, 71},
        {81, 81, 81},
        {66, 66, 66},

    };

    int randomIndex = rand() % colors.size();
    auto selectedColor = colors[randomIndex];
    return (Color){static_cast<unsigned char>(selectedColor[0]), static_cast<unsigned char>(selectedColor[1]), static_cast<unsigned char>(selectedColor[2]), 255};
}

Color sandColor() {
    std::vector<std::array<int, 3>> colors = {
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
    std::vector<std::array<int, 3>> colors = {
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
    std::vector<std::array<int, 3>> colors = {
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

std::string dir() {
    int dir = rand() % 2 + 1;
    std::string direction = (dir == 1) ? "LEFT" : "RIGHT";

    return direction;
}
