#pragma once

const int screenWidth = 1000;
const int screenHeight = 800;
const int pixelSize = 4; // Minimum 2
const int gridWidth = screenWidth / pixelSize;
const int gridHeight = screenHeight / pixelSize;
const int mouseGrid = 5;
const float gravity = 2.0f;
const int viscosity = 8; // For liquids only
const int frequency = 15;
