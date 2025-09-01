#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

typedef union color {
    std::uint32_t raw;
    struct {
        std::uint32_t a : 8;
        std::uint32_t r : 8;
        std::uint32_t g : 8;
        std::uint32_t b : 8;
    };
} Color;

#endif
