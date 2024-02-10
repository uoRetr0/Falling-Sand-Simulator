#pragma once
#include "Particle.h"

extern const int screenWidth;
extern const int screenHeight;
extern const int pixelSize;
extern const int gridWidth;
extern const int gridHeight;
extern const int mouseGrid;
extern const float gravity;
extern const int viscosity;
extern const int frequency;

void mouseDrag(vector<vector<Particle*>>& grid);
void clear(vector<vector<Particle*>>& grid);
void updateSimulation(vector<vector<Particle*>>& grid);
void initializeGrids(vector<vector<Particle*>>& currentGrid, vector<vector<Particle*>>& nextGrid);
void handleGui(vector<vector<Particle*>>& grid);
