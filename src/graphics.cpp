#include "graphics.h"
#include "constants.h"
#include <iostream>
#include <exception>
#include "line.h"

Graphics::Graphics()
    : pixels(NUM_PIXELS, blank)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL_Init failed");
    }

    window = SDL_CreateWindow(
        "Software Rasterizer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr) {
        throw std::runtime_error("SDL_CreateWindow failed");
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        throw std::runtime_error("SDL_CreateRenderer failed");
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    if (texture == nullptr) {
        throw std::runtime_error("SDL_CreateTexture failed");
    }
}

Graphics::~Graphics()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::render()
{
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, nullptr, &pixels[0], TEXTURE_PITCH);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    std::fill(pixels.begin(), pixels.end(), 0x00FFFFFF);
}
