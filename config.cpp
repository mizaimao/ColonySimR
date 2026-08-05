#pragma once
#include <string>
#include <set>
#include <stdexcept>
#include <iostream>
#include <type_traits>
#include "src/deps/toml11/toml.hpp"
#include "config.h"  // For Color struct and compile-time constants

// Runtime configuration loaded from config.toml.
// Accessed globally via `cfg` after calling cfg.load().
//
// Every field has a usable default, so a missing or unparseable config.toml is
// a warning rather than a fatal error. Keys that are misspelled or hold the
// wrong type are reported on stderr instead of being silently dropped.
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
    int get_sim_ticks_per_sec() const { return sim_tick_ms > 0 ? 1000 / sim_tick_ms : 0; }

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

    /**
     * Load config.toml over the defaults.
     *
     * Returns true if the file was parsed and applied, false if it could not be
     * read and the built-in defaults are in effect. Either way the returned
     * Config is usable -- the caller does not have to treat false as fatal.
     */
    bool load(const std::string &path = "config.toml") {
        toml::value data;
        try {
            data = toml::parse(path);
        } catch (const std::exception &e) {
            std::cerr << "config: could not read " << path << ": " << e.what() << "\n"
                      << "config: continuing with built-in defaults." << std::endl;
            return false;
        }

        seen_.clear();
        consumed_.clear();
        tables_.clear();

        Section root{&data, ""};

        if (Section g = sub(root, "general")) {
            master_seed         = get(g, "master_seed", master_seed);
            initial_spore_count = get(g, "initial_spore_count", initial_spore_count);
        }

        Section viewer = sub(root, "viewer");
        if (viewer) {
            target_fps         = get(viewer, "target_fps", target_fps);
            window_width       = get(viewer, "width", window_width);
            window_height      = get(viewer, "height", window_height);
            show_gridlines     = get(viewer, "show_gridlines", show_gridlines);
            show_spore_outline = get(viewer, "show_spore_outline", show_spore_outline);

            if (Section p = sub(viewer, "panes")) {
                left_pane_width   = get(p, "left_pane_width", left_pane_width);
                central_padding_x = get(p, "central_padding_x", central_padding_x);
                central_padding_y = get(p, "central_padding_y", central_padding_y);
                lower_pane_height = get(p, "lower_pane_height", lower_pane_height);
            }

            if (Section c = sub(viewer, "curve")) {
                pop_curve_padding_pct = get(c, "padding_pct", pop_curve_padding_pct);
                pop_curve_height_pct  = get(c, "height_pct", pop_curve_height_pct);
                pop_curve_timepoints  = get(c, "timepoints", pop_curve_timepoints);
            }
        }

        if (Section s = sub(root, "simulation")) {
            sim_tick_ms = get(s, "sim_tick_ms", sim_tick_ms);
        }

        Section colors = sub(root, "colors");
        if (colors) {
            color_bg          = color(colors, "background", color_bg);
            color_ground      = color(colors, "ground", color_ground);
            color_underground = color(colors, "underground_color", color_underground);
            color_grid        = color(colors, "grid", color_grid);

            if (Section s = sub(colors, "spore")) {
                color_spore_male      = color(s, "male", color_spore_male);
                color_spore_female    = color(s, "female", color_spore_female);
                color_spore_outline   = color(s, "outline", color_spore_outline);
                color_spore_selected  = color(s, "selected", color_spore_selected);
                color_spore_left_mod  = color(s, "left_modifier", color_spore_left_mod);
                color_spore_right_mod = color(s, "right_modifier", color_spore_right_mod);
            }

            if (Section u = sub(colors, "underground")) {
                color_underground_left_mod  = color(u, "left_modifier", color_underground_left_mod);
                color_underground_right_mod = color(u, "right_modifier", color_underground_right_mod);
            }
        }

        Section spores = sub(root, "spores");
        if (spores) {
            spore_height_ratio            = get(spores, "height_ratio", spore_height_ratio);
            underground_height_ratio      = get(spores, "underground_height_ratio", underground_height_ratio);
            spore_age_cap                 = get(spores, "age_cap", spore_age_cap);
            spore_mood_recover_per_turn   = get(spores, "mood_recover_per_turn", spore_mood_recover_per_turn);
            spore_health_recover_per_turn = get(spores, "health_recover_per_turn", spore_health_recover_per_turn);

            if (Section r = sub(spores, "reproduction")) {
                repro_event_pct     = get(r, "event_pct", repro_event_pct);
                repro_min_age       = get(r, "min_age", repro_min_age);
                repro_min_health    = get(r, "min_health", repro_min_health);
                repro_min_mood      = get(r, "min_mood", repro_min_mood);
                repro_conceive_rate = get(r, "conceive_rate", repro_conceive_rate);
            }

            if (Section f = sub(spores, "fight")) {
                fight_event_pct  = get(f, "event_pct", fight_event_pct);
                fight_max_damage = get(f, "max_damage", fight_max_damage);
                fight_fatal_pct  = get(f, "fatal_pct", fight_fatal_pct);
            }

            if (Section ch = sub(spores, "chat")) {
                chat_event_pct       = get(ch, "event_pct", chat_event_pct);
                chat_happy_pct       = get(ch, "happy_pct", chat_happy_pct);
                chat_neutral_pct     = get(ch, "neutral_pct", chat_neutral_pct);
                chat_negative_pct    = get(ch, "negative_pct", chat_negative_pct);
                chat_mood_impact_max = get(ch, "mood_impact_max", chat_mood_impact_max);
            }
        }

        // Anything in the file we never looked at is almost certainly a typo.
        report_unknown(data, "");
        return true;
    }

