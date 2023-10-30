#pragma once
#include <vector>

using namespace std;

class BitMap{
    public:
        int width;
        int height;
        vector< vector<int> > bitmap;

        BitMap(int i_width, int i_height){
            width = i_width;
            height = i_height;
            
            bitmap.resize(height, vector<int>(width, 0));
            ;
        };


};