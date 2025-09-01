#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include "color.hpp"

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int X_MID_SCREEN = SCREEN_WIDTH / 2;
constexpr int Y_MID_SCREEN = SCREEN_HEIGHT / 2;
constexpr int NUM_PIXELS = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr int TEXTURE_PITCH = SCREEN_WIDTH * sizeof(uint32_t);

constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
constexpr float D = 1.0; // TODO: Rename this
constexpr float VIEWPORT_SIZE = 1.0;

constexpr Color COLOR_RED   = {.raw = 0xFFFF0000};
constexpr Color COLOR_GREEN = {.raw = 0xFF00FF00};
constexpr Color COLOR_BLUE  = {.raw = 0xFF0000FF};
constexpr Color COLOR_BLACK = {.raw = 0xFF000000};
constexpr Color COLOR_BLANK = {.raw = 0x00FFFFFF};

#endif
