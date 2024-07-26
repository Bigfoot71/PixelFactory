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

#include "pixelfactory/pf_texture2d.h"
#include <string.h>

pf_texture2d_t
pf_texture2d_create(
    void* pixels, uint32_t w, uint32_t h,
    pf_pixelformat_t format)
{
    pf_texture2d_t texture = { 0 };
    if (w == 0 || h == 0) return texture;

    texture.texels = pixels;
    texture.w = w;
    texture.h = h;

    texture.sampler = pf_texture2d_sample_nearest;

    if ((w & (w - 1)) == 0 && (h & (h - 1)) == 0)
    {
        texture.mapper = pf_texture2d_uv_map_wrap_pot;
    }
    else
    {
        texture.mapper = pf_texture2d_uv_map_wrap;
    }

    pf_pixel_default_getter_setter(
        &texture.getter,
        &texture.setter,
        format);

    texture.tx = 1.0f/w;
    texture.ty = 1.0f/h;

    return texture;
}

void
pf_texture2d_destroy(
    pf_texture2d_t* tex)
{
    PF_FREE(tex->texels);
    memset(tex, 0, sizeof(pf_texture2d_t));
}

/* Map Functions */

void
pf_texture2d_uv_map_wrap_pot(
    const pf_texture2d_t* tex,
    uint32_t* x, uint32_t* y,
    float u, float v)
{
    *x = (uint32_t)((u - (int)u)*(tex->w - 1)) & (tex->w - 1);
    *y = (uint32_t)((v - (int)v)*(tex->h - 1)) & (tex->h - 1);
}

void
pf_texture2d_uv_map_wrap(
    const pf_texture2d_t* tex,
    uint32_t* x, uint32_t* y,
    float u, float v)
{
    *x = (uint32_t)((u - (int)u)*(tex->w - 1)) % tex->w;
    *y = (uint32_t)((v - (int)v)*(tex->h - 1)) % tex->h;
}

void
pf_texture2d_uv_map_clamp(
    const pf_texture2d_t* tex,
    uint32_t* x, uint32_t* y,
    float u, float v)
{
    *x = (uint32_t)PF_CLAMP((int)(u*(tex->w - 1)), 0, (int)tex->w - 1);
    *y = (uint32_t)PF_CLAMP((int)(v*(tex->h - 1)), 0, (int)tex->h - 1);
}


/* Sampler Functions */

pf_color_t
pf_texture2d_sample_nearest(
    const pf_texture2d_t* tex,
    float u, float v)
{
    uint32_t x, y; tex->mapper(tex, &x, &y, u, v);
    return tex->getter(tex->texels, y * tex->w + x);
}

pf_color_t
pf_texture2d_sample_bilinear(
    const pf_texture2d_t* tex,
    float u, float v)
{
    uint32_t x0, y0, x1, y1;
    float fx, fy;

    tex->mapper(tex, &x0, &y0, u, v);
    tex->mapper(tex, &x1, &y1, u + tex->tx, v + tex->ty);

    // Calculer les fractions fx, fy
    fx = u * tex->w - x0;
    fy = v * tex->h - y0;

    // Obtenir les couleurs des quatre pixels
    pf_color_t c00 = tex->getter(tex->texels, y0 * tex->w + x0);
    pf_color_t c10 = tex->getter(tex->texels, y0 * tex->w + x1);
    pf_color_t c01 = tex->getter(tex->texels, y1 * tex->w + x0);
    pf_color_t c11 = tex->getter(tex->texels, y1 * tex->w + x1);

    // Interpoler les couleurs horizontalement
    pf_color_t c0 = pf_color_lerpf(c00, c10, fx);
    pf_color_t c1 = pf_color_lerpf(c01, c11, fx);

    // Interpoler les couleurs verticalement
    return pf_color_lerpf(c0, c1, fy);
}
