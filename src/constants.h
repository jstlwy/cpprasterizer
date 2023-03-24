#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cstdint>

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int X_MID_SCREEN = SCREEN_WIDTH / 2;
constexpr int Y_MID_SCREEN = SCREEN_HEIGHT / 2;
constexpr int NUM_PIXELS = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr int TEXTURE_PITCH = SCREEN_WIDTH * sizeof(std::uint32_t);
constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
constexpr float d = 1.0;
constexpr float viewportSize = 1.0;

#endif