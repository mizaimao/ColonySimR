#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>
#include "config.h"
#include "config.cpp"  // For runtime Config `cfg`
#include "colony.cpp"
#include "stat_curve.h"

using namespace::std;


class Viewer {
private:
    // Helper variables.
    int frame_counter = 0;
    double target_frame_ms = 1000.0 / cfg.target_fps;
    Uint64 perf_freq = 0;       // Performance counter frequency
    Uint64 last_perf_tick = 0;  // Previous high-res tick (for FPS display)
    double fps_smooth = 0.0;    // Smoothed FPS over ~30 frames

    // Decoupled simulation timing
    double sim_accumulator_ms = 0.0;  // Accumulated time for sim steps

    // SDL components.
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    TTF_Font * font = nullptr;

    // Pointers.
    Colony * p_colony = nullptr;
    Color color_map_arr[2];

    // Population curve.
    StatCurve * p_pop_curve = nullptr;

    // Tile geometry (computed from runtime config).
    int tile_width = 0;
    int tile_height = 0;
    float base_tile_x = 0;
    float base_tile_y = 0;

    // Curve panel geometry.
    int curve_pos_x = 0, curve_pos_y = 0, curve_width = 0, curve_height = 0;


    // ---- Drawing helpers ----

    void draw_filled_triangle(SDL_Renderer *r, Color c, float x1, float y1, float x2, float y2, float x3, float y3) {
        SDL_Vertex verts[3] = {
            {{x1, y1}, {c.r, c.g, c.b, c.a}},
            {{x2, y2}, {c.r, c.g, c.b, c.a}},
            {{x3, y3}, {c.r, c.g, c.b, c.a}}
        };
        SDL_RenderGeometry(r, nullptr, verts, 3, nullptr, 0);
    }

    void draw_line(SDL_Renderer *r, Color c, float x1, float y1, float x2, float y2) {
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
        SDL_FPoint pts[2] = {{x1, y1}, {x2, y2}};
        SDL_RenderDrawLinesF(r, pts, 2);
    }

    void draw_rect_filled(SDL_Renderer *r, Color c, float x, float y, float w, float h) {
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
        SDL_FRect rect = {x, y, w, h};
        SDL_RenderFillRectF(r, &rect);
    }

    // ---- Isometric coordinate helpers ----

    int determine_base_tile_loc(){
        int tile_w_raw = cfg.get_central_total_width() / COLONY_WIDTH;
        int tile_h_raw = cfg.get_central_total_height() / COLONY_HEIGHT;
        if (min(tile_w_raw, tile_h_raw) == tile_w_raw){  // Bound by width.
            tile_width = tile_w_raw;
            tile_height = (int)(tile_width / 2);
        } else {
            tile_height = tile_h_raw;
            tile_width = (int)(tile_height * 2);
        }
        base_tile_x = cfg.left_pane_width + (COLONY_WIDTH / 2) * tile_width;
        base_tile_y = cfg.central_padding_y;
        return 0;
    }

    void get_tile_loc(int x, int y, float & mapped_x, float & mapped_y){
        mapped_x = (x - y) * tile_width / 2 + base_tile_x;
        mapped_y = (x + y) * tile_height / 2 + base_tile_y;
    }

    // Inverse of get_tile_loc: screen position back to a colony grid coordinate.
    void convert_to_isometric_coor(float x, float y, int & mapped_x, int & mapped_y){
        float _inter_a = (y - base_tile_y) / (float)tile_height;
        float _inter_b = (x - base_tile_x) / (float)tile_width;
        mapped_x = (int)round(_inter_a + _inter_b);
        mapped_y = (int)round(_inter_a - _inter_b);
    }

    // ---- Static ground ----

