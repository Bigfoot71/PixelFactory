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

#ifndef PF_PIXEL_H
#define PF_PIXEL_H

#include "pf_color.h"

typedef pf_color_t (*pf_pixel_getter_fn)(const void*, size_t);
typedef void (*pf_pixel_setter_fn)(void*, size_t, pf_color_t);

typedef enum {
    PF_PIXELFORMAT_UNKNOWN = 0,
    PF_PIXELFORMAT_GRAY,
    PF_PIXELFORMAT_GRAY_ALPHA,
    PF_PIXELFORMAT_RGB565,
    PF_PIXELFORMAT_RGB888,
    PF_PIXELFORMAT_RGBA5551,
    PF_PIXELFORMAT_RGBA4444,
    PF_PIXELFORMAT_RGBA8888,
    PF_PIXELFORMAT_R32,
    PF_PIXELFORMAT_RGB323232,
    PF_PIXELFORMAT_RGBA32323232,
    PF_PIXELFORMAT_R16,
    PF_PIXELFORMAT_RGB161616,
    PF_PIXELFORMAT_RGBA16161616,
} pf_pixelformat_t;


/* Pixel Getters */

pf_color_t
pf_pixel_get_gray(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_gray_alpha(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgb565(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgb888(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgba5551(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgba4444(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgba8888(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_r32(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgb323232(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgba32323232(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_r16(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgb161616(
    const void* pixels,
    size_t offset);

pf_color_t
pf_pixel_get_rgba16161616(
    const void* pixels,
    size_t offset);


/* Pixel Setters */

void
pf_pixel_set_gray(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_gray_alpha(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgb565(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgb888(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgba5551(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgba4444(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgba8888(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_r32(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgb323232(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgba32323232(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_r16(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgb161616(
    void* pixels,
    size_t offset,
    pf_color_t color);

void
pf_pixel_set_rgba16161616(
    void* pixels,
    size_t offset,
    pf_color_t color);


/* Helper Functions */

void
pf_pixel_default_getter_setter(
    pf_pixel_getter_fn* getter,
    pf_pixel_setter_fn* setter,
    pf_pixelformat_t format);

size_t
pf_pixel_get_bytes(
    pf_pixelformat_t format);


#endif //PF_PIXEL_H