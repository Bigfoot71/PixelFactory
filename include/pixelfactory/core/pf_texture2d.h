/**
 *  Copyright (c) 2024 Le Juez Victor
 *
 *  This software is provided "as-is", without any express or implied warranty. In no event 
 *  will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial 
 *  applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you 
 *  wrote the original software. If you use this software in a product, an acknowledgment 
 *  in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *  as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PF_TEXTURE2D_H
#define PF_TEXTURE2D_H

#include "../components/pf_pixel.h"

struct pf_texture2d;

typedef void(*pf_texture2d_mapper_fn)(
    const struct pf_texture2d*,
    uint32_t*, uint32_t*,
    float, float);

typedef pf_color_t(*pf_texture2d_sampler_fn)(
    const struct pf_texture2d*,
    float u, float v);

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


PFAPI pf_texture2d_t
pf_texture2d_create(
    void* pixels, uint32_t w, uint32_t h,
    pf_pixelformat_e format);

PFAPI void
pf_texture2d_delete(
    pf_texture2d_t* tex);


/* Map Functions */

PFAPI void
pf_texture2d_uv_map_wrap_pot(
    const pf_texture2d_t* tex,
    uint32_t* x, uint32_t* y,
    float u, float v);

PFAPI void
pf_texture2d_uv_map_wrap(
    const pf_texture2d_t* tex,
    uint32_t* x, uint32_t* y,
    float u, float v);

PFAPI void
pf_texture2d_uv_map_clamp(
    const pf_texture2d_t* tex,
    uint32_t* x, uint32_t* y,
    float u, float v);

/* Sampler Functions */

PFAPI pf_color_t
pf_texture2d_sample_nearest(
    const pf_texture2d_t* tex,
    float u, float v);

PFAPI pf_color_t
pf_texture2d_sample_bilinear(
    const pf_texture2d_t* tex,
    float u, float v);

#endif //PF_TEXTURE2D_H
