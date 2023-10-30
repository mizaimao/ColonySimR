#pragma once
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "config.h"
#include "colony.cpp"

using namespace::std;


class Viewer {
    private:
        int frame_counter = 0;
        sf::Time zero_second = sf::seconds(0.0f);
        sf::Time render_time = sf::seconds(1.0f / TARGET_FPS);
        sf::Clock clock;

        sf::Font font;
        sf::Event event;
        sf::Texture frame_texture;
        sf::Sprite sprite;
        Colony * colony = nullptr;
        sf::Color color_map_arr[2];
        sf::Text fps_counter_text;


        map<string, sf::VertexArray> va_map;

        int tile_width = 0;
        int tile_height = 0;
        int base_tile_x = 0;
        int base_tile_y = 0;
        int grid_line_count = COLONY_WIDTH + 1 + COLONY_HEIGHT + 1;

    public:
        int width = WIDTH;
        int height = HEIGHT;

    Viewer(Colony * p_colony){
        colony = p_colony;

        frame_texture.create(WIDTH, HEIGHT);
        //va_map.insert(make_pair("dwd", sf::VertexArray()));
        // va_map["background"] = (sf::VertexArray(sf::Triangles, 2 * 3));
        va_map["ground"] = (sf::VertexArray(sf::Triangles, 6 * 3));
        va_map["spores"] = (sf::VertexArray(sf::Triangles, 200 * 6 * 3));
        va_map["grid_lines"] = (sf::VertexArray(sf::Lines, grid_line_count * 4));

        // Adjust color map.
        color_map_arr[0] = RED_SPORE;
        color_map_arr[1] = BLUE_SPORE;

        determine_base_tile_loc();
        draw_static_ground();
        draw_grid_lines();

        draw_spore(0, 0, 0, BLUE_SPORE);
        draw_spore(9, 9, 1, RED_SPORE);
        draw_all_spores_from_colony();
    };

    int modify_pixel_shader(
        int x,
        int y,
        sf::Color pixel_color,
        float scale,
        sf::VertexArray & target_va,
        int x_offset = 0,
        int y_offset = 0
    ){
        int tgt_index = (y * 256 + x) * 6;  // Two triangles, six vertices.
        sf::Vertex* triangles = &target_va[tgt_index];
        triangles[0].color = pixel_color;
        triangles[1].color = pixel_color;
        triangles[2].color = pixel_color;
        triangles[3].color = pixel_color;
        triangles[4].color = pixel_color;
        triangles[5].color = pixel_color;

        triangles[0].position = sf::Vector2f( x * scale, y * scale) ;
        triangles[1].position = sf::Vector2f( x * scale, (y + 1) * scale );
        triangles[2].position = sf::Vector2f( (x + 1) * scale, y * scale );
        triangles[3].position = sf::Vector2f( (x + 1) * scale, y * scale );
        triangles[4].position = sf::Vector2f( (x + 1) * scale, (y + 1) * scale );
        triangles[5].position = sf::Vector2f( x * scale, (y + 1) * scale );
        return 0;
    }

    int determine_base_tile_loc(){
        if (min(TILE_WIDTH, TILE_HEIGHT) == TILE_WIDTH){  // Bound by width.
            tile_width = TILE_WIDTH;
            tile_height = (int)( TILE_WIDTH / 2 );
        } else {
            tile_height = TILE_HEIGHT;
            tile_width = (int)( TILE_HEIGHT * 2);
        }
        base_tile_x = LEFT_PANE_WIDTH  + (COLONY_WIDTH / 2) * tile_width;
        base_tile_y = Y_OFFSET;
        return 0;
    }

    int get_tile_loc(int x, int y, float & mapped_x, float & mapped_y){
        mapped_x = (x - y) * tile_width / 2 + base_tile_x;
        mapped_y = (x + y) * tile_height / 2 + base_tile_y;
        return 0;
    }

