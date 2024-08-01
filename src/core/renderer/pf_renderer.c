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

#include "pixelfactory/core/pf_renderer.h"
#include <float.h>

/* Internal Macros */

#define PF_RENDERER_MAP2D(PIXEL_CODE)                                   \
    for (int y = 0; y < (int)rn->fb.h; ++y) {                           \
        size_t y_offset = y * rn->fb.w;                                 \
        for (int x = 0; x < (int)rn->fb.w; ++x) {                       \
            pf_color_t* fb_ptr = rn->fb.buffer + y_offset + x;          \
            pf_color_t color = *fb_ptr;                                 \
            PIXEL_CODE                                                  \
            u += tx;                                                    \
        }                                                               \
        v += ty;                                                        \
    }

#define PF_RENDERER_MAP3D(PIXEL_CODE)                                   \
    for (int y = 0; y < (int)rn->fb.h; ++y) {                           \
        size_t y_offset = y * rn->fb.w;                                 \
        for (int x = 0; x < (int)rn->fb.w; ++x) {                       \
            size_t offset = y_offset + x;                               \
            pf_color_t* fb_ptr = rn->fb.buffer + offset;                \
            float* zb_ptr = rn->zb.buffer + offset;                     \
            pf_color_t color = *fb_ptr;                                 \
            float depth = *zb_ptr;                                      \
            PIXEL_CODE                                                  \
            u += tx;                                                    \
        }                                                               \
        v += ty;                                                        \
    }

#define PF_RENDERER_MAP2D_OMP(PIXEL_CODE)                               \
    _Pragma("omp parallel for                                           \
        if (rn->fb.w * rn->fb.h >= PF_OMP_BUFFER_MAP_SIZE_THRESHOLD)")  \
    PF_RENDERER_MAP2D(PIXEL_CODE)

#define PF_RENDERER_MAP3D_OMP(PIXEL_CODE)                               \
    _Pragma("omp parallel for                                           \
        if (rn->fb.w * rn->fb.h >= PF_OMP_BUFFER_MAP_SIZE_THRESHOLD)")  \
    PF_RENDERER_MAP3D(PIXEL_CODE)

/* Public API */

pf_renderer_t
pf_renderer_load(
    uint32_t w, uint32_t h,
    pf_renderer_flag_e flags)
{
    pf_renderer_t rn = { 0 };

    rn.fb = pf_framebuffer_create(w, h, PF_BLANK);

    if (flags & PF_RENDERER_2D) {
        rn.conf2d = PF_CALLOC(1, sizeof(pf_renderer_config_2d_t));
        pf_mat3_identity(rn.conf2d->mat_view);
    }

    if (flags & PF_RENDERER_3D) {
        rn.conf3d = PF_CALLOC(1, sizeof(pf_renderer_config_3d_t));

        pf_mat4_identity(rn.conf3d->mat_view);
        pf_mat4_perspective(rn.conf3d->mat_proj, 45.0, (w > h) ? (float)w / h : (float)h / w, 0.01f, 1000.0f);

        rn.zb = pf_depthbuffer_create(w, h, FLT_MAX);

        rn.conf3d->viewport_pos[0] = 0;
        rn.conf3d->viewport_pos[1] = 0;

        rn.conf3d->viewport_dim[0] = w - 1;
        rn.conf3d->viewport_dim[1] = h - 1;

        rn.conf3d->cull_mode = PF_CULL_NONE;
    }

    return rn;
}

void
pf_renderer_delete(
    pf_renderer_t* rn)
{
    pf_framebuffer_delete(&rn->fb);
    pf_depthbuffer_delete(&rn->zb);

    if (rn->conf2d != NULL) PF_FREE(rn->conf2d);
    if (rn->conf3d != NULL) PF_FREE(rn->conf3d);

    *rn = (pf_renderer_t) { 0 };
}

bool
pf_renderer_is_valid(
    pf_renderer_t* rn,
    pf_renderer_flag_e flags)
{
    bool valid = pf_framebuffer_is_valid(&rn->fb);
    if (valid && (flags & PF_RENDERER_2D)) {
        valid = (rn->conf2d != NULL);
    }
    if (valid && (flags & PF_RENDERER_3D)) {
        valid = (rn->conf3d != NULL)
            && pf_depthbuffer_is_valid(&rn->zb);
    }
    return valid;
}

void
pf_renderer_clear2d(
    pf_renderer_t* rn,
    pf_color_t clear_color)
{
    if (rn->conf2d == NULL) {
        return;
    }

    size_t size = rn->fb.w * rn->fb.h;
    pf_color_t* fb = rn->fb.buffer;

#   if PF_SIMD_SIZE > 1
    pf_simd_i_t clear_color_vec = pf_simd_set1_i32(clear_color.v);
    size_t i = 0;
    for (; i + PF_SIMD_SIZE - 1 < size; i += PF_SIMD_SIZE) {
        pf_simd_store_i32((pf_simd_i_t*)(fb + i), clear_color_vec);
    }
    for (; i < size; ++i) {
        fb[i] = clear_color;
    }
#else
#ifdef _OPENMP
#   pragma omp parallel for \
        if (size >= PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD)
#endif //_OPENMP
    for (size_t i = 0; i < size; ++i) {
        fb[i] = clear_color;
    }
#endif
}

