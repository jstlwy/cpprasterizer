#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstdint>
#include <cstddef>

void print_argb8888(const std::uint32_t color);

std::vector<float> interpolate(
    const float i0,
    const float d0,
    const float i1,
    const float d1
);

void upscale(
    std::vector<std::uint32_t>& original,
    const std::size_t orig_width,
    const std::size_t orig_height,
    const std::size_t upscale_factor
);

#endif
