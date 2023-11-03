#include "triangle.h"
#include "constants.h"
#include "line.h"
#include "utils.h"
#include <cassert>
#include <cmath>
#include <functional>

void draw_filled_triangle(
    std::vector<std::uint32_t>& pixels,
    const unsigned int width,
    const std::uint32_t color,
    Point3D p0, Point3D p1, Point3D p2)
{
    // Sort points so that y0 <= y1 <= y2
    if (p1.y < p0.y) {
        std::swap(p1, p0);
    }
    if (p2.y < p0.y) {
        std::swap(p2, p0);
    }
    if (p2.y < p1.y) {
        std::swap(p2, p1);
    }

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
    if (x02.at(m) < x012.at(m)) {
        xLeft = std::move(x02);
        xRight = std::move(x012);
    } else {
        xLeft = std::move(x012);
        xRight = std::move(x02);
    }

    // Draw the horizontal segments
    for (float y = p0.y; y < p2.y; y++) {
        const int yPixel = Y_MID_SCREEN - std::round(y);
        const int row = yPixel * width;
        int n = static_cast<int>(y - p0.y);
        for (float x = xLeft.at(n); x < xRight.at(n); x++) {
            int xPixel = X_MID_SCREEN + std::round(x);
            pixels.at(row + xPixel) = color;
        }
    }
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
    if (p1.y < p0.y) {
        std::swap(p1, p0);
    }
    if (p2.y < p0.y) {
        std::swap(p2, p0);
    }
    if (p2.y < p1.y) {
        std::swap(p2, p1);
    }

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
    if (x02.at(m) < x012.at(m)) {
        xLeft = std::move(x02);
        hLeft = std::move(h02);
        xRight = std::move(x012);
        hRight = std::move(h012);
    } else {
        xLeft = std::move(x012);
        hLeft = std::move(h012);
        xRight = std::move(x02);
        hRight = std::move(h02);
    }

    // Draw the horizontal segments
    const float y0 = p0.y;
    for (float y = y0; y <= p2.y; y++) {
        const int yPixel = Y_MID_SCREEN - std::round(y);
        const int row = yPixel * width;
        int i = static_cast<int>(y - y0);
        float x_l = xLeft.at(i);
        float x_r = xRight.at(i);
        std::vector<float> hSeg = interpolate(x_l, hLeft.at(i), x_r, hRight.at(i));
        for (float x = x_l; x <= x_r; x++) {
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


void draw_triangle_outline(std::vector<std::uint32_t>& pixels,
    const unsigned int width, const std::uint32_t color,
    const SDL_Point& v0, const SDL_Point& v1, const SDL_Point& v2)
{
    draw_line_bresenham(pixels, width, color, v0.x, v0.y, v1.x, v1.y);
    draw_line_bresenham(pixels, width, color, v1.x, v1.y, v2.x, v2.y);
    draw_line_bresenham(pixels, width, color, v2.x, v2.y, v0.x, v0.y);
}


void draw_triangle_outline_3d(std::vector<std::uint32_t>& pixels,
    const unsigned int width, const std::uint32_t color,
    const Point3D& p0, const Point3D& p1, const Point3D& p2)
{
    const SDL_Point v0 = project_special(p0);
    const SDL_Point v1 = project_special(p1);
    const SDL_Point v2 = project_special(p2);
    draw_triangle_outline(pixels, width, color, v0, v1, v2);
}


void draw_filled_triangle_flat_side(std::vector<std::uint32_t>& pixels,
    const unsigned int width,  const std::uint32_t color,
    SDL_Point v0, SDL_Point v1, SDL_Point v2)
{
    assert(v1.y == v2.y);
    if (v1.x > v2.x) {
        std::swap(v1, v2);
    }

    int x10;
    int y10;
    int x10_end;
    int x20;
    int y20;
    int x20_end;
    int y_end;
    if (v0.y < v1.y) {
        // Top is pointed, bottom is flat,
        // so start both lines at v0
        x10 = v0.x;
        y10 = v0.y;
        x10_end = v1.x;
        x20 = v0.x;
        y20 = v0.y;
        x20_end = v2.x;
        y_end = v1.y;
    } else {
        // Top is flat, bottom is pointed,
        // so start each line at v1 and v2
        x10 = v1.x;
        y10 = v1.y;
        x10_end = v0.x;
        x20 = v2.x;
        y20 = v2.y;
        x20_end = v0.x;
        y_end = v0.y;
    }

    int s10 = x10 < x10_end ? 1 : -1;
    int e10_same;
    int e10_diff;
    int p10;
    std::function<void(int&, int&, int&, int&, int&, int&)> inc_func_l10;
    const int dx10 = std::abs(v0.x - v1.x);
    const int dy10 = std::abs(v0.y - v1.y);
    if (dx10 > dy10) {
        // Gentle slope; x always incremented
        e10_same = 2 * dy10;
        e10_diff = 2 * (dy10 - dx10);
        p10 = e10_same - dx10;
        inc_func_l10 = inc_bresenham_gentle;
    } else {
        // Steep slope; y always incremented
        e10_same = 2 * dx10;
        e10_diff = 2 * (dx10 - dy10);
        p10 = e10_same - dy10;
        inc_func_l10 = inc_bresenham_steep;
    }

    int s20 = x20 < x20_end ? 1 : -1;
    int e20_same;
    int e20_diff;
    int p20;
    std::function<void(int&, int&, int&, int&, int&, int&)> inc_func_l20;
    const int dx20 = std::abs(v0.x - v2.x);
    const int dy20 = std::abs(v0.y - v2.y);
    if (dx20 > dy20) {
        // Gentle slope; x always incremented
        e20_same = 2 * dy20;
        e20_diff = 2 * (dy20 - dx20);
        p20 = e20_same - dx20;
        inc_func_l20 = inc_bresenham_gentle;
    } else {
        // Steep slope; y always incremented
        e20_same = 2 * dx20;
        e20_diff = 2 * (dx20 - dy20);
        p20 = e20_same - dy20;
        inc_func_l20 = inc_bresenham_steep;
    }

    const int row_end = y_end * width;
    for (int row = y10 * width; row <= row_end; row += width) {
        // Draw row
        for (int x = x10; x <= x20; x++) {
            pixels.at(row + x) = color;
        }

        // Increment both lines
        const int y_old = y10;
        while (y10 == y_old) {
            inc_func_l10(x10, y10, s10, p10, e10_same, e10_diff);
        }
        while (y20 == y_old) {
            inc_func_l20(x20, y20, s20, p20, e20_same, e20_diff);
        }
    }
}


void draw_filled_triangle_bres(std::vector<std::uint32_t>& pixels,
    const unsigned int width,  const std::uint32_t color,
    SDL_Point v0, SDL_Point v1, SDL_Point v2)
{
    // Sort points so that y0 <= y1 <= y2
    if (v1.y < v0.y) {
        std::swap(v1, v0);
    }
    if (v2.y < v0.y) {
        std::swap(v2, v0);
    }
    if (v2.y < v1.y) {
        std::swap(v2, v1);
    }

    if (v1.y == v2.y) {
        // Bottom is flat
        draw_filled_triangle_flat_side(pixels, width, color, v0, v1, v2);
    } else if (v0.y == v1.y) {	
        // Top is flat
        draw_filled_triangle_flat_side(pixels, width, color, v2, v0, v1);
    } else {
        // Split triangle in two.	
        // Note that y1 < y2, so the program needs to find
        // the x value where a horizontal line extending from p1
        // intersects the line between p0 and p2.
        const float dxdy02 = static_cast<float>(v2.x - v0.x) / (v2.y - v0.y);
        const int y_diff = v1.y - v0.y;
        const int vmid_x = std::round((dxdy02 * static_cast<float>(y_diff)) + static_cast<float>(v0.x));
        SDL_Point vmid = {vmid_x, v1.y};
        if (v1.x < vmid_x) {
            draw_filled_triangle_flat_side(pixels, width, color, v0, v1, vmid);
            draw_filled_triangle_flat_side(pixels, width, color, v2, v1, vmid);
        } else {
            draw_filled_triangle_flat_side(pixels, width, color, v0, vmid, v1);
            draw_filled_triangle_flat_side(pixels, width, color, v2, vmid, v1);
        }
    }
}


void draw_filled_triangle_3d(std::vector<std::uint32_t>& pixels,
    const unsigned int width, const std::uint32_t color,
    Point3D p0, Point3D p1, Point3D p2)
{
    SDL_Point v0 = project_special(p0);
    SDL_Point v1 = project_special(p1);
    SDL_Point v2 = project_special(p2);
    draw_filled_triangle_bres(pixels, width, color, v0, v1, v2);
}