void
pf_renderer_clear3d(
    pf_renderer_t* rn,
    pf_color_t clear_color,
    float clear_depth)
{
    if (rn->conf3d == NULL) {
        return;
    }

    size_t size = rn->fb.w * rn->fb.h;
    pf_color_t* fb = rn->fb.buffer;
    float* zb = rn->zb.buffer;

#if PF_SIMD_SIZE > 1
    pf_simd_i_t clear_color_vec = pf_simd_set1_i32(clear_color.v);
    pf_simd_t clear_depth_vec = pf_simd_set1_ps(clear_depth);
    size_t i = 0;
    for (; i + PF_SIMD_SIZE - 1 < size; i += PF_SIMD_SIZE) {
        pf_simd_store_i32((pf_simd_i_t*)(fb + i), clear_color_vec);
        pf_simd_store_ps(zb + i, clear_depth_vec);
    }
    for (; i < size; ++i) {
        fb[i] = clear_color;
        zb[i] = clear_depth;
    }
#else
#ifdef _OPENMP
#   pragma omp parallel for \
        if (size >= PF_OMP_CLEAR_BUFFER_SIZE_THRESHOLD)
#endif //_OPENMP
    for (size_t i = 0; i < size; ++i) {
        fb[i] = clear_color;
        zb[i] = clear_depth;
    }
#endif
}

void
pf_renderer_map3d(
    pf_renderer_t* rn,
    pf_renderer_map3d_fn func)
{
    if (rn->conf3d == NULL) {
        return;
    }

    float tx = 1.0f / rn->fb.w;
    float ty = 1.0f / rn->fb.h;
    float u = 0, v = 0;

#if defined(_OPENMP)
    if (rn->conf3d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf3d->color_blend;
        PF_RENDERER_MAP3D_OMP({
            func(rn, &color, &depth, x, y, u, v);
            *fb_ptr = blend(*fb_ptr, color);
            *zb_ptr = depth;
        })
    } else {
        PF_RENDERER_MAP3D_OMP({
            func(rn, &color, &depth, x, y, u, v);
            *fb_ptr = color;
            *zb_ptr = depth;
        })
    }
#else
    if (rn->blend != NULL) {
        pf_color_blend_fn blend = rn->conf3d->color_blend;
        PF_RENDERER_MAP3D({
            func(rn, &color, &depth, x, y, u, v);
            *fb_ptr = blend(*fb_ptr, color);
            *zb_ptr = depth;
        })
    } else {
        PF_RENDERER_MAP3D({
            func(rn, &color, &depth, x, y, u, v);
            *fb_ptr = color;
            *zb_ptr = depth;
        })
    }
#endif
}

void
pf_renderer_map2d(
    pf_renderer_t* rn,
    pf_renderer_map2d_fn func)
{
    if (rn->conf3d == NULL) {
        return;
    }

    float tx = 1.0f / rn->fb.w;
    float ty = 1.0f / rn->fb.h;
    float u = 0, v = 0;

#if defined(_OPENMP)
    if (rn->conf3d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf3d->color_blend;
        PF_RENDERER_MAP3D_OMP({
            func(rn, &color, x, y, u, v);
            *fb_ptr = blend(*fb_ptr, color);
            *zb_ptr = depth;
        })
    } else {
        PF_RENDERER_MAP3D_OMP({
            func(rn, &color, x, y, u, v);
            *fb_ptr = color;
            *zb_ptr = depth;
        })
    }
#else
    if (rn->blend != NULL) {
        pf_color_blend_fn blend = rn->conf3d->color_blend;
        PF_RENDERER_MAP3D({
            func(rn, &color, x, y, u, v);
            *fb_ptr = blend(*fb_ptr, color);
            *zb_ptr = depth;
        })
    } else {
        PF_RENDERER_MAP3D({
            func(rn, &color, x, y, u, v);
            *fb_ptr = color;
            *zb_ptr = depth;
        })
    }
#endif
}

void
pf_renderer_viewport(
    pf_renderer_t* rn,
    int x, int y, int w, int h)
{
    if (rn->conf3d != NULL) {
        rn->conf3d->viewport_pos[0] = x;
        rn->conf3d->viewport_pos[1] = y;
        rn->conf3d->viewport_dim[0] = w - 1;
        rn->conf3d->viewport_dim[1] = h - 1;
    }
}

void
pf_renderer_apply_camera2d(
    pf_renderer_t* rn,
    pf_camera2d_t* cam)
{
    if (rn->conf2d != NULL) {
        pf_camera2d_get_view_matrix(cam, rn->conf2d->mat_view);
    }
}

void
pf_renderer_apply_camera3d(
    pf_renderer_t* rn,
    pf_camera3d_t* cam)
{
    if (rn->conf2d != NULL) {
        pf_camera3d_get_view_matrix(cam, rn->conf3d->mat_view);
    }
}

void
pf_renderer_apply_camera3d_orthographic(
    pf_renderer_t* rn,
    pf_camera3d_t* cam)
{
    if (rn->conf2d != NULL) {
        pf_camera3d_get_view_matrix(cam, rn->conf3d->mat_view);
        pf_camera3d_get_orthographic_matrix(cam, rn->conf3d->mat_view);
    }
}

void
pf_renderer_apply_camera3d_perspective(
    pf_renderer_t* rn,
    pf_camera3d_t* cam)
{
    if (rn->conf2d != NULL) {
        pf_camera3d_get_view_matrix(cam, rn->conf3d->mat_view);
        pf_camera3d_get_perspective_matrix(cam, rn->conf3d->mat_view);
    }
}
