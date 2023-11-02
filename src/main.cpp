#include <iostream>
#include <chrono>
#include <exception>
#include "constants.h"
#include "utils.h"
#include "point.h"
#include "line.h"
#include "triangle.h"
#include "graphics.h"
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
	} catch (std::runtime_error &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

void render_shapes()
{
	Graphics gfx;

	// Create a cube
	Square cube_front_verts = {
		{-2, -0.5, 5, 0},
		{-2,  0.5, 5, 0},
		{-1,  0.5, 5, 0},
		{-1, -0.5, 5, 0}
	};
	Square cube_back_verts = {
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
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, blue, pfa.x, pfa.y, pfb.x, pfb.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, blue, pfb.x, pfb.y, pfc.x, pfc.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, blue, pfc.x, pfc.y, pfd.x, pfd.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, blue, pfd.x, pfd.y, pfa.x, pfa.y);
	// Back face
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, red, pba.x, pba.y, pbb.x, pbb.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, red, pbb.x, pbb.y, pbc.x, pbc.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, red, pbc.x, pbc.y, pbd.x, pbd.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, red, pbd.x, pbd.y, pba.x, pba.y);
	// Lines connecting the two faces
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, green, pfa.x, pfa.y, pba.x, pba.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, green, pfb.x, pfb.y, pbb.x, pbb.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, green, pfc.x, pfc.y, pbc.x, pbc.y);
	draw_line_bresenham(gfx.pixels, SCREEN_WIDTH, green, pfd.x, pfd.y, pbd.x, pbd.y);
	auto end_time = std::chrono::system_clock::now();
	const auto cube_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::cout << "Cube: " << cube_us_elapsed.count() << " us" << std::endl;
	gfx.render();
	if (wait_for_input()) {
		return;
	}

	// TRIANGLE OUTLINE
	Triangle greenTri = {
		{-200, -250, 0, 0.3},
		{ 200,   50, 0, 0.1},
		{  20,  250, 0, 1.0}
	};
	start_time = std::chrono::system_clock::now();
	draw_triangle_outline_3d(gfx.pixels, SCREEN_WIDTH, black, greenTri.a, greenTri.b, greenTri.c);
	end_time = std::chrono::system_clock::now();
	gfx.render();
	const auto otri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::cout << "Triangle outline: " << otri_us_elapsed.count() << " us" << std::endl;
	if (wait_for_input()) {
		return;
	}

	// FILLED TRIANGLE
	start_time = std::chrono::system_clock::now();
	draw_filled_triangle(gfx.pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
	end_time = std::chrono::system_clock::now();
	gfx.render();
	const auto ftri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::cout << "Filled triangle: " << ftri_us_elapsed.count() << " us" << std::endl;
	if (wait_for_input()) {
		return;
	}

	// SHADED TRIANGLE
	start_time = std::chrono::system_clock::now();
	draw_shaded_triangle(gfx.pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
	end_time = std::chrono::system_clock::now();
	gfx.render();
	const auto stri_ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Shaded triangle: " << stri_ms_elapsed.count() << " ms" << std::endl;
	if (wait_for_input()) {
		return;
	}

	// FILLED TRIANGLE (BRESENHAM)
	start_time = std::chrono::system_clock::now();
	draw_filled_triangle_3d(gfx.pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
	end_time = std::chrono::system_clock::now();
	gfx.render();
	const auto btri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::cout << "Filled triangle (Bresenham): " << btri_us_elapsed.count() << " us" << std::endl;
	wait_for_input();
}

bool wait_for_input()
{
	bool should_continue = false;
	bool should_quit = false;

	do {
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			should_continue = true;
			should_quit = true;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				should_quit = true;
				// Intentional fallthrough
			case SDLK_RETURN:
			case SDLK_SPACE:
				should_continue = true;
				break;
			default:
				break;
			}
		}
	} while (!should_continue);

	return should_quit;
}
