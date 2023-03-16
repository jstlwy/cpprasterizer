#include <SDL.h>
#include <iostream>
#include <chrono>
#include <cmath>
//#include <thread>
#include <vector>

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
constexpr int X_MID_SCREEN = SCREEN_WIDTH / 2;
constexpr int Y_MID_SCREEN = SCREEN_HEIGHT / 2;
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

SDL_Point project2d(const Point3D& p)
{
    const float p_x = p.x * (d / p.z);
    const int c_x = std::round(p_x * (SCREEN_WIDTH / viewportSize));
    const float p_y = p.y * (d / p.z);
    const int c_y = std::round(p_y * (SCREEN_HEIGHT / viewportSize) * ASPECT_RATIO);
    return {X_MID_SCREEN + c_x, Y_MID_SCREEN - c_y};
}


Point3D projectVertex(const Point3D& p)
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


void draw_line_bresenham(SDL_Renderer* r, const int ax, const int ay, const int bx, const int by)
{
	const int dx = std::abs(bx - ax);
	const int dy = std::abs(by - ay);
	int x;
	int y;
	
	if (dx > dy)
	{
		int sy = 1;
		int x_end;
		if (ax > bx)
		{
			x = bx;
			x_end = ax;
			y = by;
			if (y > ay)
				sy = -1;
		}
		else
		{
			x = ax;
			x_end = bx;
			y = ay;
			if (y > by)
				sy = -1;
		}

		const int two_dy = 2 * dy;
		const int two_diff_dy_dx = 2 * (dy - dx);
		int p = two_dy - dx;
		
		while (x <= x_end)
		{
			SDL_RenderDrawPoint(r, x, y);
			x++;
			if (p < 0)
			{
				p += two_dy;
			}
			else
			{
				y += sy;
				p += two_diff_dy_dx;
			}
		}
	}
	else
	{
		int sx = 1;
		int y_end;
		if (ay > by)
		{
			x = bx;
			if (x > ax)
				sx = -1;
			y = by;
			y_end = ay;
		}
		else
		{
			x = ax;
			if (x > bx)
				sx = -1;
			y = ay;
			y_end = by;
		}

		const int two_dx = 2 * dx;
		const int two_diff_dx_dy = 2 * (dx - dy);
		int p = two_dx - dy;
		
		while (y <= y_end)
		{
			SDL_RenderDrawPoint(r, x, y);
			y++;
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


void drawLine(SDL_Renderer* r, Point3D p0, Point3D p1)
{   
    const float dx = p1.x - p0.x;
    const float dy = p1.y - p0.y;
    
    if (abs(dx) > abs(dy)) // Longer horizontally
    {
        if (p0.x > p1.x)
            std::swap(p0, p1);
        std::vector<float> yVals = interpolate(p0.x, p0.y, p1.x, p1.y);
        // Draw all the pixels in the line
        const int x0 = static_cast<int>(p0.x);
        for (int x = (int) p0.x; x <= p1.x; x++)
        {
            int xPixel = X_MID_SCREEN + x;
            int yPixel = Y_MID_SCREEN - std::round(yVals.at(x - x0));
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
    else // Longer vertically
    {
        if (p0.y > p1.y)
            std::swap(p0, p1);
        std::vector<float> xVals = interpolate(p0.y, p0.x, p1.y, p1.x);
        // Draw all the pixels in the line
        const int y0 = static_cast<int>(p0.y);
        for (int y = y0; y <= p1.y; y++)
        {
            int xPixel = X_MID_SCREEN + std::round(xVals.at(y - y0));
            int yPixel = Y_MID_SCREEN - y;
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
}


void draw_triangle(SDL_Renderer* r, const Point3D p0, const Point3D p1, const Point3D p2, const SDL_Color c)
{
    const int p0x = X_MID_SCREEN + static_cast<int>(p0.x);
    const int p0y = Y_MID_SCREEN - static_cast<int>(p0.y);
    const int p1x = X_MID_SCREEN + static_cast<int>(p1.x);
    const int p1y = Y_MID_SCREEN - static_cast<int>(p1.y);
    const int p2x = X_MID_SCREEN + static_cast<int>(p2.x);
    const int p2y = Y_MID_SCREEN - static_cast<int>(p2.y);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    draw_line_bresenham(r, p0x, p0y, p1x, p1y);
    draw_line_bresenham(r, p1x, p1y, p2x, p2y);
    draw_line_bresenham(r, p2x, p2y, p0x, p0y);
}


void drawFilledTriangle(SDL_Renderer* r, Point3D p0, Point3D p1, Point3D p2, const SDL_Color c)
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
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    for (float y = p0.y; y < p2.y; y++)
    {
        int n = static_cast<int>(y - p0.y);
        for (float x = xLeft.at(n); x < xRight.at(n); x++)
        {
            int xPixel = X_MID_SCREEN + std::round(x);
            int yPixel = Y_MID_SCREEN - std::round(y);
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
}


void drawShadedTriangle(SDL_Renderer* r, Point3D p0, Point3D p1, Point3D p2, SDL_Color c)
{
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
        int i = static_cast<int>(y - y0);
        float x_l = xLeft.at(i);
        float x_r = xRight.at(i);
        std::vector<float> hSeg = interpolate(x_l, hLeft.at(i), x_r, hRight.at(i));
        for (float x = x_l; x <= x_r; x++)
        {
            // Get pixel coordinates
            const int xPixel = X_MID_SCREEN + std::round(x);
            const int yPixel = Y_MID_SCREEN - std::round(y);
            // Get pixel hue
            const int j = static_cast<int>(x - x_l);
            const float hVal = hSeg.at(j);
            SDL_SetRenderDrawColor(
                r,
                std::round(c.r * hVal),
                std::round(c.g * hVal),
                std::round(c.b * hVal),
                c.a
            );
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
}


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
        std::cerr << "Window error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr)
    {
        std::cerr << "Renderer error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    // Init colors
    SDL_Color red   = {255,   0,   0, 255};
    SDL_Color green = {  0, 255,   0, 255};
    SDL_Color blue  = {  0,   0, 255, 255};
    SDL_Color black = {  0,   0,   0, 255};
    
    // Create green triangle
    Triangle greenTri = {
        {-200, -250, 0, 0.3},
        { 200,   50, 0, 0.1},
        {  20,  250, 0, 1.0}
    };
    
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
    
    //using frame_len_fps_60 = std::chrono::duration<float, std::ratio<1, 60>>;
    // Keep rendering until the user chooses to quit
    bool should_continue = true;
    while (should_continue)
    {
        const auto frame_start_time = std::chrono::system_clock::now();
        
        // Select the color with which to fill the screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // Clear the entire screen to the selected color
        SDL_RenderClear(renderer);

        // TRIANGLE
        // First, draw the filled triangle
        const auto shaded_tri_start_time = std::chrono::system_clock::now();
        drawShadedTriangle(renderer, greenTri.a, greenTri.b, greenTri.c, green);
        const auto shaded_tri_end_time = std::chrono::system_clock::now();
        const auto shaded_tri_ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(shaded_tri_end_time - shaded_tri_start_time);
        std::cout << "Shaded triangle: " << shaded_tri_ms_elapsed.count() << " ms" << std::endl;
        // Then draw the triangle's outline
        const auto tri_start_time = std::chrono::system_clock::now();
        draw_triangle(renderer, greenTri.a, greenTri.b, greenTri.c, black);
        const auto tri_end_time = std::chrono::system_clock::now();
        const auto tri_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(tri_end_time - tri_start_time);
        std::cout << "Triangle: " << tri_us_elapsed.count() << " us" << std::endl;
        
        // CUBE (Gambetta)
        /*
        Point3D pfa = projectVertex(cube_front_verts.a);
        Point3D pfb = projectVertex(cube_front_verts.b);
        Point3D pfc = projectVertex(cube_front_verts.c);
        Point3D pfd = projectVertex(cube_front_verts.d);
        Point3D pba = projectVertex(cube_back_verts.a);
        Point3D pbb = projectVertex(cube_back_verts.b);
        Point3D pbc = projectVertex(cube_back_verts.c);
        Point3D pbd = projectVertex(cube_back_verts.d);
        // First, front face
        SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        drawLine(renderer, pfa, pfb);
        drawLine(renderer, pfb, pfc);
        drawLine(renderer, pfc, pfd);
        drawLine(renderer, pfd, pfa);
        // Next, back face
        SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
        drawLine(renderer, pba, pbb);
        drawLine(renderer, pbb, pbc);
        drawLine(renderer, pbc, pbd);
        drawLine(renderer, pbd, pba);
        // Finally, lines connecting the faces
        SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);
        drawLine(renderer, pfa, pba);
        drawLine(renderer, pfb, pbb);
        drawLine(renderer, pfc, pbc);
        drawLine(renderer, pfd, pbd);
        */
        
        // CUBE
        const auto cube_start_time = std::chrono::system_clock::now();
        SDL_Point pfa = project2d(cube_front_verts.a);
        SDL_Point pfb = project2d(cube_front_verts.b);
        SDL_Point pfc = project2d(cube_front_verts.c);
        SDL_Point pfd = project2d(cube_front_verts.d);
        SDL_Point pba = project2d(cube_back_verts.a);
        SDL_Point pbb = project2d(cube_back_verts.b);
        SDL_Point pbc = project2d(cube_back_verts.c);
        SDL_Point pbd = project2d(cube_back_verts.d);
        // First, front face
        SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        draw_line_bresenham(renderer, pfa.x, pfa.y, pfb.x, pfb.y);
        draw_line_bresenham(renderer, pfb.x, pfb.y, pfc.x, pfc.y);
        draw_line_bresenham(renderer, pfc.x, pfc.y, pfd.x, pfd.y);
        draw_line_bresenham(renderer, pfd.x, pfd.y, pfa.x, pfa.y);
        // Next, back face
        SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
        draw_line_bresenham(renderer, pba.x, pba.y, pbb.x, pbb.y);
        draw_line_bresenham(renderer, pbb.x, pbb.y, pbc.x, pbc.y);
        draw_line_bresenham(renderer, pbc.x, pbc.y, pbd.x, pbd.y);
        draw_line_bresenham(renderer, pbd.x, pbd.y, pba.x, pba.y);
        // Finally, lines connecting the faces
        SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);
        draw_line_bresenham(renderer, pfa.x, pfa.y, pba.x, pba.y);
        draw_line_bresenham(renderer, pfb.x, pfb.y, pbb.x, pbb.y);
        draw_line_bresenham(renderer, pfc.x, pfc.y, pbc.x, pbc.y);
        draw_line_bresenham(renderer, pfd.x, pfd.y, pbd.x, pbd.y);
        const auto cube_end_time = std::chrono::system_clock::now();
        const auto cube_us_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(cube_end_time - cube_start_time);
		std::cout << "Cube: " << cube_us_elapsed.count() << " us" << std::endl;
		
        // Finally, show the results
        SDL_RenderPresent(renderer);
        
        const auto frame_end_time = std::chrono::system_clock::now();
		const auto frame_ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end_time - frame_start_time);
		std::cout << "Frame: " << frame_ms_elapsed.count() << " ms" << std::endl;
		/*
		if (frame_ms_elapsed < fps_60)
			std::this_thread::sleep_for(fps_limit - frame_ms_elapsed);
		*/
		
        // Wait for user input before rendering again
        SDL_Event event;
        SDL_PollEvent(&event);
        while (event.type != SDL_KEYDOWN && event.type != SDL_QUIT)
        {
            SDL_PollEvent(&event);
        }
        if (event.type == SDL_QUIT)
        {
            should_continue = false;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_ESCAPE:
                    should_continue = false;
                    break;
                case SDLK_UP:
                    break;
                case SDLK_DOWN:
                    break;
                default :
                    break;
            }
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
