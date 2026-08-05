#include "src/vis/viewer.cpp"
#include "colony.cpp"
#include "config_validation.cpp"


int main(){

    // Load configuration from config.toml. A missing or unreadable file is not
    // fatal -- cfg.load() reports it and leaves the built-in defaults in place.
    cfg.load();

    // Check configuration validity.
    if (config_check()){
        return 1;
    }
    else{
        print("Chicken!");
    }

    BitMap init_bitmap(COLONY_WIDTH, COLONY_HEIGHT);
    SporeManager init_spore_man(cfg.initial_spore_count, &init_bitmap, POPULATION_CAP, cfg.master_seed);

    Colony colony(&init_spore_man, &init_bitmap);
    Viewer viewer(&colony);
    viewer.launch_window();

    return 0;
}
