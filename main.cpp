#include <iostream>
#include "src/vis/viewer.cpp"
#include "colony.cpp"
#include "config_validation.cpp"


int main(){

    // Check configuration file.
    if (config_check()){
        return 1;
    }
    else{
        print("Chicken!");
    }

    BitMap init_bitmap(COLONY_WIDTH, COLONY_HEIGHT);
    SporeManager init_spore_man(INITIAL_SPORE_COUNT, &init_bitmap);

    Colony colony(&init_spore_man, &init_bitmap);
    Viewer viewer(&colony);
    viewer.launch_window();

    return 0;
}