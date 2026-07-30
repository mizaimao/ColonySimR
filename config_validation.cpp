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
    if (COLONY_WIDTH * COLONY_HEIGHT < cfg.initial_spore_count){
        print("Colony has too many initial spores.");
        return 1;
    }
    if (cfg.initial_spore_count > POPULATION_CAP){
        print("Initial population exceeds cap.");
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

    return 0;
}
