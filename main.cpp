#include <SDL2/SDL.h>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
//#include <thread>
#include <vector>

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int X_MID_SCREEN = SCREEN_WIDTH / 2;
constexpr int Y_MID_SCREEN = SCREEN_HEIGHT / 2;
constexpr int NUM_PIXELS = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr int TEXTURE_PITCH = SCREEN_WIDTH * sizeof(std::uint32_t);
constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
constexpr float d = 1.0;
constexpr float viewportSize = 1.0;

struct Point3D {
	float x;
	float y;
	float z;
	float h;
};

struct ScreenPoint3D {
	int x;
	int y;
	float z;
	float h;
};

struct Triangle {
	Point3D a;
	Point3D b;
	Point3D c;
};

struct Square {
	Point3D a;
	Point3D b;
	Point3D c;
	Point3D d;
};

template <typename T>
void print_hex(std::ostream& stream, const T value, const int width = 0);
void print_argb8888(std::uint32_t color);
void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, std::vector<std::uint32_t>& pixels);
bool scan_input_for_quit();
bool wait_for_input();

SDL_Point project2d(const Point3D& p);
Point3D project_vertex(const Point3D& p);
std::vector<float> interpolate(float i0, float d0, float i1, float d1);

void draw_line_bresenham(std::vector<std::uint32_t>& pixels,
	const unsigned int width, const std::uint32_t color,
	const int ax, const int ay, const int bx, const int by);
void draw_triangle(std::vector<std::uint32_t>& pixels,
	const unsigned int width, const std::uint32_t color,
	const Point3D p0, const Point3D p1, const Point3D p2);
void draw_filled_triangle(std::vector<std::uint32_t>& pixels,
	const unsigned int width, const std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2);
void draw_filled_triangle_bres(std::vector<std::uint32_t>& pixels,
	const unsigned int width, const std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2);
void draw_filled_triangle_flat_side(std::vector<std::uint32_t>& pixels,
	const unsigned int width,  const std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2);
