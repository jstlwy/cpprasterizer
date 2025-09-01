#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>

class Graphics {
public:
    std::vector<uint32_t> pixels;

    Graphics();
    ~Graphics();
    void render();
    void render_nondestructive();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#endif
