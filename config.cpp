#pragma once
#include <string>
#include <stdexcept>
#include "src/deps/toml11/toml.hpp"
#include "config.h"  // For Color struct and compile-time constants

// Runtime configuration loaded from config.toml.
// Accessed globally via `cfg` after calling cfg.load().
class Config {
public:
    // --- General ---
    int master_seed = 720;
    int initial_spore_count = 50;

    // --- Viewer ---
    int target_fps = 30;
    int window_width = 1280;
    int window_height = 800;
    bool show_gridlines = true;
    bool show_spore_outline = false;

    // --- Simulation ---
    int sim_tick_ms = 100;
    int get_sim_ticks_per_sec() const { return 1000 / sim_tick_ms; }

    // --- Viewer panes ---
    int left_pane_width = 300;
    int central_padding_x = 100;
    int central_padding_y = 100;
    int lower_pane_height = 200;

    // Derived (computed at runtime)
    int get_central_total_width() const {
        return window_width - left_pane_width - central_padding_x * 2;
    }
    int get_central_total_height() const {
        return window_height - lower_pane_height - central_padding_y * 2;
    }
    int get_tile_width() const {
        return get_central_total_width() / COLONY_WIDTH;
    }
    int get_tile_height() const {
        return get_central_total_height() / COLONY_HEIGHT;
    }
    int get_x_offset() const {
        return left_pane_width + get_central_total_width();
    }
    int get_y_offset() const {
        return central_padding_y;
    }

    // --- Population curve ---
    float pop_curve_padding_pct = 5.0f;
    float pop_curve_height_pct = 20.0f;
    int pop_curve_timepoints = 50;

    // --- Colors ---
    Color color_bg{103, 193, 202};
    Color color_ground{141, 191, 57};
    Color color_underground{155, 118, 83};
    Color color_grid{150, 150, 150};

    Color color_spore_male{250, 95, 95};
    Color color_spore_female{66, 158, 245};
    Color color_spore_outline{120, 120, 120};
    Color color_spore_selected{255, 255, 255};
    Color color_spore_left_mod{20, 20, 20, 0};
    Color color_spore_right_mod{20, 20, 20, 0};

    Color color_underground_left_mod{20, 20, 20, 0};
    Color color_underground_right_mod{20, 20, 20, 0};

    // --- Spores ---
    float spore_height_ratio = 0.3f;
    float underground_height_ratio = 0.05f;
    int spore_age_cap = -1;
    float spore_mood_recover_per_turn = 1.0f;
    float spore_health_recover_per_turn = 1.0f;

    // --- Reproduction ---
    int repro_event_pct = 10;
    int repro_min_age = 0;
    int repro_min_health = 90;
    int repro_min_mood = 80;
    int repro_conceive_rate = 50;

    // --- Fight ---
    int fight_event_pct = 1;
    int fight_max_damage = 10;
    int fight_fatal_pct = 10;

    // --- Chat ---
    int chat_event_pct = 89;
    int chat_happy_pct = 40;
    int chat_neutral_pct = 30;
    int chat_negative_pct = 30;
    int chat_mood_impact_max = 10;