    void draw_static_ground(){
        float left_x, left_y, right_x, right_y, top_x, top_y, bottom_x, bottom_y;
        get_tile_loc(0, COLONY_HEIGHT, left_x, left_y);
        get_tile_loc(COLONY_WIDTH, 0, right_x, right_y);
        get_tile_loc(0, 0, top_x, top_y);
        get_tile_loc(COLONY_WIDTH, COLONY_HEIGHT, bottom_x, bottom_y);

        // Above ground diamond (2 triangles)
        draw_filled_triangle(renderer, cfg.color_ground, left_x, left_y, top_x, top_y, bottom_x, bottom_y);
        draw_filled_triangle(renderer, cfg.color_ground, bottom_x, bottom_y, right_x, right_y, top_x, top_y);

        // Underground sides
        float under_height = cfg.underground_height_ratio * cfg.get_central_total_height();
        Color ulc = cfg.color_underground + cfg.color_underground_left_mod;
        Color urc = cfg.color_underground - cfg.color_underground_right_mod;

        draw_filled_triangle(renderer, ulc, left_x, left_y, left_x, left_y + under_height, bottom_x, bottom_y);
        draw_filled_triangle(renderer, ulc, bottom_x, bottom_y, bottom_x, bottom_y + under_height, left_x, left_y + under_height);

        draw_filled_triangle(renderer, urc, bottom_x, bottom_y, bottom_x, bottom_y + under_height, right_x, right_y);
        draw_filled_triangle(renderer, urc, right_x, right_y, right_x, right_y + under_height, bottom_x, bottom_y + under_height);
    }

    // ---- Grid lines ----

    void draw_grid_lines(){
        if (!cfg.show_gridlines) return;
        float under_height = cfg.underground_height_ratio * cfg.get_central_total_height();

        // Horizontal grid lines
        for (int i = 0; i <= COLONY_HEIGHT; i++){
            float lx, ly, rx, ry;
            get_tile_loc(0, i, lx, ly);
            get_tile_loc(COLONY_WIDTH, i, rx, ry);
            draw_line(renderer, cfg.color_grid, lx, ly, rx, ry);
            draw_line(renderer, cfg.color_grid, rx, ry, rx, ry + under_height);
        }
        // Vertical grid lines
        for (int i = 1; i <= COLONY_WIDTH; i++){
            float tx, ty, bx, by;
            get_tile_loc(i, 0, tx, ty);
            get_tile_loc(i, COLONY_HEIGHT, bx, by);
            draw_line(renderer, cfg.color_grid, tx, ty, bx, by);
            draw_line(renderer, cfg.color_grid, bx, by, bx, by + under_height);
        }
    }

    // ---- Spore drawing ----

    void draw_spore(int x, int y, Color base_color){
        float left_x, left_y, right_x, right_y, top_x, top_y, bottom_x, bottom_y;
        float spore_height = cfg.spore_height_ratio * tile_height;
        get_tile_loc(x, y + 1, left_x, left_y);
        get_tile_loc(x + 1, y, right_x, right_y);
        get_tile_loc(x, y, top_x, top_y);
        get_tile_loc(x + 1, y + 1, bottom_x, bottom_y);

        Color lcolor = base_color + cfg.color_spore_left_mod;
        Color rcolor = base_color - cfg.color_spore_right_mod;

        // Top face (2 triangles)
        draw_filled_triangle(renderer, base_color, left_x, left_y - spore_height, top_x, top_y - spore_height, bottom_x, bottom_y - spore_height);
        draw_filled_triangle(renderer, base_color, bottom_x, bottom_y - spore_height, right_x, right_y - spore_height, top_x, top_y - spore_height);

        // Left side (2 triangles)
        draw_filled_triangle(renderer, lcolor, left_x, left_y - spore_height, left_x, left_y, bottom_x, bottom_y - spore_height);
        draw_filled_triangle(renderer, lcolor, bottom_x, bottom_y - spore_height, bottom_x, bottom_y, left_x, left_y);

        // Right side (2 triangles)
        draw_filled_triangle(renderer, rcolor, bottom_x, bottom_y - spore_height, bottom_x, bottom_y, right_x, right_y - spore_height);
        draw_filled_triangle(renderer, rcolor, right_x, right_y - spore_height, right_x, right_y, bottom_x, bottom_y);
    }

