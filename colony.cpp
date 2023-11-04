#pragma once
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>

#include "src/terrain/terrain_map.cpp"
#include "src/characters/spore.cpp"
#include "config.h"

using namespace std;

class Colony{
    public:
        SporeManager * spore_man = nullptr;
        BitMap * bitmap = nullptr;
        
        bool is_realtime = false;
        sf::Time residual_time = sf::seconds(0.0f);


        Colony(){
            start_colony();

        }

        int start_colony(){
            std::cout << "col manager called " << std::endl;
            BitMap init_bitmap(COLONY_WIDTH, COLONY_HEIGHT);
            bitmap = &init_bitmap;
            SporeManager init_spore_man(10, &init_bitmap);
            spore_man = &init_spore_man;
            return 0;
        }

        Colony(SporeManager * p_spore_man, BitMap * p_bitmap){
            spore_man = p_spore_man;
            bitmap = p_bitmap;
        }

        int step_colony(){
            int spore_man_code = spore_man->step_spores();

            return 0;
        }
};