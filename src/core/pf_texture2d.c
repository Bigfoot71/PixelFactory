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

#include "pixelfactory/core/pf_texture2d.h"
#include "pixelfactory/components/pf_pixel.h"
#include <stddef.h>
#include <string.h>

pf_texture2d_t
pf_texture2d_create(
    void* pixels, uint32_t w, uint32_t h,
    pf_pixelformat_e format)
{
    pf_texture2d_t texture = { 0 };
    if (w == 0 || h == 0) return texture;

    texture.texels = pixels;
    texture.w = w;
    texture.h = h;

    texture.sampler = pf_texture2d_sample_nearest;

    if ((w & (w - 1)) == 0 && (h & (h - 1)) == 0) {
        texture.mapper = pf_texture2d_uv_map_wrap_pot;
    } else {
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

pf_texture2d_t
pf_texture2d_create_with_copy(
    void* pixels, uint32_t w, uint32_t h,
    pf_pixelformat_e format)
{
    pf_texture2d_t texture = { 0 };
    if (w == 0 || h == 0) return texture;

    size_t size = w * h * pf_pixel_get_bytes(format);

    texture.texels = malloc(size);
    memcpy(texture.texels, pixels, size);

    texture.w = w;
    texture.h = h;

    texture.sampler = pf_texture2d_sample_nearest;

    if ((w & (w - 1)) == 0 && (h & (h - 1)) == 0) {
        texture.mapper = pf_texture2d_uv_map_wrap_pot;
    } else {
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
pf_texture2d_delete(
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


/* TEXTURE2D EXTENSION */


#ifdef PF_EXT_TEXTURE2D

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

pf_texture2d_t
pfext_texture2d_load(
    const char* file_path)
{
    pf_pixelformat_e format = PF_PIXELFORMAT_UNKNOWN;
    pf_texture2d_t texture = { 0 };

    int w, h, comp;
    void* buffer = stbi_load(file_path, &w, &h, &comp, 0);

    if (buffer == NULL) {
        fprintf(stderr, "ERROR: %s\n", stbi_failure_reason());
        return texture;
    }

    switch (comp) {
        case 1: format = PF_PIXELFORMAT_GRAY; break;
        case 2: format = PF_PIXELFORMAT_GRAY_ALPHA; break;
        case 3: format = PF_PIXELFORMAT_RGB888; break;
        case 4: format = PF_PIXELFORMAT_RGBA8888; break;
        default: format = PF_PIXELFORMAT_UNKNOWN; break;
    }

    texture = pf_texture2d_create(buffer, w, h, format);

    return texture;
}

pf_texture2d_t
pfext_texture2d_load_from_memory(
    const void* buffer, size_t size)
{
    pf_pixelformat_e format = PF_PIXELFORMAT_UNKNOWN;
    pf_texture2d_t texture = { 0 };

    int w, h, comp;
    void* img_data = stbi_load_from_memory(buffer, (int)size, &w, &h, &comp, 0);

    if (img_data == NULL) {
        fprintf(stderr, "ERROR: %s\n", stbi_failure_reason());
        return texture;
    }

    switch (comp) {
        case 1: format = PF_PIXELFORMAT_GRAY; break;
        case 2: format = PF_PIXELFORMAT_GRAY_ALPHA; break;
        case 3: format = PF_PIXELFORMAT_RGB888; break;
        case 4: format = PF_PIXELFORMAT_RGBA8888; break;
        default: format = PF_PIXELFORMAT_UNKNOWN; break;
    }

    texture = pf_texture2d_create(img_data, w, h, format);

    return texture;
}

pf_texture2d_t
pfext_texture2d_gen_color(
    uint32_t w, uint32_t h,
    pf_color_t color, pf_pixelformat_e format)
{
    pf_texture2d_t texture = { 0 };
    if (format <= PF_PIXELFORMAT_UNKNOWN) return texture;

    size_t size = w * h;

    void* buffer = PF_MALLOC(size * pf_pixel_get_bytes(format));
    if (buffer == NULL) return texture;

    if (format == PF_PIXELFORMAT_RGBA8888) {
#if PF_SIMD_SIZE > 1
        pf_simd_i_t vc = pf_simd_set1_i32(color.v);
        size_t i = 0;
        for (; i + PF_SIMD_SIZE - 1 < size; i += PF_SIMD_SIZE) {
            pf_simd_store_i32((pf_simd_i_t*)(buffer + i), vc);
        }
        for (; i < size; ++i) {
            ((pf_color_t*)buffer)[i] = color;
        }
#else
        for (size_t i = 0; i < size; ++i) {
            ((pf_color_t*)buffer)[i] = color;
        }
#endif
    } else {
        pf_pixel_setter_fn setter;
        pf_pixel_default_getter_setter(NULL, &setter, format);
        for (size_t i = 0; i < size; ++i)  setter(buffer, i, color);
    }

    texture = pf_texture2d_create(buffer, w, h, PF_PIXELFORMAT_RGBA8888);

    return texture;
}

pf_texture2d_t
pfext_texture2d_gen_checkerboard(
    uint32_t w, uint32_t h,
    pf_color_t c1, pf_color_t c2,
    uint32_t tile_size, pf_pixelformat_e format)
{
    pf_texture2d_t texture = { 0 };
    pf_color_t* buffer = PF_MALLOC(w * h * sizeof(pf_color_t));

    if (buffer == NULL) return texture;

    pf_pixel_setter_fn setter;
    pf_pixel_default_getter_setter(NULL, &setter, format);

    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            if (((x / tile_size) + (y / tile_size)) % 2 == 0) {
                setter(buffer, y * w + x, c1);
            } else {
                setter(buffer, y * w + x, c2);
            }
        }
    }

    texture = pf_texture2d_create(buffer, w, h, PF_PIXELFORMAT_RGBA8888);

    return texture;
}

pf_texture2d_t
pfext_texture2d_gen_gradient(
    uint32_t w, uint32_t h,
    pf_color_t c1, pf_color_t c2,
    bool radial, pf_pixelformat_e format)
{
    pf_texture2d_t texture = { 0 };
    pf_color_t* buffer = PF_MALLOC(w * h * sizeof(pf_color_t));

    if (buffer == NULL) return texture;

    pf_pixel_setter_fn setter;
    pf_pixel_default_getter_setter(NULL, &setter, format);

    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            float u = (float)x / (w - 1);
            if (radial) {
                float v = (float)y / (h - 1);
                float dist = sqrtf((u - 0.5f) * (u - 0.5f) + (v - 0.5f) * (v - 0.5f));
                u = dist;
            }
            setter(buffer, y * w + x, pf_color_lerpf(c1, c2, u));
        }
    }

    texture = pf_texture2d_create(buffer, w, h, PF_PIXELFORMAT_RGBA8888);

    return texture;
}

pf_texture2d_t
pfext_texture2d_gen_white_noise(
    uint32_t w, uint32_t h,
    uint32_t seed, pf_pixelformat_e format)
{
    pf_texture2d_t texture = { 0 };
    pf_color_t* buffer = PF_MALLOC(w * h * sizeof(pf_color_t));

    if (buffer == NULL) return texture;

    pf_pixel_setter_fn setter;
    pf_pixel_default_getter_setter(NULL, &setter, format);

    srand(seed);

    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            setter(buffer, y * w + x, (rand() % 2) == 0 ? PF_BLACK : PF_WHITE);
        }
    }

    texture = pf_texture2d_create(buffer, w, h, PF_PIXELFORMAT_RGBA8888);

    return texture;
}

#endif
