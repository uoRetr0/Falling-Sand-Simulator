#pragma once
#include <vector>
#include "raylib.h"
using std::vector;

enum ElementType {
    ELEMENT_NONE,
    ELEMENT_ERASE,
    ELEMENT_STONE,
    ELEMENT_SAND,
    ELEMENT_WATER
};

class Particle {
public:
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) = 0;
    virtual ~Particle() {}
    Color color;
};

class SolidState : public Particle {
public:
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) override;
};

class LiquidState : public Particle {
public:
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) override;
};

class Stone : public SolidState {
public:
    Stone();
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) override;
};

class Sand : public SolidState {
public:
    Sand();
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) override;
};

class Water : public LiquidState {
public:
    Water();
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) override;
};

class WetSand : public SolidState {
public:
    WetSand();
    virtual void update(vector<vector<Particle*>>& grid, int x, int y) override;
};