    // --- Load from TOML file ---
    bool load(const std::string &path = "config.toml") {
        try {
            auto data = toml::parse_file(path);
            if (!data) return false;

            // General
            if (data.contains("general")) {
                auto &g = data.at("general");
                master_seed         = gtry<int>(g, "master_seed", master_seed);
                initial_spore_count = gtry<int>(g, "initial_spore_count", initial_spore_count);
            }

            // Viewer
            if (data.contains("viewer")) {
                auto &v = data.at("viewer");
                target_fps         = gtry<int>(v, "target_fps", target_fps);
                window_width       = gtry<int>(v, "width", window_width);
                window_height      = gtry<int>(v, "height", window_height);
                show_gridlines     = gtry<bool>(v, "show_gridlines", show_gridlines);
                show_spore_outline = gtry<bool>(v, "show_spore_outline", show_spore_outline);
            }

            // Simulation
            if (data.contains("simulation")) {
                auto &s = data.at("simulation");
                sim_tick_ms = gtry<int>(s, "sim_tick_ms", sim_tick_ms);
            }

            // Viewer panes
            if (data.contains("viewer") && data.at("viewer").contains("panes")) {
                auto &p = data.at("viewer.panes");
                left_pane_width   = gtry<int>(p, "left_pane_width", left_pane_width);
                central_padding_x = gtry<int>(p, "central_padding_x", central_padding_x);
                central_padding_y = gtry<int>(p, "central_padding_y", central_padding_y);
                lower_pane_height = gtry<int>(p, "lower_pane_height", lower_pane_height);
            }

            // Viewer curve
            if (data.contains("viewer") && data.at("viewer").contains("curve")) {
                auto &c = data.at("viewer.curve");
                pop_curve_padding_pct = gtry<double>(c, "padding_pct", pop_curve_padding_pct);
                pop_curve_height_pct  = gtry<double>(c, "height_pct", pop_curve_height_pct);
                pop_curve_timepoints  = gtry<int>(c, "timepoints", pop_curve_timepoints);
            }

            // Colors
            if (data.contains("colors")) {
                auto &col = data.at("colors");
                color_bg            = load_color(col, "background", color_bg);
                color_ground        = load_color(col, "ground", color_ground);
                color_underground   = load_color(col, "underground", color_underground);
                color_grid          = load_color(col, "grid", color_grid);

                if (col.contains("spore")) {
                    auto &s = col.at("spore");
                    color_spore_male       = load_color(s, "male", color_spore_male);
                    color_spore_female     = load_color(s, "female", color_spore_female);
                    color_spore_outline    = load_color(s, "outline", color_spore_outline);
                    color_spore_selected   = load_color(s, "selected", color_spore_selected);
                    color_spore_left_mod   = load_color(s, "left_modifier", color_spore_left_mod);
                    color_spore_right_mod  = load_color(s, "right_modifier", color_spore_right_mod);
                }

                if (col.contains("underground")) {
                    auto &u = col.at("underground");
                    color_underground_left_mod  = load_color(u, "left_modifier", color_underground_left_mod);
                    color_underground_right_mod = load_color(u, "right_modifier", color_underground_right_mod);
                }
            }

            // Spores
            if (data.contains("spores")) {
                auto &sp = data.at("spores");
                spore_height_ratio         = gtry<double>(sp, "height_ratio", spore_height_ratio);
                underground_height_ratio   = gtry<double>(sp, "underground_height_ratio", underground_height_ratio);
                spore_age_cap              = gtry<int>(sp, "age_cap", spore_age_cap);
                spore_mood_recover_per_turn  = gtry<double>(sp, "mood_recover_per_turn", spore_mood_recover_per_turn);
                spore_health_recover_per_turn = gtry<double>(sp, "health_recover_per_turn", spore_health_recover_per_turn);

                if (sp.contains("reproduction")) {
                    auto &r = sp.at("reproduction");
                    repro_event_pct      = gtry<int>(r, "event_pct", repro_event_pct);
                    repro_min_age        = gtry<int>(r, "min_age", repro_min_age);
                    repro_min_health     = gtry<int>(r, "min_health", repro_min_health);
                    repro_min_mood       = gtry<int>(r, "min_mood", repro_min_mood);
                    repro_conceive_rate  = gtry<int>(r, "conceive_rate", repro_conceive_rate);
                }

                if (sp.contains("fight")) {
                    auto &f = sp.at("fight");
                    fight_event_pct   = gtry<int>(f, "event_pct", fight_event_pct);
                    fight_max_damage  = gtry<int>(f, "max_damage", fight_max_damage);
                    fight_fatal_pct   = gtry<int>(f, "fatal_pct", fight_fatal_pct);
                }

                if (sp.contains("chat")) {
                    auto &ch = sp.at("chat");
                    chat_event_pct       = gtry<int>(ch, "event_pct", chat_event_pct);
                    chat_happy_pct       = gtry<int>(ch, "happy_pct", chat_happy_pct);
                    chat_neutral_pct     = gtry<int>(ch, "neutral_pct", chat_neutral_pct);
                    chat_negative_pct    = gtry<int>(ch, "negative_pct", chat_negative_pct);
                    chat_mood_impact_max = gtry<int>(ch, "mood_impact_max", chat_mood_impact_max);
                }
            }

            return true;
        } catch (const std::exception &e) {
            std::cerr << "Failed to load config.toml: " << e.what() << std::endl;
            std::cerr << "Using default values." << std::endl;
            return false;
        }
    }

private:
    // Helper: get a value from TOML table, falling back to default if missing
    template<typename T>
    T gtry(const toml::table &tbl, const std::string &key, T default_val) {
        try {
            return tbl.at(key).as<T>().value;
        } catch (...) {
            return default_val;
        }
    }

    // Helper: load a color {r, g, b} or {r, g, b, a} from TOML table
    Color load_color(const toml::table &tbl, const std::string &key, Color fallback) {
        try {
            auto &ct = tbl.at(key).as<table>().value;
            uint8_t r = ct.at("r").as<int32_t>().value;
            uint8_t g = ct.at("g").as<int32_t>().value;
            uint8_t b = ct.at("b").as<int32_t>().value;
            uint8_t a = 255;
            try { a = ct.at("a").as<int32_t>().value; } catch (...) {}
            return Color(r, g, b, a);
        } catch (...) {
            return fallback;
        }
    }
};

// Global config instance. Call cfg.load() in main() before using anything else.
Config cfg;
