#ifndef UTILS_H
#define UTILS_H

#include <vector>

void print_argb8888(const uint32_t color);

std::vector<float> interpolate(
    const float i0,
    const float d0,
    const float i1,
    const float d1
);

void upscale(
    std::vector<uint32_t>& original,
    const size_t orig_width,
    const size_t orig_height,
    const size_t upscale_factor
);

#endif
