#pragma once
#include <iostream>
#include <random>

#include "src/terrain/terrain_map.cpp"
#include "src/characters/spore.cpp"
#include "config.h"

using namespace std;

class Colony{
public:
    SporeManager * spore_man = nullptr;
    BitMap * bitmap = nullptr;

    bool is_realtime = false;
    double residual_time_ms = 0.0;

    Colony(SporeManager * p_spore_man, BitMap * p_bitmap){
        spore_man = p_spore_man;
        bitmap = p_bitmap;
    }

    int step_colony(){
        int spore_man_code = spore_man->step_spores();

        if (spore_man_code == 1){
            print("Spore step error.");
            return 1;
        }

        return 0;
    }
};