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

#include "pixelfactory/core/pf_renderer3d.h"

/* Internal Drawing Macros */

#define PF_POINT_THICK_TRAVEL_NODEPTH(PIXEL_CODE)                               \
    float rSq = radius*radius;                                                  \
    for (int y = -radius; y <= radius; ++y) {                                   \
        uint32_t py = screen_pos[1] + y;                                        \
        for (int x = -radius; x <= radius; ++x) {                               \
            if (y*y + x*x <= rSq){                                              \
                uint32_t px = screen_pos[0] + x;                                \
                size_t offset = py * rn->fb.w + px;                             \
                if (px < rn->fb.w && py < rn->fb.h) {                           \
                    rn->zb.buffer[offset] = homogen[2];                         \
                    PIXEL_CODE                                                  \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }

#define PF_POINT_THICK_TRAVEL_DEPTH(PIXEL_CODE)                                 \
    float rSq = radius*radius;                                                  \
    for (int y = -radius; y <= radius; ++y) {                                   \
        uint32_t py = screen_pos[1] + y;                                        \
        for (int x = -radius; x <= radius; ++x) {                               \
            if (y*y + x*x <= rSq){                                              \
                uint32_t px = screen_pos[0] + x;                                \
                size_t offset = py * rn->fb.w + px;                             \
                if (px < rn->fb.w && py < rn->fb.h) {                           \
                    if (rn->test(rn->zb.buffer[offset], homogen[2])) {          \
                        rn->zb.buffer[offset] = homogen[2];                     \
                        PIXEL_CODE                                              \
                    }                                                           \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }

/* Internal Pixel Code Macros */

#define PF_PIXEL_CODE_NOBLEND()                                         \
    pf_color_t* ptr = rn->fb.buffer + offset;                           \
    pf_color_t final_color = *ptr;                                      \
    proc->fragment(rn, &vertex, &final_color, proc->uniforms);          \
    *ptr = final_color;

#define PF_PIXEL_CODE_BLEND()                                           \
    pf_color_t* ptr = rn->fb.buffer + offset;                           \
    pf_color_t final_color = *ptr;                                      \
    proc->fragment(rn, &vertex, &final_color, proc->uniforms);          \
    *ptr = rn->blend(*ptr, final_color);

/* Helper Function Declarations */

void
pf_renderer3d_screen_projection_INTERNAL(
    const pf_renderer3d_t* rn,
    pf_vec4_t* homogeneous,
    pf_vertex_t* vertices,
    size_t vertices_count,
    int screen_pos[][2]);

/* Internal Clipping Function */

static void
pf_clip3d_point_INTERNAL(
    const struct pf_renderer3d* rn,
    pf_vertex_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count)
{
    (void)rn;
    (void)out_vertices;

    for (int_fast8_t i = 0; i < 3; ++i)
    {
        if ((*out_homogeneous)[i] < -(*out_homogeneous)[3] || (*out_homogeneous)[i] > (*out_homogeneous)[3])
        {
            *out_vertices_count = 0;
            return;
        }
    }
}

/* Internal Rendering Functions */

void
pf_renderer3d_point_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc)
{
    pf_vertex_t vertex = *point;
    pf_vec4_t homogen = { 0 };
    int screen_pos[2] = { 0 };
    size_t num = 1;

    proc->vertex(&vertex, homogen, mat_model, mat_normal, mat_mvp, proc->uniforms);
    pf_clip3d_point_INTERNAL(rn, &vertex, &homogen, &num);
    if (num != 1) return;

    pf_renderer3d_screen_projection_INTERNAL(
        rn, &homogen, &vertex, num, &screen_pos);

    if (radius == 0) {
        size_t offset = screen_pos[1] * rn->fb.w + screen_pos[0];
        if (rn->test != NULL && rn->test(rn->zb.buffer[offset], homogen[2])) {
            pf_color_t* ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;
            proc->fragment(rn, &vertex, &final_color, proc->uniforms);
            *ptr = (rn->blend != NULL) ? rn->blend(*ptr, final_color) : final_color;
        } else {
            pf_color_t* ptr = rn->fb.buffer + offset;
            pf_color_t final_color = *ptr;
            proc->fragment(rn, &vertex, &final_color, proc->uniforms);
            *ptr = (rn->blend != NULL) ? rn->blend(*ptr, final_color) : final_color;
        }
    } else {
        if (rn->test != NULL) {
            if (rn->blend != NULL) {
                PF_POINT_THICK_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_POINT_THICK_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        } else {
            if (rn->blend != NULL) {
                PF_POINT_THICK_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_POINT_THICK_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        }
    }
}