    int draw_static_ground(){
        sf::VertexArray & bg_va = va_map.at("ground");
        for (int i = 0; i < 6; i ++)
            bg_va[i].color = GROUND_COLOR;
        for (int i = 6; i < 12; i++)
            bg_va[i].color = UNDERGROUND_COLOR + UNDERGROUND_LEFT_COLOR_MODIFIER;
        for (int i = 12; i < 18; i++)
            bg_va[i].color = UNDERGROUND_COLOR - UNDERGROUND_RIGHT_COLOR_MODIFIER;
    
        float left_x, left_y;
        float right_x, right_y;
        float top_x, top_y;
        float bottom_x, bottom_y;
        get_tile_loc(0, COLONY_HEIGHT, left_x, left_y);
        get_tile_loc(COLONY_WIDTH, 0, right_x, right_y);
        get_tile_loc(0, 0, top_x, top_y);
        get_tile_loc(COLONY_WIDTH, COLONY_HEIGHT, bottom_x, bottom_y);

        // cout << left_x << " " <<  left_y << endl;
        // cout << right_x << " " <<  right_y << endl;
        // cout << top_x << " " <<  top_y << endl;
        // cout << bottom_x << " " <<  bottom_y << endl;
        
        bg_va[0].position = sf::Vector2f( left_x, left_y);
        bg_va[1].position = sf::Vector2f( top_x, top_y);
        bg_va[2].position = sf::Vector2f( bottom_x, bottom_y);
        bg_va[3].position = sf::Vector2f( bottom_x, bottom_y);
        bg_va[4].position = sf::Vector2f( right_x, right_y);
        bg_va[5].position = sf::Vector2f( top_x, top_y);

        // Draw underground.
        float under_height = UNDERGROUND_HEIGHT_REL * CENTRAL_TOTAL_HEIGHT;
        bg_va[6].position = sf::Vector2f( left_x, left_y);
        bg_va[7].position = sf::Vector2f( left_x, left_y + under_height );
        bg_va[8].position = sf::Vector2f( bottom_x, bottom_y);
        bg_va[9].position = sf::Vector2f( bottom_x, bottom_y);
        bg_va[10].position = sf::Vector2f( bottom_x, bottom_y + under_height );
        bg_va[11].position = sf::Vector2f( left_x, left_y + under_height);

        bg_va[12].position = sf::Vector2f( bottom_x, bottom_y);
        bg_va[13].position = sf::Vector2f( bottom_x, bottom_y + under_height );
        bg_va[14].position = sf::Vector2f( right_x, right_y);
        bg_va[15].position = sf::Vector2f( right_x, right_y);
        bg_va[16].position = sf::Vector2f( right_x, right_y + under_height );
        bg_va[17].position = sf::Vector2f( bottom_x, bottom_y + under_height);

        return 0;
    }

    int draw_grid_lines(){
        sf::VertexArray & grid_line_va = va_map.at("grid_lines");

        for (int i = 0; i < grid_line_count * 4; i++){
            grid_line_va[i].color = GRID_COLOR;
        }
        float under_height = UNDERGROUND_HEIGHT_REL * CENTRAL_TOTAL_HEIGHT;

        int i = 0;
        // Horizontal lines.
        float left_x, left_y;
        float right_x, right_y;
        float down_x, down_y;
        // Vertical lines;
        float top_x, top_y;
        float bottom_x, bottom_y;
        // reusing down_x and down_y.

        // Viewing the isometric pane as a plane, draw horizontal lines.
        while (i < COLONY_HEIGHT + 1){
            int grid_line_index_offset = i * 4;
            get_tile_loc(0, i, left_x, left_y);
            get_tile_loc(COLONY_WIDTH, i, right_x, right_y);
            down_x = right_x;
            down_y = right_y + under_height;
        
            grid_line_va[grid_line_index_offset + 0].position = sf::Vector2f( left_x, left_y );
            grid_line_va[grid_line_index_offset + 1].position = sf::Vector2f( right_x, right_y );
            grid_line_va[grid_line_index_offset + 2].position = sf::Vector2f( right_x, right_y );
            grid_line_va[grid_line_index_offset + 3].position = sf::Vector2f( down_x, down_y );

            i++;
        }
        while ( i < COLONY_HEIGHT + 1 + COLONY_WIDTH + 1){
            int grid_line_index_offset = i * 4;
            get_tile_loc(i - COLONY_HEIGHT - 1, 0, top_x, top_y);
            get_tile_loc(i - COLONY_HEIGHT - 1, COLONY_HEIGHT, bottom_x, bottom_y);
            down_x = bottom_x;
            down_y = bottom_y + under_height;

            grid_line_va[grid_line_index_offset + 0].position = sf::Vector2f( top_x, top_y );
            grid_line_va[grid_line_index_offset + 1].position = sf::Vector2f( bottom_x, bottom_y );
            grid_line_va[grid_line_index_offset + 2].position = sf::Vector2f( bottom_x, bottom_y );
            grid_line_va[grid_line_index_offset + 3].position = sf::Vector2f( down_x, down_y );
            i++;
        }
        return 0;
    }

