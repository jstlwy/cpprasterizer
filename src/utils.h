#ifndef UTILS_H
#define UTILS_H
#include <ostream>
#include <vector>

template <typename T>
void print_hex(std::ostream& stream, const T value, const int width = 0);

void print_argb8888(std::uint32_t color);

std::vector<float> interpolate(float i0, float d0, float i1, float d1);

#endif