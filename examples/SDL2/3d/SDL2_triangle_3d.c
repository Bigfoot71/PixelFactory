#include "../pf_sdl2_common.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static float triangle_positions[] = {
     0,  1, 0,
    -1, -1, 0,
     1, -1, 0
};

static pf_color_t triangle_colors[] = {
    { .c = { 255, 0, 0, 255 } },
    { .c = { 0, 255, 0, 255 } },
    { .c = { 0, 0, 255, 255 } }
};

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "PixelFactory - SDL2 - Triangle 3D",
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

    pf_vertexbuffer_t triangle = pf_vertexbuffer_create_3d(
        3, triangle_positions, NULL, NULL, triangle_colors);

    bool running = true;

    while (running)
    {
        SDL_Event e;
        
        while (SDL_PollEvent(&e))
        {
            running = !(e.type == SDL_QUIT);
        }

        pf_mat4_look_at(rn.pf_rn.conf3d->mat_view,
            (float[3]) { 3.0f*cosf(SDL_GetTicks()/1000.0f), 0, 3.0f*sinf(SDL_GetTicks()/1000.0f) },
            (float[3]) { 0, 0, 0 }, (float[3]) { 0, 1, 0 });

        pf_renderer_clear3d(&rn.pf_rn, PF_BLACK, FLT_MAX);
        pf_renderer_vertexbuffer3d(&rn.pf_rn, &triangle, NULL, NULL);

        pf_sdl2_renderer2d_update(&rn);
    }

    pf_sdl2_renderer2d_delete(&rn);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