    int draw_all_spores_from_colony(){

        for (auto & spore: colony->spore_man->spores_v){
            // cout << "drawing spore with sex " << spore.sex 
            //     << " loc x " << spore.loc_x 
            //     << " loc y " << spore.loc_y
            //     << " spore id " << spore.spore_id<< endl;
            int spore_index = spore.spore_id;
            sf::Color spore_color = color_map_arr[spore.sex];
            draw_spore(spore.loc_x, spore.loc_y, spore.spore_id, spore_color);
        }
        return 0;
    }



    int draw_spore(int x, int y, int spore_index, sf::Color color){
        sf::VertexArray & spore_va = va_map.at("spores");
        int spore_va_index = spore_index * 6 * 3;

        for (int i = 0; i < 6; i++)
            spore_va[spore_va_index + i].color = color;
        for (int i = 6; i < 12; i++)
            spore_va[spore_va_index + i].color = color + SPORE_LEFT_COLOR_MODIFIER;
        for (int i = 12; i < 18; i++)
            spore_va[spore_va_index + i].color = color - SPORE_RIGHT_COLOR_MODIFIER;
        
        float left_x, left_y;
        float right_x, right_y;
        float top_x, top_y;
        float bottom_x, bottom_y;
        float spore_height = SPORE_HEIGHT_REL * tile_height;
        get_tile_loc(x, y + 1, left_x, left_y);
        get_tile_loc(x + 1, y, right_x, right_y);
        get_tile_loc(x, y, top_x, top_y);
        get_tile_loc(x + 1, y + 1, bottom_x, bottom_y);

        spore_va[ spore_va_index + 0 ].position = sf::Vector2f( left_x, left_y - spore_height);
        spore_va[ spore_va_index + 1 ].position = sf::Vector2f( top_x, top_y - spore_height);
        spore_va[ spore_va_index + 2 ].position = sf::Vector2f( bottom_x, bottom_y - spore_height);
        spore_va[ spore_va_index + 3 ].position = sf::Vector2f( bottom_x, bottom_y - spore_height);
        spore_va[ spore_va_index + 4 ].position = sf::Vector2f( right_x, right_y - spore_height);
        spore_va[ spore_va_index + 5 ].position = sf::Vector2f( top_x, top_y - spore_height);

        spore_va[ spore_va_index + 6].position = sf::Vector2f( left_x, left_y - spore_height);
        spore_va[ spore_va_index + 7].position = sf::Vector2f( left_x, left_y);
        spore_va[ spore_va_index + 8].position = sf::Vector2f( bottom_x, bottom_y - spore_height);
        spore_va[ spore_va_index + 9].position = sf::Vector2f( bottom_x, bottom_y - spore_height);
        spore_va[ spore_va_index + 10].position = sf::Vector2f( bottom_x, bottom_y );
        spore_va[ spore_va_index + 11].position = sf::Vector2f( left_x, left_y);

        spore_va[ spore_va_index + 12].position = sf::Vector2f( bottom_x, bottom_y - spore_height);
        spore_va[ spore_va_index + 13].position = sf::Vector2f( bottom_x, bottom_y );
        spore_va[ spore_va_index + 14].position = sf::Vector2f( right_x, right_y - spore_height);
        spore_va[ spore_va_index + 15].position = sf::Vector2f( right_x, right_y - spore_height);
        spore_va[ spore_va_index + 16].position = sf::Vector2f( right_x, right_y );
        spore_va[ spore_va_index + 17].position = sf::Vector2f( bottom_x, bottom_y );

        return 0;
    }

