#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cstdint>

#include "src/terrain/terrain_map.cpp"

using namespace::std;

// Spore definition.
class Spore {
    public:
        int sex;
        int age;
        int spore_id;
        float health;
        int loc_x;
        int loc_y;
        vector< array<int, 3> > route;

        Spore(int i_sex, int i_spore_id, int i_loc_x, int i_loc_y, float f_health = 100.0f, int i_age = 0){
            sex = i_sex;
            spore_id = i_spore_id;
            age = i_age;
            loc_x = i_loc_x;
            loc_y = i_loc_y;
            health = f_health;
            age = i_age;
        };
};

// Controls all spores' behavior as a collection.
class SporeManager {
    public:
        vector<Spore> spores_v;
        int current_spore_id = 0;
        int current_population = 0;
        BitMap * bitmap = nullptr;

        SporeManager(
            int initial_pop, BitMap * r_bitmap, int pop_cap = 200, int seed = 720
        ){
            bitmap = r_bitmap;
            for (int i = 0; i < initial_pop; i++){
                create_a_spore();
            }
        };

        int create_a_spore(
            int i_sex = -1, int i_spore_id = -1, int i_loc_x = -1, int i_loc_y = -1,
            float f_health = 100.0f, int i_age = 0
        ){
            int spore_sex = i_sex;
            int spore_id = i_spore_id;
            int spore_loc_x = i_loc_x;
            int spore_loc_y = i_loc_y;
            float spore_health = f_health;
            int spore_age = i_age;

            if (spore_sex == -1){
                spore_sex = rand() % 2;
            }
            if (spore_id == -1){
                spore_id = current_spore_id;
                current_spore_id += 1;
            }
            if (spore_loc_x == -1 or spore_loc_y == -1)
                get_random_loc(spore_loc_x, spore_loc_y);

            spores_v.push_back(Spore(spore_sex, spore_id, spore_loc_x, spore_loc_y, spore_health, spore_age));
            current_population++;
            return 0;
        }

        // Acquire a pair of random locations.
        int get_random_loc(int & loc_x, int & loc_y){
            loc_x = rand() % bitmap->width;
            loc_y = rand() % bitmap->height;
            return 0;
        }

        // Move a spore.
        void spore_move(int & x, int & y){
            int x_movement = rand() % 3;
            int y_movement = rand() % 3;
            switch (x_movement){
                case (0):
                    break;
                case (1):
                    if (0 < x)
                        x--;
                    break;
                case (2):
                    if (x + 1 < bitmap->width)
                        x++;
                    break;
            };
            switch (y_movement){
                case (0):
                    break;
                case (1):
                    if (0 < y)
                        y--;
                    break;
                case (2):
                    if (y + 1 < bitmap->height)
                        y++;
                    break;
            };
        }

        // Move all spores.
        int step_spores(){
            for (auto & spore : spores_v){
                spore_move(spore.loc_x, spore.loc_y);
            }
            return 0;
        }

};