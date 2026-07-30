#pragma once
#include <cstdint>

// Color helper (replaces sf::Color)
struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
        : r(_r), g(_g), b(_b), a(_a) {}
    Color() : r(0), g(0), b(0), a(255) {}

    // Clamped addition/subtraction for color modifiers (SFML-style)
    static uint8_t clamp(int v) { return v < 0 ? 0 : (v > 255 ? 255 : (uint8_t)v); }
    Color operator+(const Color &o) const { return {clamp(r+o.r), clamp(g+o.g), clamp(b+o.b), a}; }
    Color operator-(const Color &o) const { return {clamp(r-o.r), clamp(g-o.g), clamp(b-o.b), a}; }
};

// Randomness.
const int MASTER_SEED = 720;

// Colony.
const int COLONY_WIDTH = 20;
const int COLONY_HEIGHT = 20;
const int INITIAL_SPORE_COUNT = 50;
const int POPULATION_CAP = 200;

// Viewer.
const float TARGET_FPS = 30.0f;
const int WIDTH = 1280;
const int HEIGHT = 800;
const bool SHOW_TILE_GRIDLINES = true;
const bool SHOW_SPORE_OUTLINE = false;

// Simulation tick rate (decoupled from render FPS).
const int SIM_TICK_MS = 100;       // ms per sim tick (default: 10 ticks/sec)
const int SIM_TICKS_PER_SEC = 1000 / SIM_TICK_MS;

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

// UI: Population curve.
const float POP_CURVE_PANE_PADDING = 5.f;
const float POP_CURVE_PANE_REL_HEIGHT = 20.f;
const int POP_CURVE_TIMEPOINTS = 50;

// Viewer aesthetics.
const Color GRID_COLOR(150, 150, 150);
const Color BG_COLOR(103, 193, 202);
const Color SPORE_OUTLINE_COLOR(120, 120, 120);
const Color SPORE_SELECTED_OUTLINE_COLOR(255, 255, 255);
const Color GROUND_COLOR(141, 191, 57);
const Color UNDERGROUND_COLOR(155, 118, 83);
const Color UNDERGROUND_LEFT_COLOR_MODIFIER(20, 20, 20, 0);
const Color UNDERGROUND_RIGHT_COLOR_MODIFIER(20, 20, 20, 0);
const float UNDERGROUND_HEIGHT_REL = 0.05f;  // Ratio to CENTRAL_TOTAL_HEIGHT.
const float SPORE_HEIGHT_REL = 0.3f;  // Ratio to tile_height (calculated in viewer).

const Color SPORE_LEFT_COLOR_MODIFIER(20, 20, 20, 0);
const Color SPORE_RIGHT_COLOR_MODIFIER(20, 20, 20, 0);
const Color BLUE_SPORE(66, 158, 245);
const Color RED_SPORE(250, 95, 95);

// Spore events.
const int SPORE_AGE_CAP = -1;  // Spores elder than this age will be killed (is it how it works now?).

const int SPORE_REPRODUCTION_PCT = 10;  // How likely spores reproduce when encountered (requires opposite sexes).
const int SPORE_REPRODUCTION_AGE_LOW = 0;  // At least how old a spore can reproduce.
// const int SPORE_REPRODUCTION_AGE_HIGH = 50;  // At most how old a spore can reproduce.
const int SPORE_REPRODUCTION_HEALTH_REQ = 90;  // At least how healthy a spore can reproduce.
const int SPORE_REPRODUCTION_MOOD_REQ = 80;  // At least how happy a spore can reproduce.
const int SPORE_REPRODUCTION_CONCEIVE_RATE = 50;  // If a reproduction is triggered, this is the chance of getting a new spore.

const int SPORE_FIGHT_PCT = 1;  // Chances of starting a fight (requires two males).
const int SPORE_FIGHT_DAMAGE_IMPACT_MAX = 10;  // If a fight is triggered, max damage to either sides.
const int SPORE_FIGHT_FATAL_PCT = 10;  // Fatal fight.

const int SPORE_CHAT_CHANCE = 89;  // Two spores start a chat when on the same tile.
const int SPORE_CHAT_HAPPINESS_CHANCE = 40;  // How likely to get a happy conversation, boosting happiness.
const int SPORE_CHAT_NEUTRAL_CHANCE = 30;
const int SPORE_CHAT_NEGATIVE_CHANCE = 30;
const int SPORE_CHAT_MOOD_IMPACT_MAX = 10;  // Randomly pick a number between 0 and this to affect happiness when a spore chats.

const float SPORE_MOOD_RECOVER_PER_TURN = 1.0f;
const float SPORE_HEALTH_RECOVER_PER_TURN = 1.0f;