#ifndef PF_SDL2_COMMON_H
#define PF_SDL2_COMMON_H

#include "pixelfactory/pf.h"
#include <SDL2/SDL.h>

typedef struct {
    pf_renderer_t pf_rn;
    SDL_Renderer* sdl_rn;
    SDL_Texture* sdl_tex;
} pf_sdl2_renderer2d_t;

pf_sdl2_renderer2d_t
pf_sdl2_renderer2d_create(SDL_Window* sdl_win, int w, int h)
{
    pf_sdl2_renderer2d_t rn = { 0 };

    rn.pf_rn = pf_renderer_load(w, h, PF_RENDERER_2D | PF_RENDERER_3D);

    rn.sdl_rn = SDL_CreateRenderer(sdl_win, -1,
        SDL_RENDERER_ACCELERATED);

    if (rn.sdl_rn == NULL) {
        pf_renderer_delete(&rn.pf_rn);
        rn.pf_rn = (pf_renderer_t) { 0 };
        return rn;
    }

    rn.sdl_tex = SDL_CreateTexture(rn.sdl_rn,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);

    if (rn.sdl_tex == NULL) {
        pf_renderer_delete(&rn.pf_rn);
        rn.pf_rn = (pf_renderer_t) { 0 };
        SDL_DestroyTexture(rn.sdl_tex);
        rn.sdl_tex = NULL;
        return rn;
    }

    return rn;
}

void
pf_sdl2_renderer2d_delete(pf_sdl2_renderer2d_t* rn)
{
    pf_renderer_delete(&rn->pf_rn);
    SDL_DestroyTexture(rn->sdl_tex);
    SDL_DestroyRenderer(rn->sdl_rn);
}

bool
pf_sdl2_renderer2d_is_valid(pf_sdl2_renderer2d_t* rn)
{
    return pf_framebuffer_is_valid(&rn->pf_rn.fb)
        && rn->sdl_rn != NULL && rn->sdl_tex != NULL;
}

void
pf_sdl2_renderer2d_update(pf_sdl2_renderer2d_t* rn)
{
    SDL_UpdateTexture(rn->sdl_tex, NULL, rn->pf_rn.fb.buffer,
        rn->pf_rn.fb.w * sizeof(pf_color_t));

    SDL_RenderCopy(rn->sdl_rn, rn->sdl_tex,
        NULL, NULL);

    SDL_RenderPresent(rn->sdl_rn);
}

#endif //PF_SDL2_COMMON_H