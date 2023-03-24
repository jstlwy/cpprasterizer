#ifndef LINE_H
#define LINE_H
#include <vector>

void draw_line_bresenham(std::vector<std::uint32_t>& pixels,
	const unsigned int width, const std::uint32_t color,
	const int ax, const int ay, const int bx, const int by);
void inc_bresenham_gentle(int& x, int& y, int& step, int& p, int& e_same, int& e_diff);
void inc_bresenham_steep(int& x, int& y, int& step, int& p, int& e_same, int& e_diff);

#endif