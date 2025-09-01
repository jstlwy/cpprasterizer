#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

typedef union color {
    uint32_t raw;
    struct {
        uint32_t a : 8;
        uint32_t r : 8;
        uint32_t g : 8;
        uint32_t b : 8;
    };
} Color;

#endif
