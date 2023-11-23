#pragma once
#include <deque>
#include "config.h"

class StatCurve{
private:
    float y_padding = 10.f;  // Relative padding of drawing area height. 10.f means 10%.
    float x_padding = 10.f;  // Relative padding of width of drawing area.

    float x_offset = 0.f;  // Width times padding one side.
    float y_offset = 0.f;

    float drawable_width = 0.f;  // Width deducting double padding on both sides.
    float drawable_height = 0.f;

public:

    int pos_x = 0;
    int pos_y = 0;
    int width = 0;
    int height = 0;
    int data_points = 0;

    // Used to normalize curve height.
    int prev_high = 0;

    // These two arrays stores X, Y coordinates for each point.
    deque<int> data_x;
    deque<int> data_y;
    deque<int> pop_history;


    StatCurve(int pos_xx, int pos_yy, int size_x, int size_y, int i_data_points){
        pos_x = pos_xx;
        pos_y = pos_yy;
        width = size_x;
        height = size_y;
        data_points = i_data_points;

        x_offset = (float)width * x_padding / 100.f;
        y_offset = (float)height * y_padding / 100.f;
        drawable_width = (float)width * (1.f - x_padding * 2 / 100.f);
        drawable_height = (float)height * (1.f - y_padding * 2 / 100.f);
        // data_x.resize(data_points);
        // data_y.resize(data_points);
        // pop_history.resize(data_points);

        // Calculate X points of all data.
        float x_interval = drawable_width / data_points;
        for (int i = 0; i < data_points; i++){
            data_x.push_back(
                (int)(pos_x + x_offset + i * x_interval)
            );
            data_y.push_back(0);
            pop_history.push_back(0);
        }
    };

    // Calculate height of a point.
    float calculate_height(float current, float prev_high){
        float relative_y = (float)drawable_height * current / prev_high;
        return pos_y + y_offset + (drawable_height - (int)relative_y);
    }
    

    // Add current population into array and recalculate xy if necessary.
    void add_point(int data){
        // Remove the first elements. Leave X positions because they don't really change.
        if (pop_history.size() == data_points){
            pop_history.pop_front();
            data_y.pop_front();
            pop_history.push_back(data);
        }

        // Re-calculate height of each point.
        if (data > prev_high){
            prev_high = data;
            for (int i = 0; i < data_points - 1; i++){
                data_y[i] = calculate_height(pop_history[i], prev_high);
            }
        }
        // Calculate only the newly added point.
        data_y.push_back( calculate_height(data, prev_high) );

    }

};