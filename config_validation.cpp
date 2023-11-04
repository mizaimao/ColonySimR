/**
 * Sanity check of the configuration file.
*/
#pragma once
#include <iostream>

#include "config.h"
#include "utils.cpp"


int config_check(){
    // Colony general settings.
    if (COLONY_HEIGHT != COLONY_WIDTH){
        print("Colony for now supports only COLONY_HEIGHT==COLONY_WIDTH.");
        return 1;
    }
    if (COLONY_WIDTH * COLONY_HEIGHT < INITIAL_SPORE_COUNT){
        print("Colony have too many initial spores.");
        return 1;
    }
    if (INITIAL_SPORE_COUNT > POPULATION_CAP){
        print("Inital population exceeds cap.");
        return 1;
    }

    // Spore config check.
    if (SPORE_CHAT_HAPPINESS_CHANCE + SPORE_CHAT_NEUTRAL_CHANCE + SPORE_CHAT_NEGATIVE_CHANCE != 100){
        print("Spore chat event sum is not equal to 100.");
        return 1;
    }
    if (SPORE_REPRODUCTION_PCT + SPORE_FIGHT_PCT + SPORE_CHAT_CHANCE != 100){
        print("Spore events chances not summing up to 100.");
        return 1;
    }

    return 0;
}