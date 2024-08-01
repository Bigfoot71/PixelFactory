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
#include "pixelfactory/math/pf_vec2.h"

/* Macros */

#define PF_CIRCLE_TRAVEL(PIXEL_CODE)                                                    \
    int x = 0;                                                                          \
    int y = radius;                                                                     \
    int d = 3 - 2 * radius;                                                             \
    while (y >= x) {                                                                    \
        for (int i = cx - x; i <= cx + x; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + y >= 0 && cy + y < (int)rn->fb.h) {                            \
                    size_t offset = (cy + y) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
                if (cy - y >= 0 && cy - y < (int)rn->fb.h) {                            \
                    size_t offset = (cy - y) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        for (int i = cx - y; i <= cx + y; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + x >= 0 && cy + x < (int)rn->fb.h) {                            \
                    size_t offset = (cy + x) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
                if (cy - x >= 0 && cy - x < (int)rn->fb.h) {                            \
                    size_t offset = (cy - x) * rn->fb.w + i;                            \
                    PIXEL_CODE                                                          \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        ++x;                                                                            \
        if (d > 0) {                                                                    \
            --y;                                                                        \
            d = d + 4 * (x - y) + 10;                                                   \
        } else {                                                                        \
            d = d + 4 * x + 6;                                                          \
        }                                                                               \
    }

#define PF_CIRCLE_TRAVEL_EX(PC_A, PC_B, PC_C, PC_D)                                     \
    int x = 0;                                                                          \
    int y = radius;                                                                     \
    int d = 3 - 2 * radius;                                                             \
    while (y >= x) {                                                                    \
        for (int i = cx - x; i <= cx + x; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + y >= 0 && cy + y < (int)rn->fb.h) {                            \
                    size_t offset = (cy + y) * rn->fb.w + i;                            \
                    PC_A                                                                \
                }                                                                       \
                if (cy - y >= 0 && cy - y < (int)rn->fb.h) {                            \
                    size_t offset = (cy - y) * rn->fb.w + i;                            \
                    PC_B                                                                \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        for (int i = cx - y; i <= cx + y; ++i) {                                        \
            if (i >= 0 && i < (int)rn->fb.w) {                                          \
                if (cy + x >= 0 && cy + x < (int)rn->fb.h) {                            \
                    size_t offset = (cy + x) * rn->fb.w + i;                            \
                    PC_C                                                                \
                }                                                                       \
                if (cy - x >= 0 && cy - x < (int)rn->fb.h) {                            \
                    size_t offset = (cy - x) * rn->fb.w + i;                            \
                    PC_D                                                                \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        ++x;                                                                            \
        if (d > 0) {                                                                    \
            --y;                                                                        \
            d = d + 4 * (x - y) + 10;                                                   \
        } else {                                                                        \
            d = d + 4 * x + 6;                                                          \
        }                                                                               \
    }

#define PF_CIRCLE_LINE_TRAVEL(PIXEL_CODE)                                               \
    int x = 0;                                                                          \
    int y = radius;                                                                     \
    int d = 3 - 2 * radius;                                                             \
    while (y >= x) {                                                                    \
        int px1 = cx + x, px2 = cx - x;                                                 \
        int py1 = cy + y, py2 = cy - y;                                                 \
        int px3 = cx + y, px4 = cx - y;                                                 \
        int py3 = cy + x, py4 = cy - x;                                                 \
        if (px1 >= 0 && px1 < (int)rn->fb.w) {                                          \
            if (py1 >= 0 && py1 < (int)rn->fb.h) {                                      \
                size_t offset = py1 * rn->fb.w + px1;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py2 >= 0 && py2 < (int)rn->fb.h) {                                      \
                size_t offset = py2 * rn->fb.w + px1;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        if (px2 >= 0 && px2 < (int)rn->fb.w) {                                          \
            if (py1 >= 0 && py1 < (int)rn->fb.h) {                                      \
                size_t offset = py1 * rn->fb.w + px2;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py2 >= 0 && py2 < (int)rn->fb.h) {                                      \
                size_t offset = py2 * rn->fb.w + px2;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        if (px3 >= 0 && px3 < (int)rn->fb.w) {                                          \
            if (py3 >= 0 && py3 < (int)rn->fb.h) {                                      \
                size_t offset = py3 * rn->fb.w + px3;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py4 >= 0 && py4 < (int)rn->fb.h) {                                      \
                size_t offset = py4 * rn->fb.w + px3;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        if (px4 >= 0 && px4 < (int)rn->fb.w) {                                          \
            if (py3 >= 0 && py3 < (int)rn->fb.h) {                                      \
                size_t offset = py3 * rn->fb.w + px4;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
            if (py4 >= 0 && py4 < (int)rn->fb.h) {                                      \
                size_t offset = py4 * rn->fb.w + px4;                                   \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
        ++x;                                                                            \
        if (d > 0) {                                                                    \
            --y;                                                                        \
            d = d + 4 * (x - y) + 10;                                                   \
        } else {                                                                        \
            d = d + 4 * x + 6;                                                          \
        }                                                                               \
    }


/* Public API */

void
pf_renderer_point2d(
    pf_renderer_t* rn, int x, int y,
    pf_color_t color)
{
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        color = rn->conf2d->color_blend(pf_framebuffer_get(&rn->fb, x, y), color);
    }
    pf_framebuffer_put(&rn->fb, x, y, color);
}

void
pf_renderer_circle2d(
    pf_renderer_t* rn, int cx, int cy,
    int radius, pf_color_t color)
{
    // Transformation
    if (rn->conf2d != NULL) {
        PF_MATH_FLOAT* mat_view = rn->conf2d->mat_view;
        if (pf_mat3_is_identity(mat_view) != 0) {
            pf_vec2_transform_i(&cx, &cy, cx, cy, mat_view);
            float scale_x = sqrtf(mat_view[0] * mat_view[0] + mat_view[3] * mat_view[3]);
            float scale_y = sqrtf(mat_view[1] * mat_view[1] + mat_view[4] * mat_view[4]);
            float scale = (scale_x + scale_y) / 2.0f;
            radius = (int)(radius * scale + 0.5f);
        }
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_CIRCLE_TRAVEL({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, color);
        })
    }
    else {
        PF_CIRCLE_TRAVEL({
            rn->fb.buffer[offset] = color;
        })
    }
}

void
pf_renderer_circle2d_gradient(
    pf_renderer_t* rn, int cx, int cy, int radius,
    pf_color_t c1, pf_color_t c2)
{
    // Transformation
    if (rn->conf2d != NULL) {
        PF_MATH_FLOAT* mat_view = rn->conf2d->mat_view;
        if (pf_mat3_is_identity(mat_view) != 0) {
            pf_vec2_transform_i(&cx, &cy, cx, cy, mat_view);
            float scale_x = sqrtf(mat_view[0] * mat_view[0] + mat_view[3] * mat_view[3]);
            float scale_y = sqrtf(mat_view[1] * mat_view[1] + mat_view[4] * mat_view[4]);
            float scale = (scale_x + scale_y) / 2.0f;
            radius = (int)(radius * scale + 0.5f);
        }
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_CIRCLE_TRAVEL_EX({
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + y - cy) * (cy + y - cy)), radius));
        }, {
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - y - cy) * (cy - y - cy)), radius));
        }, {
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + x - cy) * (cy + x - cy)), radius));
        }, {
            pf_color_t* ptr = rn->fb.buffer + offset;
            *ptr = blend(*ptr, pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - x - cy) * (cy - x - cy)), radius));
        })
    } else {
        PF_CIRCLE_TRAVEL_EX({
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + y - cy) * (cy + y - cy)), radius);
        }, {
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - y - cy) * (cy - y - cy)), radius);
        }, {
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy + x - cy) * (cy + x - cy)), radius);
        }, {
            rn->fb.buffer[offset] = pf_color_lerpi(c1, c2,
                sqrtf((i - cx) * (i - cx) + (cy - x - cy) * (cy - x - cy)), radius);
        })
    }
}

