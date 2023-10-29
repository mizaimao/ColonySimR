#include <iostream>
#include "src/vis/viewer.cpp"

int main(){

    std::cout << "Chicken" << std::endl;

    Viewer viewer;
    viewer.launch_window();

    return 0;
}