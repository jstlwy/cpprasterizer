#include <iostream>
#include <cmath>
#include <cstdlib>
#include <SDL.h>
#include <vector>

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr double aspectRatio = (double) SCREEN_WIDTH / SCREEN_HEIGHT;
constexpr int halfSW = SCREEN_WIDTH / 2;
constexpr int halfSH = SCREEN_HEIGHT / 2;
constexpr double d = 1.0;
constexpr double viewportSize = 1.0;

struct Point3D {
    double x;
    double y;
    double z;
    double h;
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

struct Color {
    int R;
    int G;
    int B;
};

Point3D projectVertex(Point3D point)
{
    double p_x = point.x * (d / point.z);
    double c_x = p_x * (SCREEN_WIDTH / viewportSize);
    double p_y = point.y * (d / point.z);
    double c_y = p_y * (SCREEN_HEIGHT / viewportSize);
    
    Point3D newPoint = {
        c_x,
        c_y * aspectRatio,
        point.z,
        point.h
    };
    return newPoint;
}


// i = independent variable
// d = dependent variable
std::vector<double> interpolate(double i0, double d0, double i1, double d1)
{
    const int numValues = std::round(i1 - i0) + 1;
    std::vector<double> values;
    const double slope = (d1 - d0) / (i1 - i0);
    double d = d0;
    for (int i = 0; i < numValues; i++)
    {
        values.push_back(d);
        d += slope;
    }
    return values;
}


void drawLine(SDL_Renderer* r, Point3D p0, Point3D p1, Color c)
{
    const double dx = p1.x - p0.x;
    const double dy = p1.y - p0.y;
    
    if (abs(dx) > abs(dy)) // Longer horizontally
    {
        if (p0.x > p1.x)
            std::swap(p0, p1);
        std::vector<double> yVals = interpolate(p0.x, p0.y, p1.x, p1.y);
        // Draw all the pixels in the line
        SDL_SetRenderDrawColor(r, c.R, c.G, c.B, 255);
        const int x0 = static_cast<int>(p0.x);
        for (int x = (int) p0.x; x <= p1.x; x++)
        {
            int xPixel = halfSW + x;
            int yPixel = halfSH - std::round(yVals.at(x - x0));
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
    else // Longer vertically
    {
        if (p0.y > p1.y)
            std::swap(p0, p1);
        std::vector<double> xVals = interpolate(p0.y, p0.x, p1.y, p1.x);
        // Draw all the pixels in the line
        SDL_SetRenderDrawColor(r, c.R, c.G, c.B, 255);
        const int y0 = static_cast<int>(p0.y);
        for (int y = y0; y <= p1.y; y++)
        {
            int xPixel = halfSW + std::round(xVals.at(y - y0));
            int yPixel = halfSH - y;
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
}


void drawTriangle(SDL_Renderer* r, Point3D p0, Point3D p1, Point3D p2, Color c)
{
    drawLine(r, p0, p1, c);
    drawLine(r, p1, p2, c);
    drawLine(r, p2, p0, c);
}


void drawFilledTriangle(SDL_Renderer* r, Point3D p0, Point3D p1, Point3D p2, Color c)
{
    // Sort points so that y0 <= y1 <= y2
    if(p1.y < p0.y)
        std::swap(p1, p0);
    if(p2.y < p0.y)
        std::swap(p2, p0);
    if(p2.y < p1.y)
        std::swap(p2, p1);

    // Compute the x coordinates of the triangle edges
    std::vector<double> x01 = interpolate(p0.y, p0.x, p1.y, p1.x);
    std::vector<double> x12 = interpolate(p1.y, p1.x, p2.y, p2.x);
    std::vector<double> x02 = interpolate(p0.y, p0.x, p2.y, p2.x);
    
    // Concatenate the short sides.
    // Remember to remove the last element from x01!
    std::vector<double> x012;
    x012.insert(x012.end(), x01.begin(), x01.end() - 1);
    x012.insert(x012.end(), x12.begin(), x12.end());
    
    // Determine which is left and which is right
    const int m = floor(x012.size() / 2);
    std::vector<double> xLeft;
    std::vector<double> xRight;
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
    SDL_SetRenderDrawColor(r, c.R, c.G, c.B, 255);
    for (double y = p0.y; y < p2.y; y++)
    {
        int n = static_cast<int>(y - p0.y);
        for (double x = xLeft.at(n); x < xRight.at(n); x++)
        {
            int xPixel = halfSW + std::round(x);
            int yPixel = halfSH - std::round(y);
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
}


void drawShadedTriangle(SDL_Renderer* r, Point3D p0, Point3D p1, Point3D p2, Color c)
{
    // Sort the points so that y0 <= y1 <= y2
    if (p1.y < p0.y)
        std::swap(p1, p0);
    if (p2.y < p0.y)
        std::swap(p2, p0);
    if (p2.y < p1.y)
        std::swap(p2, p1);

    // Compute the x coordinates of the triangle edges
    std::vector<double> x01 = interpolate(p0.y, p0.x, p1.y, p1.x);
    std::vector<double> h01 = interpolate(p0.y, p0.h, p1.y, p1.h);
    std::vector<double> x12 = interpolate(p1.y, p1.x, p2.y, p2.x);
    std::vector<double> h12 = interpolate(p1.y, p1.h, p2.y, p2.h);
    std::vector<double> x02 = interpolate(p0.y, p0.x, p2.y, p2.x);
    std::vector<double> h02 = interpolate(p0.y, p0.h, p2.y, p2.h);
    
    // Concatenate the short sides.
    // Remember to remove the last elements from x01 and h01!
    std::vector<double> x012;
    x012.insert(x012.end(), x01.begin(), x01.end() - 1);
    x012.insert(x012.end(), x12.begin(), x12.end());
    std::vector<double> h012;
    h012.insert(h012.end(), h01.begin(), h01.end() - 1);
    h012.insert(h012.end(), h12.begin(), h12.end());
    
    // Determine which is left and which is right
    std::vector<double> xLeft;
    std::vector<double> hLeft;
    std::vector<double> xRight;
    std::vector<double> hRight;
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
    const double y0 = p0.y;
    for (double y = y0; y <= p2.y; y++)
    {
        int i = static_cast<int>(y - y0);
        double x_l = xLeft.at(i);
        double x_r = xRight.at(i);
        std::vector<double> hSeg = interpolate(x_l, hLeft.at(i), x_r, hRight.at(i));
        for (double x = x_l; x <= x_r; x++)
        {
            // Get pixel coordinates
            const int xPixel = halfSW + std::round(x);
            const int yPixel = halfSH - std::round(y);
            // Get pixel hue
            const int j = static_cast<int>(x - x_l);
            const double hVal = hSeg.at(j);
            const int R = std::round(c.R * hVal);
            const int G = std::round(c.G * hVal);
            const int B = std::round(c.B * hVal);
            SDL_SetRenderDrawColor(r, R, G, B, 255);
            SDL_RenderDrawPoint(r, xPixel, yPixel);
        }
    }
}


int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }
    
    SDL_Window* window = SDL_CreateWindow(
        "Rasterizer Project",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr)
    {
        std::cerr << "Window error: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr)
    {
        std::cerr << "Renderer error: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }

    // Init colors
    Color red   = {255,   0,   0};
    Color green = {  0, 255,   0};
    Color blue  = {  0,   0, 255};
    Color black = {  0,   0,   0};
    
    // Create green triangle
    Triangle greenTri = {
        {-200, -250, 0, 0.3},
        { 200,   50, 0, 0.1},
        {  20,  250, 0, 1.0}
    };
    
    // Create a cube
    Square frontVertices = {
        {-2, -0.5, 5, 0},
        {-2,  0.5, 5, 0},
        {-1,  0.5, 5, 0},
        {-1, -0.5, 5, 0}
    };
    Square backVertices = {
        {-2, -0.5, 6, 0},
        {-2,  0.5, 6, 0},
        {-1,  0.5, 6, 0},
        {-1, -0.5, 6, 0}
    };
    
    // Keep rendering until the user chooses to quit
    bool quit {false};
    while (!quit)
    {
        // Select the color with which to fill the screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // Clear the entire screen to the selected color
        SDL_RenderClear(renderer);

        // TRIANGLE
        // First, draw the filled triangle
        drawShadedTriangle(renderer, greenTri.a, greenTri.b, greenTri.c, green);
        // Then draw the triangle's outline
        drawTriangle(renderer, greenTri.a, greenTri.b, greenTri.c, black);
        
        // SQUARE
        // First, front face
        drawLine(renderer, projectVertex(frontVertices.a), projectVertex(frontVertices.b), blue);
        drawLine(renderer, projectVertex(frontVertices.b), projectVertex(frontVertices.c), blue);
        drawLine(renderer, projectVertex(frontVertices.c), projectVertex(frontVertices.d), blue);
        drawLine(renderer, projectVertex(frontVertices.d), projectVertex(frontVertices.a), blue);
        // Next, back face
        drawLine(renderer, projectVertex(backVertices.a), projectVertex(backVertices.b), red);
        drawLine(renderer, projectVertex(backVertices.b), projectVertex(backVertices.c), red);
        drawLine(renderer, projectVertex(backVertices.c), projectVertex(backVertices.d), red);
        drawLine(renderer, projectVertex(backVertices.d), projectVertex(backVertices.a), red);
        // Finally, lines connecting the faces
        drawLine(renderer, projectVertex(frontVertices.a), projectVertex(backVertices.a), green);
        drawLine(renderer, projectVertex(frontVertices.b), projectVertex(backVertices.b), green);
        drawLine(renderer, projectVertex(frontVertices.c), projectVertex(backVertices.c), green);
        drawLine(renderer, projectVertex(frontVertices.d), projectVertex(backVertices.d), green);

        // Finally, show the results
        SDL_RenderPresent(renderer);
        
        // Wait for user input before rendering again
        SDL_Event event;
        SDL_PollEvent(&event);
        while (event.type != SDL_KEYDOWN && event.type != SDL_QUIT)
        {
            SDL_PollEvent(&event);
        }
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_RETURN:
                case SDLK_SPACE:
                case SDLK_ESCAPE:
                    quit = true;
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