    void draw_spore_outline(int x, int y, Color color){
        if (x < 0 || y < 0 || x >= COLONY_WIDTH || y >= COLONY_HEIGHT) return;
        if (p_colony->spore_man->spore_by_loc[y][x].size() == 0) return;

        float left_x, left_y, right_x, right_y, top_x, top_y, bottom_x, bottom_y;
        float spore_height = cfg.spore_height_ratio * tile_height;
        get_tile_loc(x, y + 1, left_x, left_y);
        get_tile_loc(x + 1, y, right_x, right_y);
        get_tile_loc(x, y, top_x, top_y);
        get_tile_loc(x + 1, y + 1, bottom_x, bottom_y);

        // Top diamond edges
        draw_line(renderer, color, top_x, top_y - spore_height, left_x, left_y - spore_height);
        draw_line(renderer, color, left_x, left_y - spore_height, bottom_x, bottom_y - spore_height);
        draw_line(renderer, color, bottom_x, bottom_y - spore_height, right_x, right_y - spore_height);
        draw_line(renderer, color, right_x, right_y - spore_height, top_x, top_y - spore_height);

        // Vertical edges
        draw_line(renderer, color, left_x, left_y - spore_height, left_x, left_y);
        draw_line(renderer, color, bottom_x, bottom_y - spore_height, bottom_x, bottom_y);
        draw_line(renderer, color, right_x, right_y - spore_height, right_x, right_y);

        // Bottom edges
        draw_line(renderer, color, left_x, left_y, bottom_x, bottom_y);
        draw_line(renderer, color, bottom_x, bottom_y, right_x, right_y);
    }

    void draw_all_spores(){
        for (int y = 0; y < COLONY_HEIGHT; y++){
            for (int x = 0; x < COLONY_WIDTH; x++){
                vector<Spore *> & location_vector = p_colony->spore_man->spore_by_loc[y][x];
                for (Spore * p_spore : location_vector){
                    Spore & spore = *p_spore;
                    Color spore_color = color_map_arr[spore.sex];
                    draw_spore(spore.loc_x, spore.loc_y, spore_color);
                    if (cfg.show_spore_outline) {
                        draw_spore_outline(spore.loc_x, spore.loc_y, cfg.color_spore_outline);
                    }
                }
            }
        }
    }

    void draw_spore_outline_on_mouse(){
        int window_mx, window_my;
        SDL_GetMouseState(&window_mx, &window_my);
        // The renderer has a fixed logical size, so window pixels have to be
        // mapped back into logical space before the isometric inverse applies.
        float mx, my;
        SDL_RenderWindowToLogical(renderer, window_mx, window_my, &mx, &my);
        // A spore at grid (x,y) visually spans 4 diamond corners. The reverse
        // transform maps each corner to a different tile, so we check all 4
        // contributing tiles and highlight the first one that has spores.
        int cx, cy;
        convert_to_isometric_coor(mx, my, cx, cy);
        // Check the 4 tiles that share this diamond corner.
        int neighbors[4][2] = {{cx, cy}, {cx-1, cy}, {cx, cy-1}, {cx-1, cy-1}};
        for (int i = 0; i < 4; i++){
            if (neighbors[i][0] >= 0 && neighbors[i][0] < COLONY_WIDTH &&
                neighbors[i][1] >= 0 && neighbors[i][1] < COLONY_HEIGHT &&
                !p_colony->spore_man->spore_by_loc[neighbors[i][1]][neighbors[i][0]].empty()){
                draw_spore_outline(neighbors[i][0], neighbors[i][1], cfg.color_spore_selected);
                break;
            }
        }
    }

    // ---- Population curve ----

    void initialize_curve_panel(){
        curve_height = (int)(cfg.window_height * cfg.pop_curve_height_pct / 100);
        curve_width = (int)(cfg.window_width * (1 - cfg.pop_curve_padding_pct * 2 / 100));
        curve_pos_x = (int)(cfg.pop_curve_padding_pct / 100 * cfg.window_width);
        curve_pos_y = cfg.window_height - curve_height;

        p_pop_curve = new StatCurve(curve_pos_x, curve_pos_y, curve_width, curve_height, cfg.pop_curve_timepoints);
    }

