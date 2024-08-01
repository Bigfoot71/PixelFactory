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
        "PixelFactory - SDL2 - Lines 2D",
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

        for (int x = 0; x <= 800; x += 20)
        {
            if ((x/20)%2) pf_renderer_line2d_thick_gradient(&rn.pf_rn, x, -100, x, 700, 4, PF_YELLOW, PF_BLUE);
            else pf_renderer_line2d_thick_gradient(&rn.pf_rn, x, 700, x, -100, 4, PF_YELLOW, PF_BLUE);
        }

        for (int y = 0; y <= 600; y += 20)
        {
            if ((y/20)%2) pf_renderer_line2d_thick_gradient(&rn.pf_rn, -100, y, 900, y, 4, PF_CYAN, PF_RED);
            else pf_renderer_line2d_thick_gradient(&rn.pf_rn, 900, y, -100, y, 4, PF_CYAN, PF_RED);
        }

        pf_renderer_line2d_thick_gradient(&rn.pf_rn, -800, -600, 800, 600, 32, PF_RED, PF_BLUE);
        pf_renderer_line2d_thick_gradient(&rn.pf_rn, 800, 0, 0, 600, 32, PF_RED, PF_BLUE);

        pf_sdl2_renderer2d_update(&rn);
    }

    pf_sdl2_renderer2d_delete(&rn);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
