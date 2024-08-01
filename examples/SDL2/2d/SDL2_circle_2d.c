#include "../pf_sdl2_common.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "PixelFactory - SDL2 - Circle 2D",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    pf_sdl2_renderer2d_t rn = pf_sdl2_renderer2d_create(
        window, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!pf_sdl2_renderer2d_is_valid(&rn)) {
        fprintf(stderr, "ERROR: Unable to create PixelFactory's renderer2d\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;

    while (running)
    {
        SDL_Event e;
        
        while (SDL_PollEvent(&e))
        {
            running = !(e.type == SDL_QUIT);
        }

        pf_renderer_clear2d(&rn.pf_rn, PF_BLACK);
        pf_renderer_circle2d_gradient(&rn.pf_rn, 400, 300, 64, PF_BLUE, PF_RED);
        pf_renderer_circle2d_lines_thick(&rn.pf_rn, 400, 300, 128, 8, PF_RED);

        pf_sdl2_renderer2d_update(&rn);
    }

    pf_sdl2_renderer2d_delete(&rn);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