    // Sample the population. Called once per simulation tick rather than once
    // per frame, so the curve's x-axis measures sim time and not frame rate,
    // and so it stops advancing while the simulation is paused.
    void record_population(){
        p_pop_curve->add_point(p_colony->spore_man->current_population);
    }

    void draw_population_curve(){
        // Background pane
        draw_rect_filled(renderer, Color(255, 255, 255, 100), curve_pos_x, curve_pos_y, curve_width, curve_height);

        // Draw the curve line
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        std::vector<SDL_FPoint> points;
        const size_t n = std::min(p_pop_curve->data_x.size(), p_pop_curve->data_y.size());
        for (size_t i = 0; i < n; i++){
            points.push_back({(float)p_pop_curve->data_x[i], (float)p_pop_curve->data_y[i]});
        }
        SDL_RenderDrawLinesF(renderer, points.data(), (int)points.size());

        // Draw population text at natural size
        std::stringstream ss;
        ss << "Population: " << p_colony->spore_man->current_population;
        SDL_Surface * surf = TTF_RenderText_Blended(font, ss.str().c_str(), {50, 50, 50});
        if (!surf) return;
        SDL_Texture * txt = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = {curve_pos_x + 10, curve_pos_y + 5, surf->w, surf->h};
        SDL_RenderCopy(renderer, txt, nullptr, &dst);
        SDL_DestroyTexture(txt);
        SDL_FreeSurface(surf);
    }

    // ---- FPS counter ----

    void draw_fps(){
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << fps_smooth << " FPS";
        SDL_Surface * surf = TTF_RenderText_Blended(font, ss.str().c_str(), {50, 50, 50});
        if (!surf) return;
        SDL_Texture * txt = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = {(int)(cfg.window_width - surf->w - 10), 5, surf->w, surf->h};
        SDL_RenderCopy(renderer, txt, nullptr, &dst);
        SDL_DestroyTexture(txt);
        SDL_FreeSurface(surf);
    }

    // ---- Cleanup helper for error paths ----

    void cleanup_sdl(){
        if (font) { TTF_CloseFont(font); font = nullptr; }
        if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
        if (window) { SDL_DestroyWindow(window); window = nullptr; }
        TTF_Quit();
        SDL_Quit();
    }


public:
    int width = cfg.window_width;
    int height = cfg.window_height;

    Viewer(Colony * pp_colony){
        p_colony = pp_colony;
        color_map_arr[0] = cfg.color_spore_male;
        color_map_arr[1] = cfg.color_spore_female;

        determine_base_tile_loc();
        initialize_curve_panel();
    };

    ~Viewer(){
        if (p_pop_curve) delete p_pop_curve;
    }

    // Entrypoint.
    int launch_window(){
        // Init SDL2 subsystems
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
        }
        if (TTF_Init() < 0) {
            std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
            cleanup_sdl();
            return EXIT_FAILURE;
        }

        // Create window and renderer
        window = SDL_CreateWindow(
            "Chicken Window",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            cfg.window_width, cfg.window_height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        if (!window) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            cleanup_sdl();
            return EXIT_FAILURE;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            cleanup_sdl();
            return EXIT_FAILURE;
        }

        // The pane layout is computed once from the configured window size, so
        // pin a logical size and let SDL letterbox it. Without this, resizing
        // the window leaves the curve pane and FPS counter at stale positions.
        SDL_RenderSetLogicalSize(renderer, cfg.window_width, cfg.window_height);

        // Load font
        font = TTF_OpenFont("Arial.ttf", 20);
        if (!font) {
            std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
            cleanup_sdl();
            return EXIT_FAILURE;
        }

        // Init performance counter
        perf_freq = SDL_GetPerformanceFrequency();
        last_perf_tick = SDL_GetPerformanceCounter();
        Uint64 last_sim_tick = SDL_GetPerformanceCounter();  // For sim timing

