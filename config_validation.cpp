/**
 * Sanity check of the runtime configuration (loaded from config.toml).
 * Called AFTER cfg.load() succeeds.
*/
#pragma once
#include <iostream>

#include "config.h"
#include "config.cpp"  // For Config class and global `cfg`
#include "utils.cpp"


int config_check(){
    // Colony general settings.
    if (COLONY_HEIGHT != COLONY_WIDTH){
        print("Colony for now supports only COLONY_HEIGHT==COLONY_WIDTH.");
        return 1;
    }
    if (cfg.initial_spore_count < 0){
        print("Initial spore count cannot be negative.");
        return 1;
    }
    if (COLONY_WIDTH * COLONY_HEIGHT < cfg.initial_spore_count){
        print("Colony has too many initial spores.");
        return 1;
    }
    if (cfg.initial_spore_count > POPULATION_CAP){
        print("Initial population exceeds cap.");
        return 1;
    }

    // Timing. Both of these are divisors, so zero is not merely meaningless.
    if (cfg.sim_tick_ms <= 0){
        print("[simulation] sim_tick_ms must be greater than 0.");
        return 1;
    }
    if (cfg.target_fps <= 0){
        print("[viewer] target_fps must be greater than 0.");
        return 1;
    }

    // Window geometry. The panes and padding have to leave a drawable area
    // behind, otherwise the tile size collapses to zero.
    if (cfg.window_width <= 0 or cfg.window_height <= 0){
        print("[viewer] width and height must be greater than 0.");
        return 1;
    }
    if (cfg.get_central_total_width() < COLONY_WIDTH or cfg.get_central_total_height() < COLONY_HEIGHT){
        print("[viewer.panes] leaves no room for the colony; reduce the padding or pane sizes.");
        return 1;
    }
    if (cfg.pop_curve_timepoints <= 0){
        print("[viewer.curve] timepoints must be greater than 0.");
        return 1;
    }

    // Spore config check.
    if (cfg.chat_happy_pct + cfg.chat_neutral_pct + cfg.chat_negative_pct != 100){
        print("Spore chat event sum is not equal to 100.");
        return 1;
    }
    if (cfg.repro_event_pct + cfg.fight_event_pct + cfg.chat_event_pct != 100){
        print("Spore events chances not summing up to 100.");
        return 1;
    }

    // These two bound a modulo in the event handlers, so zero would divide by 0.
    if (cfg.fight_max_damage <= 0){
        print("[spores.fight] max_damage must be greater than 0.");
        return 1;
    }
    if (cfg.chat_mood_impact_max <= 0){
        print("[spores.chat] mood_impact_max must be greater than 0.");
        return 1;
    }

    return 0;
}
