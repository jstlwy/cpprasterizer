#ifndef LINE_H
#define LINE_H

#include <vector>
#include <cstdint>

void draw_line_bresenham(
    std::vector<uint32_t>& pixels,
    const uint32_t color,
    int ax,
    int ay,
    int bx,
    int by
);

void inc_bresenham_gentle(int& x, int& y, int& step, int& p, int& e_same, int& e_diff);
void inc_bresenham_steep(int& x, int& y, int& step, int& p, int& e_same, int& e_diff);

#endif