void
pf_renderer_circle2d_map(
    pf_renderer_t* rn, int cx, int cy, int radius,
    const pf_proc2d_t* proc)
{
    // Transformation
    if (rn->conf2d != NULL) {
        PF_MATH_FLOAT* mat_view = rn->conf2d->mat_view;
        if (pf_mat3_is_identity(mat_view) != 0) {
            pf_vec2_transform_i(&cx, &cy, cx, cy, mat_view);
            float scale_x = sqrtf(mat_view[0] * mat_view[0] + mat_view[3] * mat_view[3]);
            float scale_y = sqrtf(mat_view[1] * mat_view[1] + mat_view[4] * mat_view[4]);
            float scale = (scale_x + scale_y) / 2.0f;
            radius = (int)(radius * scale + 0.5f);
        }
    }

    // Setup processor
    pf_proc2d_fragment_fn fragment = pf_proc2d_fragment_default;
    const void* uniforms = NULL;
    if (proc != NULL) {
        if (proc->fragment != NULL) fragment = proc->fragment;
        if (proc->uniforms != NULL) uniforms = proc->uniforms;
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_CIRCLE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_CIRCLE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = final_color;
        })
    }
}

void
pf_renderer_circle2d_lines(
    pf_renderer_t* rn, int cx, int cy,
    int radius, pf_color_t color)
{
    // Transformation
    if (rn->conf2d != NULL) {
        PF_MATH_FLOAT* mat_view = rn->conf2d->mat_view;
        if (pf_mat3_is_identity(mat_view) != 0) {
            pf_vec2_transform_i(&cx, &cy, cx, cy, mat_view);
            float scale_x = sqrtf(mat_view[0] * mat_view[0] + mat_view[3] * mat_view[3]);
            float scale_y = sqrtf(mat_view[1] * mat_view[1] + mat_view[4] * mat_view[4]);
            float scale = (scale_x + scale_y) / 2.0f;
            radius = (int)(radius * scale + 0.5f);
        }
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        if (blend != NULL) {
            PF_CIRCLE_LINE_TRAVEL({
                pf_color_t* ptr = rn->fb.buffer + offset;
                *ptr = blend(*ptr, color);
            })
        }
    } else {
        PF_CIRCLE_LINE_TRAVEL({
            rn->fb.buffer[offset] = color;
        })
    }
}

