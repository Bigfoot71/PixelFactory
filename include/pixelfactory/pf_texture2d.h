#ifndef PF_TEXTURE2D_H
#define PF_TEXTURE2D_H

#include "pf_pixel.h"

struct pf_texture2d;

typedef void(*pf_texture2d_mapper_fn)(const struct pf_texture2d*, uint32_t*, uint32_t*, float, float);
typedef pf_color_t(*pf_texture2d_sampler_fn)(const struct pf_texture2d*, float u, float v);

typedef struct pf_texture2d {
    void* texels;
    pf_texture2d_sampler_fn sampler;
    pf_texture2d_mapper_fn mapper;
    pf_pixel_getter_fn getter;
    pf_pixel_setter_fn setter;
    uint32_t w;
    uint32_t h;
    float tx;
    float ty;
} pf_texture2d_t;


pf_texture2d_t
pf_texture2d_create(void* pixels, uint32_t w, uint32_t h, pf_pixelformat_t format);

void
pf_texture2d_destroy(pf_texture2d_t* tex);


/* Map Functions */

void
pf_texture2d_uv_map_wrap_pot(const pf_texture2d_t* tex, uint32_t* x, uint32_t* y, float u, float v);

void
pf_texture2d_uv_map_wrap(const pf_texture2d_t* tex, uint32_t* x, uint32_t* y, float u, float v);

void
pf_texture2d_uv_map_clamp(const pf_texture2d_t* tex, uint32_t* x, uint32_t* y, float u, float v);

/* Sampler Functions */

pf_color_t
pf_texture2d_sample_nearest(const pf_texture2d_t* tex, float u, float v);

pf_color_t
pf_texture2d_sample_bilinear(const pf_texture2d_t* tex, float u, float v);

#endif //PF_TEXTURE2D_H
