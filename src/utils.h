#ifndef UTILS_H
#define UTILS_H

#include <cinttypes>
#include <vector>

void print_argb8888(const std::uint32_t color);
std::vector<float> interpolate(const float i0, const float d0, const float i1, const float d1);

#endif
