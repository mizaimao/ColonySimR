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

// ---- Truly immutable constants (not configurable at runtime) ----

// Colony dimensions (grid size). Change only if you want to alter the world size.
const int COLONY_WIDTH = 20;
const int COLONY_HEIGHT = 20;
const int POPULATION_CAP = 200;
