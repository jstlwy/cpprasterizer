#include "utils.hpp"
#include "constants.hpp"
#include <cmath>
#include <cstdio>
#include <cassert>
#include <cinttypes>

void print_argb8888(const std::uint32_t color)
{
    std::printf(" A R G B\n%" PRIX32 "\n", color);
}

// i = independent variable
// d = dependent variable
std::vector<float> interpolate(const float i0, const float d0, const float i1, const float d1)
{
    const std::size_t num_vals = static_cast<std::size_t>(std::round(i1 - i0)) + 1;
    std::vector<float> values(num_vals);
    const float slope = (d1 - d0) / (i1 - i0);
    float d = d0;
    for (float& f : values) {
        f = d;
        d += slope;
    }
    return values;
}

void upscale(
    std::vector<std::uint32_t>& original,
    const std::size_t target_width,
    const std::size_t target_height,
    const std::size_t upscale_factor
) {
    assert(original.size() == NUM_PIXELS);
    assert((target_width * target_height) <= original.size());

    const std::size_t upscale_width = target_width * upscale_factor;
    const std::size_t upscale_height = target_height * upscale_factor;
    assert(upscale_width <= SCREEN_WIDTH);
    assert(upscale_height <= SCREEN_HEIGHT);
    assert((upscale_width * upscale_height) <= original.size());

    std::vector<std::uint32_t> upscale(original.size(), COLOR_BLANK.raw);

    for (std::size_t y_up = 0; y_up < upscale_height; y_up++) {
        const std::size_t row_up = y_up * SCREEN_WIDTH;
        const std::size_t row_orig = (y_up / upscale_factor) * SCREEN_WIDTH;
        for (std::size_t x_up = 0; x_up < upscale_width; x_up++) {
            upscale.at(row_up + x_up) = original.at(row_orig + (x_up / upscale_factor));
        }
    }

    original.assign(upscale.begin(), upscale.end());
}