        // Message text surface (left pane)
        std::string msg_str = "Chicken simulator\n[C] Step | [Space] Auto | [A] Add spore | "
            + std::to_string(cfg.get_sim_ticks_per_sec()) + " sim ticks/sec";
        SDL_Surface * msg_surf = TTF_RenderText_Blended(font, msg_str.c_str(), {50, 50, 50});
        if (!msg_surf) {
            std::cerr << "TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
            cleanup_sdl();
            return EXIT_FAILURE;
        }
        SDL_Texture * msg_txt = SDL_CreateTextureFromSurface(renderer, msg_surf);
        if (!msg_txt) {
            std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(msg_surf);
            cleanup_sdl();
            return EXIT_FAILURE;
        }

        // Seed the curve with the starting population so it has a sensible peak
        // to scale against before the first tick.
        record_population();

        // Main loop
        bool running = true;
        while (running){
            Uint64 frame_start_perf = SDL_GetPerformanceCounter();

            // Decoupled simulation: accumulate real time and fire fixed-rate sim ticks
            if (p_colony->is_realtime){
                Uint64 now = SDL_GetPerformanceCounter();
                double elapsed_ms = (double)(now - last_sim_tick) / perf_freq * 1000.0;
                last_sim_tick = now;
                sim_accumulator_ms += elapsed_ms;
                // Fire as many sim ticks as have accumulated
                int catch_up_count = 0;
                while (sim_accumulator_ms >= cfg.sim_tick_ms){
                    p_colony->step_colony();
                    record_population();
                    sim_accumulator_ms -= cfg.sim_tick_ms;
                    if (++catch_up_count >= 10) break;  // Prevent spiral of death
                }
            }

            // Handle events
            SDL_Event event;

            while (SDL_PollEvent(&event)){
                switch (event.type){
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_MOUSEMOTION:
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym){
                            case SDLK_c:
                                p_colony->step_colony();
                                record_population();
                                break;
                            case SDLK_SPACE:
                                p_colony->is_realtime = !p_colony->is_realtime;
                                break;
                            case SDLK_a:
                                p_colony->spore_man->create_a_spore();
                                break;
                            case SDLK_d:
                                running = false;
                                break;
                        }
                        break;
                }
            }

            // Clear screen
            SDL_SetRenderDrawColor(renderer, cfg.color_bg.r, cfg.color_bg.g, cfg.color_bg.b, 255);
            SDL_RenderClear(renderer);

            // Draw scene layers (order matters for isometric depth)
            draw_static_ground();
            draw_grid_lines();
            draw_all_spores();

            // Mouse hover outline (drawn every frame for smooth tracking)
            draw_spore_outline_on_mouse();

            // UI elements
            draw_population_curve();
            {
                SDL_Rect msg_rect = {10, 10, msg_surf->w, msg_surf->h};
                SDL_RenderCopy(renderer, msg_txt, nullptr, &msg_rect);
            }

            // FPS (smoothed, drawn last)
            {
                Uint64 now = SDL_GetPerformanceCounter();
                double frame_ms = (double)(now - last_perf_tick) / perf_freq * 1000.0;
                double instant_fps = frame_ms > 0 ? 1000.0 / frame_ms : cfg.target_fps;
                if (fps_smooth == 0.0) {
                    fps_smooth = instant_fps;
                } else {
                    fps_smooth = fps_smooth * 0.9 + instant_fps * 0.1;
                }
                last_perf_tick = now;
            }
            draw_fps();

            // Present
            SDL_RenderPresent(renderer);
            frame_counter++;

            // Frame timing (delay to hit target FPS)
            {
                Uint64 frame_end_perf = SDL_GetPerformanceCounter();
                double elapsed_ms = (double)(frame_end_perf - frame_start_perf) / perf_freq * 1000.0;
                if (elapsed_ms < target_frame_ms){
                    SDL_Delay((Uint32)(target_frame_ms - elapsed_ms));
                }
            }
        }

        // Cleanup
        SDL_DestroyTexture(msg_txt);
        SDL_FreeSurface(msg_surf);
        cleanup_sdl();

        return 0;
    };
};
