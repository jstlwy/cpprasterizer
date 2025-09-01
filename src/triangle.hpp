#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.hpp"
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>

struct Triangle2D {
    SDL_Point a;
    SDL_Point b;
    SDL_Point c;
};

struct Triangle3D {
    Point3D a;
    Point3D b;
    Point3D c;
};

void draw_filled_triangle(
    std::vector<std::uint32_t>& pixels,
    const unsigned int width,
    const std::uint32_t color,
    Point3D p0,
    Point3D p1,
    Point3D p2
);

void draw_shaded_triangle(
    std::vector<std::uint32_t>& pixels,
    const unsigned int width,
    const std::uint32_t color,
    Point3D p0,
    Point3D p1,
    Point3D p2
);

void draw_triangle_outline(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    const SDL_Point& v0,
    const SDL_Point& v1,
    const SDL_Point& v2
);

void draw_triangle_outline_3d(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    const Point3D& p0,
    const Point3D& p1,
    const Point3D& p2
);

void draw_filled_triangle_bres(
    std::vector<std::uint32_t>& pixels,
    const unsigned int width,
    const std::uint32_t color,
    SDL_Point v0,
    SDL_Point v1,
    SDL_Point v2
);

void draw_filled_triangle_flat_side(
    std::vector<std::uint32_t>& pixels,
    const unsigned int width,
    const std::uint32_t color,
    SDL_Point v0,
    SDL_Point p1,
    SDL_Point p2
);

void draw_filled_triangle_3d(
    std::vector<std::uint32_t>& pixels,
    const unsigned int width,
    const std::uint32_t color,
    Point3D p0,
    Point3D p1,
    Point3D p2
);

#endif
