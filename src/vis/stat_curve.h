#pragma once
#include <deque>

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
    std::deque<int> data_x;
    std::deque<int> data_y;
    std::deque<int> pop_history;


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
            // Start on the baseline rather than at y=0, which would draw a flat
            // line across the top of the pane until the first sample arrives.
            data_y.push_back((int)calculate_height(0.f, 0.f));
            pop_history.push_back(0);
        }
    };

    // Calculate the screen height of a point, given the peak it is scaled against.
    float calculate_height(float current, float peak){
        if (peak <= 0.f){  // Nothing has been recorded yet -- sit on the baseline.
            return pos_y + y_offset + drawable_height;
        }
        float relative_y = drawable_height * current / peak;
        return pos_y + y_offset + (drawable_height - (int)relative_y);
    }

    // Add current population into array and recalculate xy if necessary.
    void add_point(int data){
        // Drop the oldest sample. X positions are left alone; they never change.
        pop_history.push_back(data);
        while ((int)pop_history.size() > data_points){
            pop_history.pop_front();
        }

        // A new peak rescales every point, so just recompute the whole window.
        // data_points is small (50 by default), this is not worth optimizing.
        if (data > prev_high){
            prev_high = data;
        }
        data_y.clear();
        for (int i = 0; i < (int)pop_history.size(); i++){
            data_y.push_back((int)calculate_height((float)pop_history[i], (float)prev_high));
        }
    }

};