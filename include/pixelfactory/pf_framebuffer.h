#ifndef PF_FRAMEBUFFER_H
#define PF_FRAMEBUFFER_H

#include "pf_color.h"

struct pf_framebuffer;

typedef void(*pf_framebuffer_map_fn)(
    struct pf_framebuffer* fb,
    pf_color_t *ptr,
    int x, int y);

typedef struct pf_framebuffer {
    pf_color_t* buffer;
    uint32_t w;
    uint32_t h;
} pf_framebuffer_t;

pf_framebuffer_t
pf_framebuffer_create(uint32_t w, uint32_t h, pf_color_t def);

void
pf_framebuffer_delete(pf_framebuffer_t* fb);

int
pf_framebuffer_is_valid(const pf_framebuffer_t* fb);

void
pf_framebuffer_copy(pf_framebuffer_t * restrict dst_fb, const uint32_t dst_rect[4], const pf_framebuffer_t * restrict src_fb, const uint32_t src_rect[4]);

void
pf_framebuffer_copy_pixels(void * restrict dst, const pf_framebuffer_t * restrict src_fb, const uint32_t src_rect[4]);

pf_color_t
pf_framebuffer_get(const pf_framebuffer_t* fb, uint32_t x, uint32_t y);

void
pf_framebuffer_put(pf_framebuffer_t* fb, uint32_t x, uint32_t y, pf_color_t color);

void
pf_framebuffer_fill(pf_framebuffer_t* fb, const uint32_t rect[4], pf_color_t color);

void
pf_framebuffer_map(pf_framebuffer_t* fb, const uint32_t rect[4], pf_framebuffer_map_fn func);

#endif //PF_FRAMEBUFFER_H