void
pf_renderer_circle2d_lines_map(
    pf_renderer_t* rn, int cx, int cy, int radius,
    const pf_proc2d_t* proc)
{
    // Transformation
    if (rn->conf2d != NULL) {
        PF_MATH_FLOAT* mat_view = rn->conf2d->mat_view;
        if (pf_mat3_is_identity(mat_view) != 0) {
            pf_vec2_transform_i(&cx, &cy, cx, cy, mat_view);
            float scale_x = sqrtf(mat_view[0] * mat_view[0] + mat_view[3] * mat_view[3]);
            float scale_y = sqrtf(mat_view[1] * mat_view[1] + mat_view[4] * mat_view[4]);
            float scale = (scale_x + scale_y) / 2.0f;
            radius = (int)(radius * scale + 0.5f);
        }
    }

    // Setup processor
    pf_proc2d_fragment_fn fragment = pf_proc2d_fragment_default;
    const void* uniforms = NULL;
    if (proc != NULL) {
        if (proc->fragment != NULL) fragment = proc->fragment;
        if (proc->uniforms != NULL) uniforms = proc->uniforms;
    }

    // Rendering
    if (rn->conf2d != NULL && rn->conf2d->color_blend != NULL) {
        pf_color_blend_fn blend = rn->conf2d->color_blend;
        PF_CIRCLE_LINE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = blend(*ptr, final_color);
        })
    } else {
        PF_CIRCLE_LINE_TRAVEL({
            pf_vertex_t vertex = pf_vertex_create_2d(x, y, 0, 0, PF_WHITE);
            pf_color_t *ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;

            fragment(rn, &vertex, &final_color, uniforms);
            *ptr = final_color;
        })
    }
}

void
pf_renderer_circle2d_lines_thick(
    pf_renderer_t* rn, int cx, int cy,
    int radius, int thick, pf_color_t color)
{
    int ht = thick/2;
    for (int i = -ht; i <= ht; ++i) {
        pf_renderer_circle2d_lines(rn, cx, cy, radius + i, color);
    }
}

void
pf_renderer_circle2d_lines_thick_map(
    pf_renderer_t* rn, int cx, int cy, int radius, int thick,
    const pf_proc2d_t* proc)
{
    int ht = thick/2;
    for (int i = -ht; i <= ht; ++i) {
        pf_renderer_circle2d_lines_map(rn, cx, cy, radius + i, proc);
    }
}
