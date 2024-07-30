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

#ifndef PF_FRAMEBUFFER_H
#define PF_FRAMEBUFFER_H

#include "../components/pf_color.h"

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
pf_framebuffer_create(
    uint32_t w, uint32_t h,
    pf_color_t def);

PFAPI void
pf_framebuffer_delete(
    pf_framebuffer_t* fb);

PFAPI bool
pf_framebuffer_is_valid(
    const pf_framebuffer_t* fb);

PFAPI void
pf_framebuffer_copy(
    pf_framebuffer_t * restrict dst_fb,
    const uint32_t dst_rect[4],
    const pf_framebuffer_t * restrict src_fb,
    const uint32_t src_rect[4]);

PFAPI void
pf_framebuffer_copy_pixels(
    void * restrict dst,
    const pf_framebuffer_t * restrict src_fb,
    const uint32_t src_rect[4]);

PFAPI pf_color_t
pf_framebuffer_get(
    const pf_framebuffer_t* fb,
    uint32_t x, uint32_t y);

PFAPI void
pf_framebuffer_put(
    pf_framebuffer_t* fb,
    uint32_t x, uint32_t y,
    pf_color_t color);

PFAPI void
pf_framebuffer_fill(
    pf_framebuffer_t* fb,
    const uint32_t rect[4],
    pf_color_t color);

PFAPI void
pf_framebuffer_map(
    pf_framebuffer_t* fb,
    const uint32_t rect[4],
    pf_framebuffer_map_fn func);

PFAPI int
pf_framebuffer_export_as_bmp(
    const pf_framebuffer_t* framebuffer,
    const char* filename);

#endif //PF_FRAMEBUFFER_H
