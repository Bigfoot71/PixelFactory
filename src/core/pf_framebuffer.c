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

#include "pixelfactory/core/pf_framebuffer.h"
#include "pixelfactory/misc/pf_stdinc.h"
#include "pixelfactory/misc/pf_helper.h"
#include <stdint.h>

pf_framebuffer_t
pf_framebuffer_create(
    uint32_t w, uint32_t h,
    pf_color_t def)
{
    pf_framebuffer_t result = { 0 };
    if (w == 0 || h == 0) return result;

    pf_color_t* buffer = PF_MALLOC(w*h*sizeof(pf_color_t));
    if (buffer == NULL) return result;

    pf_color_t* end = buffer + w*h;
    pf_color_t* ptr = buffer;
    while (ptr < end) {
        *ptr++ = def;
    }

    result.buffer = buffer;
    result.w = w;
    result.h = h;

    return result;
}

void
pf_framebuffer_delete(
    pf_framebuffer_t* fb)
{
    PF_FREE(fb->buffer);
    fb->buffer = NULL;
    fb->w = fb->h = 0;
}

int
pf_framebuffer_is_valid(
    const pf_framebuffer_t* fb)
{
    return (fb->buffer != NULL || fb->w > 0 || fb->h > 0);
}

void
pf_framebuffer_copy(
    pf_framebuffer_t * restrict dst_fb,
    const uint32_t dst_rect[4],
    const pf_framebuffer_t * restrict src_fb,
    const uint32_t src_rect[4])
{
    int dst_xmin, dst_ymin, dst_xmax, dst_ymax;
    int src_xmin, src_ymin, src_xmax, src_ymax;

    if (dst_fb == NULL || dst_fb->buffer == NULL
     || src_fb == NULL || src_fb->buffer == NULL) {
        return;
    }

    if (dst_rect == NULL) {
        dst_xmin = dst_ymin = 0;
        dst_xmax = dst_fb->w - 1;
        dst_ymax = dst_fb->h - 1;
    } else {
        dst_xmin = PF_MIN(dst_rect[0], dst_fb->w - 1);
        dst_ymin = PF_MIN(dst_rect[1], dst_fb->h - 1);
        dst_xmax = PF_MIN(dst_rect[2], dst_fb->w - 1);
        dst_ymax = PF_MIN(dst_rect[3], dst_fb->h - 1);
        if (dst_xmin > dst_xmax) PF_SWAP(dst_xmin, dst_xmax);
        if (dst_ymin > dst_ymax) PF_SWAP(dst_ymin, dst_ymax);
    }

    if (src_rect == NULL) {
        src_xmin = src_ymin = 0;
        src_xmax = src_fb->w - 1;
        src_ymax = src_fb->h - 1;
    } else {
        src_xmin = PF_MIN(src_rect[0], src_fb->w - 1);
        src_ymin = PF_MIN(src_rect[1], src_fb->h - 1);
        src_xmax = PF_MIN(src_rect[2], src_fb->w - 1);
        src_ymax = PF_MIN(src_rect[3], src_fb->h - 1);
        if (src_xmin > src_xmax) PF_SWAP(src_xmin, src_xmax);
        if (src_ymin > src_ymax) PF_SWAP(src_ymin, src_ymax);
    }

    float inv_dst_w = 1.0f / (float)dst_fb->w;
    float inv_dst_h = 1.0f / (float)dst_fb->h;

#ifdef _OPENMP
#   pragma omp parallel for\
        if ((dst_xmax - dst_xmin) * (dst_ymax - dst_ymin) > PF_OMP_BUFFER_COPY_SIZE_THRESHOLD)
#endif //_OPENMP
    for (int y = dst_ymin; y <= dst_ymax; ++y)
    {
        size_t y_dst_offset = y * dst_fb->w;
        size_t y_src_offset = y * inv_dst_h * (src_ymax - src_ymin) + src_ymin;

        for (int x = dst_xmin; x <= dst_xmax; ++x)
        {
            size_t dst_offset = y_dst_offset + x;
            size_t src_offset = y_src_offset + x * inv_dst_w * (src_xmax - src_xmin) + src_xmin;

            dst_fb->buffer[dst_offset] = src_fb->buffer[src_offset];
        }
    }
}

