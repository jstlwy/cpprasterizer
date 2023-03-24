#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <vector>
#include "constants.h"
#include "utils.h"
#include "point.h"
#include "line.h"
#include "triangle.h"

struct Square {
	Point3D a;
	Point3D b;
	Point3D c;
	Point3D d;
};

void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, std::vector<std::uint32_t>& pixels);
bool wait_for_input();

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Error when initializing SDL: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow(
		"Software Rasterizer",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (window == nullptr)
	{
		std::cerr << "Error when creating window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr)
	{
		std::cerr << "Error when creating renderer: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Texture* texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STATIC,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	);
	if (texture == nullptr)
	{
		std::cerr << "Error when creating texture: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	// Init colors
	const std::uint32_t red   = 0xFFFF0000;
	const std::uint32_t green = 0xFF00FF00;
	const std::uint32_t blue  = 0xFF0000FF;
	const std::uint32_t black = 0xFF000000;
	const std::uint32_t blank = 0x00FFFFFF;

	std::vector<std::uint32_t> pixels(NUM_PIXELS, blank);

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
	// First, front face
	draw_line_bresenham(pixels, SCREEN_WIDTH, blue, pfa.x, pfa.y, pfb.x, pfb.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, blue, pfb.x, pfb.y, pfc.x, pfc.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, blue, pfc.x, pfc.y, pfd.x, pfd.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, blue, pfd.x, pfd.y, pfa.x, pfa.y);
	// Next, back face
	draw_line_bresenham(pixels, SCREEN_WIDTH, red, pba.x, pba.y, pbb.x, pbb.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, red, pbb.x, pbb.y, pbc.x, pbc.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, red, pbc.x, pbc.y, pbd.x, pbd.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, red, pbd.x, pbd.y, pba.x, pba.y);
	// Finally, lines connecting the faces
	draw_line_bresenham(pixels, SCREEN_WIDTH, green, pfa.x, pfa.y, pba.x, pba.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, green, pfb.x, pfb.y, pbb.x, pbb.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, green, pfc.x, pfc.y, pbc.x, pbc.y);
	draw_line_bresenham(pixels, SCREEN_WIDTH, green, pfd.x, pfd.y, pbd.x, pbd.y);
	auto end_time = std::chrono::system_clock::now();
	const auto cube_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::cout << "Cube: " << cube_us_elapsed.count() << " us" << std::endl;
	render_texture(renderer, texture, pixels);
	if (wait_for_input())
		return EXIT_SUCCESS;

	// Create green triangle
	Triangle greenTri = {
		{-200, -250, 0, 0.3},
		{ 200,   50, 0, 0.1},
		{  20,  250, 0, 1.0}
	};

	// TRIANGLE OUTLINE
	start_time = std::chrono::system_clock::now();
	draw_triangle_outline_3d(pixels, SCREEN_WIDTH, black, greenTri.a, greenTri.b, greenTri.c);
	end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto otri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Triangle outline: " << otri_us_elapsed.count() << " us" << std::endl;
    if (wait_for_input())
		return EXIT_SUCCESS;

	// FILLED TRIANGLE
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle(pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto ftri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Filled triangle: " << ftri_us_elapsed.count() << " us" << std::endl;
    if (wait_for_input())
		return EXIT_SUCCESS;

	// SHADED TRIANGLE
    start_time = std::chrono::system_clock::now();
    draw_shaded_triangle(pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto stri_ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Shaded triangle: " << stri_ms_elapsed.count() << " ms" << std::endl;
    if (wait_for_input())
		return EXIT_SUCCESS;

	// FILLED TRIANGLE (BRESENHAM)
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle_3d(pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto btri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Filled triangle (Bresenham): " << btri_us_elapsed.count() << " us" << std::endl;
    if (wait_for_input())
		return EXIT_SUCCESS;

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}


void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, std::vector<std::uint32_t>& pixels)
{
	SDL_RenderClear(renderer);
	SDL_UpdateTexture(texture, nullptr, &pixels[0], TEXTURE_PITCH);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
	std::fill(pixels.begin(), pixels.end(), 0x00FFFFFF);
}


bool wait_for_input()
{
	bool should_continue = false;
	bool should_quit = false;
	do
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
		{
			should_continue = true;
			should_quit = true;	
		}
		else if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					should_quit = true;
				case SDLK_RETURN:
				case SDLK_SPACE:
					should_continue = true;
					break;
				default:
					break;
			}
		}
	}
	while (!should_continue);
	return should_quit;
}
