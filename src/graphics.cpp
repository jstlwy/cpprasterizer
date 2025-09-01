#include "graphics.hpp"
#include "constants.hpp"

Graphics::Graphics()
    : pixels(NUM_PIXELS, COLOR_BLANK.raw)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
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
        throw std::runtime_error(SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    if (texture == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

Graphics::~Graphics()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::render_nondestructive()
{
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), TEXTURE_PITCH);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Graphics::render()
{
    render_nondestructive();
    std::fill(pixels.begin(), pixels.end(), COLOR_BLANK.raw);
}
