#include <iostream>
#include "src/vis/viewer.cpp"
#include "colony.cpp"
#include <typeinfo>

int main(){

    BitMap init_bitmap(COLONY_WIDTH, COLONY_HEIGHT);
    SporeManager init_spore_man(10, &init_bitmap);

    std::cout << "Chicken" << std::endl;

    Colony colony(&init_spore_man, &init_bitmap);
    // std::cout << "colony sutddff " << colony.spore_man->current_population << std::endl;
    // std::cout << "colony sutddff " << colony.bitmap->height  << std::endl;
    Viewer viewer(&colony);
    viewer.launch_window();

    return 0;
}