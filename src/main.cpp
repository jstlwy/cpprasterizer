#include <iostream>
#include <chrono>
#include "constants.hpp"
#include "utils.hpp"
#include "point.hpp"
#include "line.hpp"
#include "triangle.hpp"
#include "graphics.hpp"
#include <SDL2/SDL.h>

struct Square {
    Point3D a;
    Point3D b;
    Point3D c;
    Point3D d;
};

void render_shapes();
bool wait_for_input();

int main()
{
    try {
        render_shapes();
    } catch (std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void render_shapes()
{
    Graphics gfx;

    // Create a cube
    static constexpr Square cube_front_verts = {
        {-2, -0.5, 5, 0},
        {-2,  0.5, 5, 0},
        {-1,  0.5, 5, 0},
        {-1, -0.5, 5, 0}
    };
    static constexpr Square cube_back_verts = {
        {-2, -0.5, 6, 0},
        {-2,  0.5, 6, 0},
        {-1,  0.5, 6, 0},
        {-1, -0.5, 6, 0}
    };

    // CUBE
    auto start_time = std::chrono::system_clock::now();
    SDL_Point pfa = project_to_2d(cube_front_verts.a);
    SDL_Point pfb = project_to_2d(cube_front_verts.b);
    SDL_Point pfc = project_to_2d(cube_front_verts.c);
    SDL_Point pfd = project_to_2d(cube_front_verts.d);
    SDL_Point pba = project_to_2d(cube_back_verts.a);
    SDL_Point pbb = project_to_2d(cube_back_verts.b);
    SDL_Point pbc = project_to_2d(cube_back_verts.c);
    SDL_Point pbd = project_to_2d(cube_back_verts.d);
    // Front face
    draw_line_bresenham(gfx.pixels, COLOR_BLUE.raw, pfa.x, pfa.y, pfb.x, pfb.y);
    draw_line_bresenham(gfx.pixels, COLOR_BLUE.raw, pfb.x, pfb.y, pfc.x, pfc.y);
    draw_line_bresenham(gfx.pixels, COLOR_BLUE.raw, pfc.x, pfc.y, pfd.x, pfd.y);
    draw_line_bresenham(gfx.pixels, COLOR_BLUE.raw, pfd.x, pfd.y, pfa.x, pfa.y);
    // Back face
    draw_line_bresenham(gfx.pixels, COLOR_RED.raw, pba.x, pba.y, pbb.x, pbb.y);
    draw_line_bresenham(gfx.pixels, COLOR_RED.raw, pbb.x, pbb.y, pbc.x, pbc.y);
    draw_line_bresenham(gfx.pixels, COLOR_RED.raw, pbc.x, pbc.y, pbd.x, pbd.y);
    draw_line_bresenham(gfx.pixels, COLOR_RED.raw, pbd.x, pbd.y, pba.x, pba.y);
    // Lines connecting the two faces
    draw_line_bresenham(gfx.pixels, COLOR_GREEN.raw, pfa.x, pfa.y, pba.x, pba.y);
    draw_line_bresenham(gfx.pixels, COLOR_GREEN.raw, pfb.x, pfb.y, pbb.x, pbb.y);
    draw_line_bresenham(gfx.pixels, COLOR_GREEN.raw, pfc.x, pfc.y, pbc.x, pbc.y);
    draw_line_bresenham(gfx.pixels, COLOR_GREEN.raw, pfd.x, pfd.y, pbd.x, pbd.y);
    auto end_time = std::chrono::system_clock::now();
    const auto cube_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Cube: " << cube_us_elapsed.count() << " us" << std::endl;
    gfx.render();
    if (wait_for_input()) {
        return;
    }

    // TRIANGLE OUTLINE
    static constexpr Triangle3D greenTri = {
        {-200, -250, 0, 0.3},
        { 200,   50, 0, 0.1},
        {  20,  250, 0, 1.0}
    };
    start_time = std::chrono::system_clock::now();
    draw_triangle_outline_3d(gfx.pixels, COLOR_BLACK.raw, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    const auto otri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Triangle outline: " << otri_us_elapsed.count() << " us" << std::endl;
    gfx.render();
    if (wait_for_input()) {
        return;
    }

    // FILLED TRIANGLE
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle(gfx.pixels, SCREEN_WIDTH, COLOR_GREEN.raw, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();    
    const auto ftri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Filled triangle: " << ftri_us_elapsed.count() << " us" << std::endl;
    gfx.render();
    if (wait_for_input()) {
        return;
    }

    // SHADED TRIANGLE
    start_time = std::chrono::system_clock::now();
    draw_shaded_triangle(gfx.pixels, SCREEN_WIDTH, COLOR_GREEN.raw, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    const auto stri_ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Shaded triangle: " << stri_ms_elapsed.count() << " ms" << std::endl;
    gfx.render();
    if (wait_for_input()) {
        return;
    }

    // FILLED TRIANGLE (BRESENHAM)
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle_3d(gfx.pixels, SCREEN_WIDTH, COLOR_GREEN.raw, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    const auto btri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Filled triangle (Bresenham): " << btri_us_elapsed.count() << " us" << std::endl;
    gfx.render();
    if (wait_for_input()) {
        return;
    }

    // TINY TRIANGLE
    static constexpr Triangle2D tinyTri = {
        {10, 10},
        {10, 50},
        {20, 50}
    };
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle_bres(gfx.pixels, SCREEN_WIDTH, COLOR_RED.raw, tinyTri.a, tinyTri.b, tinyTri.c);
    end_time = std::chrono::system_clock::now();
    const auto tt_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Tiny triangle: " << tt_us_elapsed.count() << " us" << std::endl;
    gfx.render_nondestructive();
    if (wait_for_input()) {
        return;
    }

    // TINY TRIANGLE UPSCALED
    // Integer upscale to new buffer
    start_time = std::chrono::system_clock::now();
    static constexpr size_t orig_width = 256;
    static constexpr size_t orig_height = 224;
    static constexpr size_t upscale_factor = 4;
    upscale(gfx.pixels, orig_width, orig_height, upscale_factor);
    end_time = std::chrono::system_clock::now();
    const auto ttus_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Tiny triangle upscaled: " << ttus_us_elapsed.count() << " us" << std::endl;
    gfx.render();
    wait_for_input();
}

bool wait_for_input()
{
    bool should_keep_waiting_for_input = true;

    while (should_keep_waiting_for_input) {
        SDL_Event event;
        if (!SDL_WaitEvent(&event)) {
            std::cerr << __func__ << ": ERROR: " << SDL_GetError() << "\n";
            continue;
        }

        if (event.type == SDL_QUIT) {
            return true;
        }
        
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                return true;
            case SDLK_RETURN:
            case SDLK_SPACE:
                should_keep_waiting_for_input = false;
                break;
            default:
                break;
            }
        }
    }

    return false;
}