    void update_fps_calculation(){
        float frame_time = clock.getElapsedTime().asSeconds();
        float fps = 1.0f / frame_time;
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << fps;
        std::string formatted_fps = stream.str();
        fps_counter_text.setString("FPS: " + formatted_fps);
        
    }
    

    int launch_window(){
        sf::RenderWindow window(
            sf::VideoMode(WIDTH, HEIGHT),
            "Chicken Window",
            // sf::Style::Default combines sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close.
            sf::Style::Titlebar | sf::Style::Close
        );
        window.setFramerateLimit(TARGET_FPS);

        if (!font.loadFromFile("Arial.ttf"))
            return EXIT_FAILURE;
        sf::Text text("Chicken text message\nBreakline", font, 20);
        text.setPosition(0, 0);
        fps_counter_text.setString("FPS");
        fps_counter_text.setFont(font);
        fps_counter_text.setCharacterSize(15);
        fps_counter_text.setPosition(WIDTH - 90, 0);

        
        sprite.setTexture(frame_texture);
        
            // Start the game loop
            while (window.isOpen())
            {
            sf::Time elapsed_time = clock.restart();
            if (colony->is_realtime){
                if (colony->residual_time > zero_second){
                    colony->residual_time -= elapsed_time;
                }
                else{
                    colony->residual_time += render_time;
                    colony->step_colony();
                }
            }

            while (window.pollEvent(event))
            {


                switch (event.type) {
                    // Close window: exit
                    case sf::Event::Closed:
                        window.close();
                        break;

                    case sf::Event::KeyPressed:
                        switch (event.key.code){
                            case sf::Keyboard::C:
                                // Step colony.
                                colony->step_colony();
                                break;
                            case sf::Keyboard::F:

                                
                                break;
                            case sf::Keyboard::R:
                                
                                break;
                            case sf::Keyboard::I:
                                
                                break;
                            case sf::Keyboard::N:
                                
                                break;
                            case sf::Keyboard::Space:
                                colony->is_realtime = (not colony->is_realtime);
                                break;
                            case sf::Keyboard::D:
                                window.close();
                                break;
                            case sf::Keyboard::Up:
                                
                                break;
                            case sf::Keyboard::Down:
                                
                                break;

                            case sf::Keyboard::Left:
                                
                                break;

                            case sf::Keyboard::Right:
                                
                                break;

                            default:
                                break;

                        }
                        break;

                    default:
                        break;
                }
            }
            // Clear screen
            window.clear(BG_COLOR);
    
            // Draw the sprite
            //frame_texture.update(pixels);
            sprite.setTexture(frame_texture);
            window.draw(sprite);

            // Draw spore locations;
            draw_all_spores_from_colony();
        

            window.draw(va_map.at("ground"));
            window.draw(va_map.at("grid_lines"));
            window.draw(va_map.at("spores"));

            // Draw ppu as shader.
            // draw_from_ppu_shader();
            // window.draw(ppu_image_va);
    
            update_fps_calculation();
            // Draw all strings.
            window.draw(text);
            window.draw(fps_counter_text);
    
            // Update the window
            window.display();
            frame_counter++;
        }
    return 0;
    };
};