void draw_shaded_triangle(std::vector<std::uint32_t>& pixels,
	const unsigned int width, std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2);

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
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
	SDL_Point pfa = project2d(cube_front_verts.a);
	SDL_Point pfb = project2d(cube_front_verts.b);
	SDL_Point pfc = project2d(cube_front_verts.c);
	SDL_Point pfd = project2d(cube_front_verts.d);
	SDL_Point pba = project2d(cube_back_verts.a);
	SDL_Point pbb = project2d(cube_back_verts.b);
	SDL_Point pbc = project2d(cube_back_verts.c);
	SDL_Point pbd = project2d(cube_back_verts.d);
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
	bool should_quit = wait_for_input();
	if (should_quit)
		return EXIT_SUCCESS;

	// Create green triangle
	Triangle greenTri = {
		{-200, -250, 0, 0.3},
		{ 200,   50, 0, 0.1},
		{  20,  250, 0, 1.0}
	};

	// SHADED TRIANGLE
    start_time = std::chrono::system_clock::now();
    draw_shaded_triangle(pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
    draw_triangle(pixels, SCREEN_WIDTH, black, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto stri_ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Shaded triangle: " << stri_ms_elapsed.count() << " ms" << std::endl;
    wait_for_input();

	// FILLED TRIANGLE
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle(pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
    draw_triangle(pixels, SCREEN_WIDTH, black, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto ftri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Filled triangle: " << ftri_us_elapsed.count() << " us" << std::endl;
    wait_for_input();

	// FILLED TRIANGLE (BRESENHAM)
	/*
    start_time = std::chrono::system_clock::now();
    draw_filled_triangle_bres(pixels, SCREEN_WIDTH, green, greenTri.a, greenTri.b, greenTri.c);
    draw_triangle(pixels, SCREEN_WIDTH, black, greenTri.a, greenTri.b, greenTri.c);
    end_time = std::chrono::system_clock::now();
    render_texture(renderer, texture, pixels);
    const auto btri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Filled triangle (Bresenham): " << btri_us_elapsed.count() << " us" << std::endl;
    wait_for_input();
	*/

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}


template <typename T>
void print_hex(std::ostream& stream, const T value, const int width)
{
	std::ios original_formatting = std::ios(nullptr);
	original_formatting.copyfmt(stream);
	if (width <= 0)
		stream << std::hex << value;
	else
		stream << std::setfill('0') << std::setw(width) << std::hex << value;
	stream.copyfmt(original_formatting);
}


void print_argb8888(std::uint32_t color)
{
	const unsigned int a = (color & 0xFF000000) >> 24;
	const unsigned int r = (color & 0x00FF0000) >> 16;
	const unsigned int g = (color & 0x0000FF00) >> 8;
	const unsigned int b = color & 0x000000FF;
	std::cout << " A R G B" << std::endl;
	print_hex(std::cout, a, 2);
	print_hex(std::cout, r, 2);
	print_hex(std::cout, g, 2);
	print_hex(std::cout, b, 2);
	std::cout << std::endl;
}


void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, std::vector<std::uint32_t>& pixels)
{
	SDL_RenderClear(renderer);
	SDL_UpdateTexture(texture, nullptr, &pixels[0], TEXTURE_PITCH);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
	std::fill(pixels.begin(), pixels.end(), 0x00FFFFFF);
}


bool scan_input_for_quit()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	return (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE));
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


SDL_Point project2d(const Point3D& p)
{
	const float p_x = p.x * (d / p.z);
	const int c_x = std::round(p_x * (SCREEN_WIDTH / viewportSize));
	const float p_y = p.y * (d / p.z);
	const int c_y = std::round(p_y * (SCREEN_HEIGHT / viewportSize) * ASPECT_RATIO);
	return {X_MID_SCREEN + c_x, Y_MID_SCREEN - c_y};
}


Point3D project_vertex(const Point3D& p)
{
	const float p_x = p.x * (d / p.z);
	const float c_x = p_x * (SCREEN_WIDTH / viewportSize);
	const float p_y = p.y * (d / p.z);
	const float c_y = p_y * (SCREEN_HEIGHT / viewportSize) * ASPECT_RATIO;
	return {c_x, c_y, p.z, p.h};
}


// i = independent variable
// d = dependent variable
std::vector<float> interpolate(float i0, float d0, float i1, float d1)
{
	const int num_vals = std::round(i1 - i0) + 1;
	std::vector<float> values;
	const float slope = (d1 - d0) / (i1 - i0);
	float d = d0;
	for (int i = 0; i < num_vals; i++)
	{
		values.push_back(d);
		d += slope;
	}
	return values;
}


void draw_line_bresenham(std::vector<std::uint32_t>& pixels, const unsigned int width,
	const std::uint32_t color, const int ax, const int ay, const int bx, const int by)
{
	const int dx = std::abs(bx - ax);
	const int dy = std::abs(by - ay);
	int x = ax;
	int y = ay;

	if (dx == 0)
	{
		int y_end;
		if (ay < by)
		{
			y_end = by;
		}
		else
		{
			y = by;
			y_end = ay;
		}
		int row = y * width;
		const int row_end = y_end * width;
		while (row <= row_end)
		{
			pixels.at(row + x) = color;
			row += width;
		}
	}
	else if (dy == 0)
	{
		int x_end;
		if (ax < bx)
		{
			x_end = bx;
		}
		else
		{
			x = bx;
			x_end = ax;
		}
		const int row = y * width;
		while (x <= x_end)
		{
			pixels.at(row + x) = color;
			x++;
		}
	}
	else if (dx > dy)
	{
		//int sy = 1;
		int srow = width;
		int x_end;
		if (ax < bx)
		{
			x_end = bx;
			if (y > by)
				srow = -width;
		}
		else
		{
			x = bx;
			x_end = ax;
			y = by;
			if (y > ay)
				srow = -width;
		}

		const int two_dy = 2 * dy;
		const int two_diff_dy_dx = 2 * (dy - dx);
		int p = two_dy - dx;
		int row = y * width;

		while (x <= x_end)
		{
			pixels.at(row + x) = color;
			x++;
			if (p < 0)
			{
				p += two_dy;
			}
			else
			{
				//y += sy;
				row += srow;
				p += two_diff_dy_dx;
			}
		}
	}
	else
	{
		int sx = 1;
		int y_end;
		if (ay < by)
		{
			if (x > bx)
				sx = -1;
			y_end = by;
		}
		else
		{
			x = bx;
			if (x > ax)
				sx = -1;
			y = by;
			y_end = ay;
		}

		const int two_dx = 2 * dx;
		const int two_diff_dx_dy = 2 * (dx - dy);
		int p = two_dx - dy;
		int row = y * width;
		const int row_end = y_end * width;

		//while (y <= y_end)
		while (row <= row_end)
		{
			pixels.at(row + x) = color;
			//y++;
			row += width;
			if (p < 0)
			{
				p += two_dx;
			}
			else
			{
				x += sx;
				p += two_diff_dx_dy;
			}
		}
	}
}


void draw_triangle(std::vector<std::uint32_t>& pixels, const unsigned int width, const std::uint32_t color,
	const Point3D p0, const Point3D p1, const Point3D p2)
{
	const int p0x = X_MID_SCREEN + std::round(p0.x);
	const int p0y = Y_MID_SCREEN - std::round(p0.y);
	const int p1x = X_MID_SCREEN + std::round(p1.x);
	const int p1y = Y_MID_SCREEN - std::round(p1.y);
	const int p2x = X_MID_SCREEN + std::round(p2.x);
	const int p2y = Y_MID_SCREEN - std::round(p2.y);
	draw_line_bresenham(pixels, width, color, p0x, p0y, p1x, p1y);
	draw_line_bresenham(pixels, width, color, p1x, p1y, p2x, p2y);
	draw_line_bresenham(pixels, width, color, p2x, p2y, p0x, p0y);
}


void draw_filled_triangle(std::vector<std::uint32_t>& pixels, const unsigned int width,  const std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2)
{
	// Sort points so that y0 <= y1 <= y2
	if(p1.y < p0.y)
		std::swap(p1, p0);
	if(p2.y < p0.y)
		std::swap(p2, p0);
	if(p2.y < p1.y)
		std::swap(p2, p1);

	// Compute the x coordinates of the triangle edges
	std::vector<float> x01 = interpolate(p0.y, p0.x, p1.y, p1.x);
	std::vector<float> x12 = interpolate(p1.y, p1.x, p2.y, p2.x);
	std::vector<float> x02 = interpolate(p0.y, p0.x, p2.y, p2.x);

	// Concatenate the short sides.
	// Remember to remove the last element from x01!
	std::vector<float> x012;
	x012.insert(x012.end(), x01.begin(), x01.end() - 1);
	x012.insert(x012.end(), x12.begin(), x12.end());

	// Determine which is left and which is right
	const int m = floor(x012.size() / 2);
	std::vector<float> xLeft;
	std::vector<float> xRight;
	if (x02.at(m) < x012.at(m))
	{
		xLeft = std::move(x02);
		xRight = std::move(x012);
	}
	else
	{
		xLeft = std::move(x012);
		xRight = std::move(x02);
	}

	// Draw the horizontal segments
	for (float y = p0.y; y < p2.y; y++)
	{
		const int yPixel = Y_MID_SCREEN - std::round(y);
		const int row = yPixel * width;
		int n = static_cast<int>(y - p0.y);
		for (float x = xLeft.at(n); x < xRight.at(n); x++)
		{
			int xPixel = X_MID_SCREEN + std::round(x);
			pixels.at(row + xPixel) = color;
		}
	}
}


void draw_filled_triangle_bres(std::vector<std::uint32_t>& pixels,
	const unsigned int width,  const std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2)
{
	// Sort points so that y0 <= y1 <= y2
	if(p1.y < p0.y)
		std::swap(p1, p0);
	if(p2.y < p0.y)
		std::swap(p2, p0);
	if(p2.y < p1.y)
		std::swap(p2, p1);

	if (p1.y == p2.y)
	{
		// Bottom is flat
		draw_filled_triangle_flat_side(pixels, width, color, p0, p1, p2);
	}
	else if (p0.y == p1.y)
	{	
		// Top is flat
		draw_filled_triangle_flat_side(pixels, width, color, p2, p0, p1);
	}
	else
	{
		// Split triangle in two.	
		// Note that y1 < y2, so the program needs to find
		// the x value where a horizontal line extending from p1
		// intersects the line between p0 and p2.
		const float dxdy02 = (p2.x - p0.x) / (p2.y - p0.y);
		const float y_diff = p1.y - p0.y;
		const float pmid_x = p0.x + (dxdy02 * y_diff);
		Point3D pmid = {pmid_x, p1.y, p1.z, p1.h};
		if (p1.x < pmid_x)
		{
			draw_filled_triangle_flat_side(pixels, width, color, p0, p1, pmid);
			draw_filled_triangle_flat_side(pixels, width, color, p2, p1, pmid);
		}
		else
		{
			draw_filled_triangle_flat_side(pixels, width, color, p0, pmid, p1);
			draw_filled_triangle_flat_side(pixels, width, color, p2, pmid, p1);
		}
	}
}


void draw_filled_triangle_flat_side(std::vector<std::uint32_t>& pixels,
	const unsigned int width,  const std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2)
{
	assert(p1.y == p2.y);

	const int dx10 = std::abs(p1.x - p0.x);
	const int dy10 = std::abs(p1.y - p0.y);

	const int dx20 = std::abs(p2.x - p0.x);
	const int dy20 = std::abs(p2.y - p0.y);
}


void draw_shaded_triangle(std::vector<std::uint32_t>& pixels,
	const unsigned int width, std::uint32_t color,
	Point3D p0, Point3D p1, Point3D p2)
{
	// ARGB8888
	const int a = (color & 0xFF000000) >> 24;
	const int r = (color & 0x00FF0000) >> 16;
	const int g = (color & 0x0000FF00) >> 8;
	const int b = color & 0x000000FF;

	// Sort the points so that y0 <= y1 <= y2
	if (p1.y < p0.y)
		std::swap(p1, p0);
	if (p2.y < p0.y)
		std::swap(p2, p0);
	if (p2.y < p1.y)
		std::swap(p2, p1);

	// Compute the x coordinates of the triangle edges
	std::vector<float> x01 = interpolate(p0.y, p0.x, p1.y, p1.x);
	std::vector<float> h01 = interpolate(p0.y, p0.h, p1.y, p1.h);
	std::vector<float> x12 = interpolate(p1.y, p1.x, p2.y, p2.x);
	std::vector<float> h12 = interpolate(p1.y, p1.h, p2.y, p2.h);
	std::vector<float> x02 = interpolate(p0.y, p0.x, p2.y, p2.x);
	std::vector<float> h02 = interpolate(p0.y, p0.h, p2.y, p2.h);

	// Concatenate the short sides.
	// Remember to remove the last elements from x01 and h01!
	std::vector<float> x012;
	x012.insert(x012.end(), x01.begin(), x01.end() - 1);
	x012.insert(x012.end(), x12.begin(), x12.end());
	std::vector<float> h012;
	h012.insert(h012.end(), h01.begin(), h01.end() - 1);
	h012.insert(h012.end(), h12.begin(), h12.end());

	// Determine which is left and which is right
	std::vector<float> xLeft;
	std::vector<float> hLeft;
	std::vector<float> xRight;
	std::vector<float> hRight;
	const unsigned long m = x012.size() / 2;
	if (x02.at(m) < x012.at(m))
	{
		xLeft = std::move(x02);
		hLeft = std::move(h02);
		xRight = std::move(x012);
		hRight = std::move(h012);
	}
	else
	{
		xLeft = std::move(x012);
		hLeft = std::move(h012);
		xRight = std::move(x02);
		hRight = std::move(h02);
	}

	// Draw the horizontal segments
	const float y0 = p0.y;
	for (float y = y0; y <= p2.y; y++)
	{
		const int yPixel = Y_MID_SCREEN - std::round(y);
		const int row = yPixel * width;
		int i = static_cast<int>(y - y0);
		float x_l = xLeft.at(i);
		float x_r = xRight.at(i);
		std::vector<float> hSeg = interpolate(x_l, hLeft.at(i), x_r, hRight.at(i));
		for (float x = x_l; x <= x_r; x++)
		{
			// Get pixel hue
			const int j = static_cast<int>(x - x_l);
			const float h = hSeg.at(j);
			const int pix_r = std::round(h * r);
			const int pix_g = std::round(h * g);
			const int pix_b = std::round(h * b);
			std::uint32_t pix_color = 0 | (a << 24) | (pix_r << 16) | (pix_g << 8) | pix_b;

			const int xPixel = X_MID_SCREEN + std::round(x);
			pixels.at(row + xPixel) = pix_color;
		}
	}
}