private:
    // A table in the parsed document, paired with its dotted path so warnings
    // can name the offending key exactly as it appears in config.toml.
    struct Section {
        const toml::value *v = nullptr;
        std::string path;
        explicit operator bool() const { return v != nullptr; }
    };

    std::set<std::string> seen_;      // scalar keys this loader knows about
    std::set<std::string> consumed_;  // subtrees handled wholesale (colors)
    std::set<std::string> tables_;    // subtables this loader descends into

    static std::string join(const std::string &a, const std::string &b) {
        return a.empty() ? b : a + "." + b;
    }

    void warn(const std::string &key, const std::string &msg) {
        std::cerr << "config: ignoring '" << key << "': " << msg << std::endl;
    }

    // Descend into a subtable, registering it as known even when absent.
    Section sub(const Section &parent, const std::string &key) {
        const std::string full = join(parent.path, key);
        tables_.insert(full);
        if (!parent.v || !parent.v->contains(key)) return {};
        const toml::value &v = parent.v->at(key);
        if (!v.is_table()) {
            warn(full, "expected a [table]");
            return {};
        }
        return {&v, full};
    }

    // Read one scalar, falling back to the default and warning on a type error.
    template<typename T>
    T get(const Section &sec, const std::string &key, T default_val) {
        const std::string full = join(sec.path, key);
        seen_.insert(full);
        if (!sec.v || !sec.v->contains(key)) return default_val;
        const toml::value &v = sec.v->at(key);
        try {
            if constexpr (std::is_floating_point_v<T>) {
                // TOML separates 5 from 5.0; for a ratio or a percentage nobody
                // means anything different by the two, so take either.
                if (v.is_integer()) return static_cast<T>(v.as_integer());
                return static_cast<T>(toml::get<double>(v));
            } else {
                return toml::get<T>(v);
            }
        } catch (const std::exception &e) {
            warn(full, e.what());
            return default_val;
        }
    }

    // Read a color table -- {r, g, b} or {r, g, b, a}.
    Color color(const Section &sec, const std::string &key, Color fallback) {
        const std::string full = join(sec.path, key);
        consumed_.insert(full);  // r/g/b/a are handled here, don't flag them below
        if (!sec.v || !sec.v->contains(key)) return fallback;
        const toml::value &ct = sec.v->at(key);
        try {
            if (!ct.is_table()) {
                warn(full, "expected { r = .., g = .., b = .. }");
                return fallback;
            }
            for (const auto &kv : ct.as_table()) {
                if (kv.first != "r" && kv.first != "g" && kv.first != "b" && kv.first != "a") {
                    warn(join(full, kv.first), "unknown color channel");
                }
            }
            const int r = toml::find<int>(ct, "r");
            const int g = toml::find<int>(ct, "g");
            const int b = toml::find<int>(ct, "b");
            const int a = ct.contains("a") ? toml::find<int>(ct, "a") : 255;
            for (const int channel : {r, g, b, a}) {
                if (channel < 0 || channel > 255) {
                    warn(full, "color channels must be between 0 and 255");
                    return fallback;
                }
            }
            return Color((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
        } catch (const std::exception &e) {
            warn(full, e.what());
            return fallback;
        }
    }

    // Walk the parsed document and report anything the loader never read.
    void report_unknown(const toml::value &v, const std::string &path) {
        if (!v.is_table()) return;
        for (const auto &kv : v.as_table()) {
            const std::string full = join(path, kv.first);
            if (consumed_.count(full) || seen_.count(full)) continue;
            if (kv.second.is_table()) {
                if (tables_.count(full)) report_unknown(kv.second, full);
                else warn(full, "unknown section");
            } else {
                warn(full, "unknown key");
            }
        }
    }
};

// Global config instance. Call cfg.load() in main() before using anything else.
// `inline` so that including this file from more than one translation unit does
// not produce duplicate symbols.
inline Config cfg;
