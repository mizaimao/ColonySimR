#pragma once
#include <SFML/Graphics.hpp>

// Colony.
const int COLONY_WIDTH = 20;
const int COLONY_HEIGHT = 20;

// Viewer.
const float TARGET_FPS = 30.0f;
const int WIDTH = 1280;
const int HEIGHT = 800;

// Viewer offsets.
const int LOWER_PANE_HEIGHT = 200;
const int LEFT_PANE_WIDTH = 300;
const int CENTRAL_PADDING_X = 100;
const int CENTRAL_PADDING_Y = 100;
// After removing panes usable area.
const int CENTRAL_TOTAL_WIDTH = WIDTH - LEFT_PANE_WIDTH - CENTRAL_PADDING_X * 2;
const int CENTRAL_TOTAL_HEIGHT = HEIGHT - LOWER_PANE_HEIGHT - CENTRAL_PADDING_Y * 2;
// Figure out X and Y size of a tile.
int TILE_WIDTH = (int)( CENTRAL_TOTAL_WIDTH / COLONY_WIDTH);
int TILE_HEIGHT = (int)( CENTRAL_TOTAL_HEIGHT / COLONY_HEIGHT);
// First plottable x and y pixels.
const int X_OFFSET = LEFT_PANE_WIDTH + CENTRAL_TOTAL_WIDTH;
const int Y_OFFSET = CENTRAL_PADDING_Y;

// Viewer aesthetics.
const sf::Color GRID_COLOR(110, 110, 110);
const sf::Color BG_COLOR(103, 193, 202);
const sf::Color GROUND_COLOR(141, 191, 57);
const sf::Color UNDERGROUND_COLOR(155, 118, 83);
const sf::Color UNDERGROUND_LEFT_COLOR_MODIFIER(20, 20, 20, 0);
const sf::Color UNDERGROUND_RIGHT_COLOR_MODIFIER(20, 20, 20, 0);
const float UNDERGROUND_HEIGHT_REL = 0.05f;  // Ratio to CENTRAL_TOTAL_HEIGHT.
const float SPORE_HEIGHT_REL = 0.3f;  // Ratio to tile_height (calculated in viewer).

const sf::Color SPORE_LEFT_COLOR_MODIFIER(20, 20, 20, 0);
const sf::Color SPORE_RIGHT_COLOR_MODIFIER(20, 20, 20, 0);
const sf::Color BLUE_SPORE(66, 158, 245);
const sf::Color RED_SPORE(250, 95, 95);


