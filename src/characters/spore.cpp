#pragma once
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <map>

#include "src/terrain/terrain_map.cpp"
#include "utils.cpp"
#include "config.h"

using namespace::std;

// Spore definition.
class Spore {
    public:
        int sex;
        int age;
        int spore_id;
        float health;
        float mood;
        int loc_x;
        int loc_y;
        bool moved = false;  // Indicates if this spore has moved (for drawing).
        //vector< array<int, 3> > route;

        Spore(int i_sex, int i_spore_id, int i_loc_x, int i_loc_y, float f_health = 100.0f, int i_age = 0, int f_mood = 100.0f){
            sex = i_sex;
            spore_id = i_spore_id;
            age = i_age;
            loc_x = i_loc_x;
            loc_y = i_loc_y;
            health = f_health;
            age = i_age;
            mood = f_mood;
        };
};

// Controls all spores' behavior as a collection.
class SporeManager {
    public:
        vector<Spore> spores_v;
        int current_spore_id = 0;
        int current_population = 0;
        BitMap * bitmap = nullptr;
        mt19937 rng;

        vector<Spore *> spore_by_loc[COLONY_HEIGHT][COLONY_WIDTH];

        SporeManager(
            int initial_pop, BitMap * r_bitmap, int pop_cap = 200, int seed = 720
        ){
            rng.seed(MASTER_SEED);
            bitmap = r_bitmap;
            for (int i = 0; i < initial_pop; i++){
                create_a_spore();
            }
        };

        int create_a_spore(
            int i_sex = -1, int i_spore_id = -1, int i_loc_x = -1, int i_loc_y = -1,
            float f_health = 100.0f, int i_age = 0
        ){
            if (current_population >= POPULATION_CAP)
                return 1;

            int spore_sex = i_sex;
            int spore_id = i_spore_id;
            int spore_loc_x = i_loc_x;
            int spore_loc_y = i_loc_y;
            float spore_health = f_health;
            int spore_age = i_age;

            if (spore_sex == -1){
                spore_sex = rng() % 2;
            }
            if (spore_id == -1){
                spore_id = current_spore_id;
                current_spore_id += 1;
            }
            if (spore_loc_x == -1 or spore_loc_y == -1)
                get_random_loc(spore_loc_x, spore_loc_y);

            vector<Spore *> & location_vector = spore_by_loc[spore_loc_y][spore_loc_x];
            Spore * new_spore = new Spore(spore_sex, spore_id, spore_loc_x, spore_loc_y, spore_health, spore_age);
            location_vector.push_back( new_spore );

            //cout << "created a spore with id " << location_vector.end().spore << endl;
            current_population++;
            return 0;
        }

        // Acquire a pair of random locations.
        int get_random_loc(int & loc_x, int & loc_y){
            loc_x = rng() % bitmap->width;
            loc_y = rng() % bitmap->height;
            return 0;
        }