void
pf_framebuffer_copy_pixels(
    void * restrict dst,
    const pf_framebuffer_t * restrict src_fb,
    const uint32_t src_rect[4])
{
    int xmin, ymin, xmax, ymax;
    
    if (dst == NULL || src_fb == NULL || src_fb->buffer == NULL) {
        return;
    }
    
    if (src_rect == NULL) {
        xmin = ymin = 0;
        xmax = src_fb->w - 1;
        ymax = src_fb->h - 1;
    } else {
        xmin = PF_MIN(src_rect[0], src_fb->w - 1);
        ymin = PF_MIN(src_rect[1], src_fb->h - 1);
        xmax = PF_MIN(src_rect[2], src_fb->w - 1);
        ymax = PF_MIN(src_rect[3], src_fb->h - 1);
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }

    uint32_t *dst_ptr = (uint32_t*)dst;
    pf_simd_i_t src_vector;
    
    for (int y = ymin; y <= ymax; ++y) {
        const pf_color_t *src_row_ptr = src_fb->buffer + y * src_fb->w + xmin;
        int x = xmin;
        
        for (; x <= xmax - PF_SIMD_SIZE + 1; x += PF_SIMD_SIZE) {
            src_vector = pf_simd_load_i32(src_row_ptr + x);
            pf_simd_store_i32(dst_ptr + x, src_vector);
        }
        
        // Copie des pixels restants
        for (; x <= xmax; ++x) {
            dst_ptr[x] = src_row_ptr[x].v;
        }
    }
}

pf_color_t
pf_framebuffer_get(
    const pf_framebuffer_t* fb,
    uint32_t x, uint32_t y)
{
    pf_color_t result = { 0 };
    if (x < fb->w && y < fb->h) {
        result = fb->buffer[y * fb->w + x];
    }
    return result;
}

void
pf_framebuffer_put(
    pf_framebuffer_t* fb,
    uint32_t x, uint32_t y,
    pf_color_t color)
{
    if (x < fb->w && y < fb->h) {
        fb->buffer[y * fb->w + x] = color;
    }
}

void
pf_framebuffer_fill(
    pf_framebuffer_t* fb,
    const uint32_t rect[4],
    pf_color_t color)
{
    int xmin, ymin, xmax, ymax;
    
    if (fb == NULL || fb->buffer == NULL) {
        return;
    }
    
    if (rect == NULL) {
        xmin = 0;
        ymin = 0;
        xmax = fb->w - 1;
        ymax = fb->h - 1;
    } else {
        xmin = PF_MIN(rect[0], fb->w - 1);
        ymin = PF_MIN(rect[1], fb->h - 1);
        xmax = PF_MIN(rect[2], fb->w - 1);
        ymax = PF_MIN(rect[3], fb->h - 1);
        
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }

    pf_color_t* row_ptr;
    pf_simd_i_t color_vector = pf_simd_set1_i32(color.v);

    for (int y = ymin; y <= ymax; ++y) {
        row_ptr = fb->buffer + y * fb->w + xmin;
        int x = xmin;
        for (; x <= xmax - PF_SIMD_SIZE + 1; x += PF_SIMD_SIZE) {
            pf_simd_store_i32(row_ptr + x, color_vector);
        }
        for (; x <= xmax; ++x) {
            row_ptr[x] = color;
        }
    }
}

void
pf_framebuffer_map(
    pf_framebuffer_t* fb,
    const uint32_t rect[4],
    pf_framebuffer_map_fn func)
{
    int xmin, ymin, xmax, ymax;

    if (fb == NULL || fb->buffer == NULL) {
        return;
    }

    if (rect == NULL) {
        xmin = ymin = 0;
        xmax = fb->w - 1;
        ymax = fb->h - 1;
    } else {
        xmin = PF_MIN(rect[0], fb->w - 1);
        ymin = PF_MIN(rect[1], fb->h - 1);
        xmax = PF_MIN(rect[2], fb->w - 1);
        ymax = PF_MIN(rect[3], fb->h - 1);
        if (xmin > xmax) PF_SWAP(xmin, xmax);
        if (ymin > ymax) PF_SWAP(ymin, ymax);
    }

#ifdef _OPENMP
#   pragma omp parallel for\
        if ((xmax - xmin) * (ymax - ymin) > PF_OMP_BUFFER_MAP_SIZE_THRESHOLD)
#endif //_OPENMP
    for (int y = ymin; y <= ymax; ++y) {
        pf_color_t* ptr = fb->buffer + y * fb->w + xmin;
        for (int x = xmin; x <= xmax; ++x) {
            func(fb, ptr++, x, y);
        }
    }
}