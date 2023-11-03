#include "line.h"
#include "constants.h"
#include <algorithm>

void draw_horizontal_run(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    int& row,
    int& x,
    const int srow,
    const int runLen
);

void draw_vertical_run(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    int& row,
    int& x,
    const int sx,
    const int runLen
);

void draw_line_bresenham(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    int ax, int ay,
    int bx, int by)
{
    if (ax < 0 || bx < 0 || ay < 0 || by < 0 || ax >= SCREEN_WIDTH || bx >= SCREEN_WIDTH || ay >= SCREEN_HEIGHT || by >= SCREEN_HEIGHT) {
        return;
    }

    const int dx = bx > ax ? bx - ax : ax - bx;
    const int dy = by > ay ? by - ay : ay - by;

    if (dx == 0) {
        // Vertical lines
        if (ay > by) {
            std::swap(ay, by);
        }
        const int row_end = by * SCREEN_WIDTH;
        for (int row = ay * SCREEN_WIDTH; row <= row_end; row += SCREEN_WIDTH) {
            pixels[row + ax] = color;
        }
    } else if (dy == 0) {
        // Horizontal lines
        if (ax > bx) {
            std::swap(ax, bx);
        }
        const int row = ay * SCREEN_WIDTH;
        std::fill(pixels.begin() + (row + ax), pixels.begin() + (row + bx + 1), color);
    } else if (dx == dy) {
        // Slope = 1: Perfectly diagonal lines
        if (ax > bx) {
            std::swap(ax, bx);
            std::swap(ay, by);
        }
        const int srow = ay < by ? SCREEN_WIDTH : -SCREEN_WIDTH;
        for (int x = ax, row = ay * SCREEN_WIDTH; x <= bx; x++, row += srow) {
            pixels[row + x] = color;
        }
    } else if (dx > dy) {
        // Slope < 1: Gradual lines
        if (ax > bx) {
            std::swap(ax, bx);
            std::swap(ay, by);
        }
        const int srow = ay < by ? SCREEN_WIDTH : -SCREEN_WIDTH;

        const int two_dy = 2 * dy;
        const int two_diff_dy_dx = 2 * (dy - dx);
        int p = two_dy - dx;

        for (int x = ax, row = ay * SCREEN_WIDTH; x <= bx; x++) {
            pixels[row + x] = color;
            if (p < 0) {
                p += two_dy;
            } else {
                row += srow;
                p += two_diff_dy_dx;
            }
        }
    } else {
        // Slope > 1: Steep lines
        if (ay > by) {
            std::swap(ax, bx);
            std::swap(ay, by);
        }
        const int sx = ax < bx ? 1 : -1;
        const int row_end = by * SCREEN_WIDTH;

        const int two_dx = 2 * dx;
        const int two_diff_dx_dy = 2 * (dx - dy);
        int p = two_dx - dy;

        for (int x = ax, row = ay * SCREEN_WIDTH; row <= row_end; row += SCREEN_WIDTH) {
            pixels[row + x] = color;
            if (p < 0) {
                p += two_dx;
            } else {
                x += sx;
                p += two_diff_dx_dy;
            }
        }
    }
}

inline void draw_horizontal_run(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    int& row,
    int& x,
    const int srow,
    const int runLen)
{
    for (int i = 0; i < runLen; i++) {
        pixels[row + x] = color;
        x++;
    }
    row += srow;
}

inline void draw_vertical_run(
    std::vector<std::uint32_t>& pixels,
    const std::uint32_t color,
    int& row,
    int& x,
    const int sx,
    const int runLen)
{
    for (int i = 0; i < runLen; i++) {
        pixels[row + x] = color;
        row += SCREEN_WIDTH;
    }
    x += sx;
}

void inc_bresenham_gentle(int& x, int& y, int& step, int& p, int& e_same, int& e_diff)
{
    x += step;
    if (p < 0) {
        p += e_same;
    } else {
        y++;
        p += e_diff;
    }
}

void inc_bresenham_steep(int& x, int& y, int& step, int& p, int& e_same, int& e_diff)
{
    y++;
    if (p < 0) {
        p += e_same;
    } else {
        x += step;
        p += e_diff;
    }
}