        // Move a spore.
        void spore_move(int & x, int & y){
            int x_movement = rng() % 3;
            int y_movement = rng() % 3;
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
        
        // Remove a spore. For whatever reason we delist this spore from our humble colony.
        int remove_spore(){
            return 1;
        }

        // Check if a spore is reproduction-ready.
        bool reproduction_check(Spore & spore){
            bool reproducible = true;
            if (SPORE_REPRODUCTION_AGE_LOW > spore.age or spore.age > SPORE_REPRODUCTION_AGE_HIGH){
                reproducible = false;
            }
            if (spore.health < SPORE_REPRODUCTION_HEALTH_REQ){
                reproducible = false;
            }
            if (spore.mood < SPORE_REPRODUCTION_MOOD_REQ){
                reproducible = false;
            }
            return reproducible;
        }

        // Determines outcome of a spore fight for one side. Need to call twice to cover both sides.
        // Death is not handled here but in the step function.
        void spore_fight_impact(Spore & spore){
            if ((rng() % 100) < SPORE_FIGHT_FATAL_PCT){  // Fatal fight.
                spore.health = 0.0f;
            } else {
                spore.health -= rng() % SPORE_FIGHT_DAMAGE_IMPACT_MAX;
            }
        }

        // Determines outcome of a spore conversation.
        void spore_chat_impact(Spore & spore){
            int roll = rng() % 100;
            if (roll < SPORE_CHAT_HAPPINESS_CHANCE){
                spore.mood = min(100.0f, spore.mood + rng() % SPORE_CHAT_MOOD_IMPACT_MAX);
            } else if (SPORE_CHAT_HAPPINESS_CHANCE <= roll and roll <= SPORE_CHAT_HAPPINESS_CHANCE + SPORE_CHAT_NEUTRAL_CHANCE){
                // Nothing happens due to a neutral conversation.
            } else {
                spore.mood = max(0.0f, spore.mood - rng() % SPORE_CHAT_MOOD_IMPACT_MAX);
            }
        }

        // Some interaction may occur among spores on the same tile.
        int spore_interaction_on_the_same_tile(
            vector<Spore *> & location_vector,  // Tile vector.
            int n_interactions = 1  // How many interactions to generate.
        ){
            int spore_count = location_vector.size();
            if (spore_count < 2) {  // Too few spores to make an interaction.
                return 0;
            }
            for (int i_interaction = 0; i_interaction < n_interactions; i_interaction++){
                int first_spore_i = rng() % spore_count;
                int second_spore_i = rng() % spore_count;
                if (first_spore_i == second_spore_i){
                    continue;
                }
                Spore & first_spore = * location_vector[first_spore_i];
                Spore & second_spore = * location_vector[second_spore_i];

                // Decide an event.
                int event_roll = rng() % 100;
                int accumulator = SPORE_REPRODUCTION_PCT;

                // Reproduction event.
                if (0 <= event_roll and event_roll < SPORE_REPRODUCTION_PCT){
                    // Opposite sex.
                    if (first_spore.sex + second_spore.sex == 1 and reproduction_check(first_spore) and reproduction_check(second_spore)){
                        // Now roll a die to decide if a new spore can be created.
                        if ((rng() % 100) < SPORE_REPRODUCTION_CONCEIVE_RATE){
                            if (create_a_spore()){
                                print("No new spore created due to pop cap.");
                            } else {
                                print("New spore created: ", current_spore_id);
                            }
                        }
                    }
                // Fight event.
                } else if (SPORE_REPRODUCTION_PCT <= event_roll and event_roll <= SPORE_REPRODUCTION_PCT + SPORE_FIGHT_PCT){
                    // Both are males.
                    if (first_spore.sex + second_spore.sex == 2){
                        spore_fight_impact(first_spore);
                        spore_fight_impact(second_spore);
                    }
                // Chat event.
                } else if (SPORE_REPRODUCTION_PCT + SPORE_FIGHT_PCT <= event_roll and event_roll < 100){
                    spore_chat_impact(first_spore);
                    spore_chat_impact(second_spore);
                } else {
                    // Place holder for future events.
                }

            }
            return 0;
        }

        /**
         * Called for each spore per turn. Checks if it still survives and update its mood and health.
         * Returns 1 if it should be dead.
        */
        bool update_spore_status(Spore & spore){
            if (spore.age++ > SPORE_AGE_CAP and SPORE_AGE_CAP > 0){
                print("Spore ", spore.spore_id, " got killed due to aging.");
                return true;
            }
            if (spore.health <= 0.0f){
                print("Spore ", spore.spore_id, " got killed due to low health.");
                return true;
            }
            spore.health = min(100.0f, spore.health + SPORE_HEALTH_RECOVER_PER_TURN);
            spore.mood = min(100.0f, spore.mood + SPORE_MOOD_RECOVER_PER_TURN);
            return false;
        }

        // Move all spores.
        int step_spores(){
            // Go through by tiles.
            for (int xx = 0; xx < COLONY_WIDTH; xx++){
                for (int yy = 0; yy < COLONY_HEIGHT; yy++){
                    vector<Spore *> & location_vector = spore_by_loc[yy][xx];
                    
                    // Trigger spore interactions on that tile.
                    spore_interaction_on_the_same_tile(location_vector);

                    // Log their original location
                    int i = location_vector.size() - 1;
                    for (
                        auto it = location_vector.end(); it != location_vector.begin(); it--, i--){
                    // for (Spore * p_spore : location_vector){
                        Spore * p_spore = location_vector[i];
                        Spore & spore = * p_spore;

                        if (spore.moved){
                            continue;
                        }
                        if (update_spore_status(spore)){  // Returns 1 if a spore should be dead.
                            location_vector.erase(location_vector.begin() + i);  // Remove this spore from vector.
                            delete p_spore;
                            current_population--;
                            continue;
                        }

                        spore_move(spore.loc_x, spore.loc_y);
                        spore.moved = true;

                        if ((spore.loc_x != xx) or (spore.loc_y != yy)){  // The spore has moved location.
                            vector<Spore *> & new_location_vector = spore_by_loc[spore.loc_y][spore.loc_x];
                            new_location_vector.push_back(p_spore);  // So add that spore to new location.
                            location_vector.erase(location_vector.begin() + i);  // And remove it from current one. 
                        }
                    }
                }
            }

            for (int xx = 0; xx < COLONY_WIDTH; xx++){
                for (int yy = 0; yy < COLONY_HEIGHT; yy++){
                    vector<Spore *> & location_vector = spore_by_loc[yy][xx];
                    for (Spore * p_spore : location_vector){
                        Spore & spore = * p_spore;
                        spore.moved = false;
                    }
                }
            }
            return 0;
        }
};