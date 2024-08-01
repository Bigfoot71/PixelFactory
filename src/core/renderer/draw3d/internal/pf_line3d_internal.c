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

#include "pixelfactory/components/pf_depth.h"
#include "pixelfactory/core/pf_renderer.h"

/* Internal Rasterization Macros */

#define PF_LINE_TRAVEL_NODEPTH(PIXEL_CODE)                                              \
    int_fast8_t y_longer = 0;                                                           \
    int short_len = y2 - y1;                                                            \
    int long_len = x2 - x1;                                                             \
    /*
        Determine if the line is more vertical than horizontal
    */                                                                                  \
    if (abs(short_len) > abs(long_len)) {                                               \
        PF_SWAP(short_len, long_len);                                                   \
        y_longer = 1;                                                                   \
    }                                                                                   \
    /*
        Initialize variables for drawing loop
    */                                                                                  \
    int end = long_len;                                                                 \
    float inv_end = 1.0f / end;                                                         \
    int_fast8_t sign = 1;                                                               \
    /*
        Adjust direction increment based on long_len sign
    */                                                                                  \
    if (long_len < 0) {                                                                 \
        long_len = -long_len;                                                           \
        sign = -1;                                                                      \
    }                                                                                   \
    /*
        Calculate fixed-point increment for shorter length
    */                                                                                  \
    int dec = (long_len == 0) ? 0 : (short_len << 16) / long_len;                       \
    /*
        Travel the line pixel by pixel
    */                                                                                  \
    if (y_longer) {                                                                     \
        /*
            If line is more vertical, iterate over y-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + (j >> 16), y = y1 + i;                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            rn->zb.buffer[offset] = z;                                                  \
            PIXEL_CODE                                                                  \
        }                                                                               \
    } else {                                                                            \
        /*
            If line is more horizontal, iterate over x-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + i, y = y1 + (j >> 16);                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            rn->zb.buffer[offset] = z;                                                  \
            PIXEL_CODE                                                                  \
        }                                                                               \
    }

#define PF_LINE_TRAVEL_DEPTH(PIXEL_CODE)                                                \
    int_fast8_t y_longer = 0;                                                           \
    int short_len = y2 - y1;                                                            \
    int long_len = x2 - x1;                                                             \
    /*
        Determine if the line is more vertical than horizontal
    */                                                                                  \
    if (abs(short_len) > abs(long_len)) {                                               \
        PF_SWAP(short_len, long_len);                                                   \
        y_longer = 1;                                                                   \
    }                                                                                   \
    /*
        Initialize variables for drawing loop
    */                                                                                  \
    int end = long_len;                                                                 \
    float inv_end = 1.0f / end;                                                         \
    int_fast8_t sign = 1;                                                               \
    /*
        Adjust direction increment based on long_len sign
    */                                                                                  \
    if (long_len < 0) {                                                                 \
        long_len = -long_len;                                                           \
        sign = -1;                                                                      \
    }                                                                                   \
    /*
        Calculate fixed-point increment for shorter length
    */                                                                                  \
    int dec = (long_len == 0) ? 0 : (short_len << 16) / long_len;                       \
    /*
        Travel the line pixel by pixel
    */                                                                                  \
    if (y_longer) {                                                                     \
        /*
            If line is more vertical, iterate over y-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + (j >> 16), y = y1 + i;                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            if (test(rn->zb.buffer[offset], z)) {                                       \
                rn->zb.buffer[offset] = z;                                              \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
    } else {                                                                            \
        /*
            If line is more horizontal, iterate over x-axis
        */                                                                              \
        for (int i = 0, j = 0; i != end; i += sign, j += dec) {                         \
            int x = x1 + i, y = y1 + (j >> 16);                                         \
            size_t offset = y * rn->fb.w + x;                                           \
            float t = (float)i * inv_end;                                               \
            float z = z1 + t * (z2 - z1);                                               \
            if (test(rn->zb.buffer[offset], z)) {                                       \
                rn->zb.buffer[offset] = z;                                              \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
    }

#define PF_LINE_THICK_TRAVEL(LINE_CODE)                                                 \
    /*
        Calculate differences in x and y coordinates
    */                                                                                  \
    int dx = x2 - x1;                                                                   \
    int dy = y2 - y1;                                                                   \
    /*
        Draw the main line between (x1, y1) and (x2, y2)
    */                                                                                  \
    LINE_CODE                                                                           \
    /*
        Determine if the line is more horizontal or vertical
    */                                                                                  \
    if (dx != 0 && abs(dy/dx) < 1) {                                                    \
        int y1_copy = y1;                                                               \
        int y2_copy = y2;                                                               \
        /*
            Line is more horizontal
            Calculate half the width of the line
        */                                                                              \
        int wy = (thick - 1) * sqrtf((float)(dx*dx + dy*dy)) / (2*abs(dx));             \
        /*
            Draw additional lines above and below the main line
        */                                                                              \
        for (int i = 1; i <= wy; ++i) {                                                 \
            y1 = y1_copy - i;                                                           \
            y2 = y2_copy - i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
            y1 = y1_copy + i;                                                           \
            y2 = y2_copy + i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
        }                                                                               \
    } else if (dy != 0) {                                                               \
        int x1_copy = x1;                                                               \
        int x2_copy = x2;                                                               \
        /*
            Line is more vertical or perfectly horizontal
            Calculate half the width of the line
        */                                                                              \
        int wx = (thick - 1) * sqrtf((float)(dx*dx + dy*dy)) / (2*abs(dy));             \
        /*
            Draw additional lines to the left and right of the main line
        */                                                                              \
        for (int i = 1; i <= wx; ++i) {                                                 \
            x1 = x1_copy - i;                                                           \
            x2 = x2_copy - i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
            x1 = x1_copy + i;                                                           \
            x2 = x2_copy + i;                                                           \
            {                                                                           \
                LINE_CODE                                                               \
            }                                                                           \
        }                                                                               \
    }

/* Internal Pixel Code Macros */

#define PF_PIXEL_CODE_NOBLEND()                                         \
    pf_vertex_t vertex;                                                 \
    pf_vertex_lerp(&vertex, &vertices[0], &vertices[1], t);             \
    pf_color_t *ptr = rn->fb.buffer + offset;                           \
    pf_color_t final_color = *ptr;                                      \
    proc->fragment(rn, &vertex, &final_color, proc->uniforms);          \
    *ptr = final_color;

#define PF_PIXEL_CODE_BLEND()                                           \
    pf_vertex_t vertex;                                                 \
    pf_vertex_lerp(&vertex, &vertices[0], &vertices[1], t);             \
    pf_color_t *ptr = rn->fb.buffer + offset;                           \
    pf_color_t final_color = *ptr;                                      \
    proc->fragment(rn, &vertex, &final_color, proc->uniforms);          \
    *ptr = blend(*ptr, final_color);

/* Helper Function Declarations */

void
pf_renderer_screen_projection_INTERNAL(
    const pf_renderer_t* rn,
    pf_vec4_t* homogeneous,
    pf_vertex_t* vertices,
    size_t vertices_count,
    int screen_pos[][2]);

/* Internal Clipping Function */

static uint8_t
pf_clip3_coord_line_INTERNAL(float q, float p, float* t1, float* t2)
{
    if (fabsf(p) < PF_EPSILON) {
        // Check if the line is entirely outside the window
        if (q < -PF_EPSILON) return 0;  // Completely outside
        return 1;                       // Completely inside or on the edges
    }

    const float r = q / p;

    if (p < 0)  {
        if (r > *t2) return 0;
        if (r > *t1) *t1 = r;
    } else {
        if (r < *t1) return 0;
        if (r < *t2) *t2 = r;
    }

    return 1;
}

void
pf_clip3d_line_INTERNAL(
    const struct pf_renderer* rn,
    pf_vertex_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count)
{
    (void)rn;
    (void)out_vertices;

    float t1 = 0, t2 = 1;

    pf_vec4_t delta;
    pf_vec4_sub_r(delta, out_homogeneous[1], out_homogeneous[0]);

    if (!pf_clip3_coord_line_INTERNAL(out_homogeneous[0][3] - out_homogeneous[0][0], -delta[3] + delta[0], &t1, &t2) ||
        !pf_clip3_coord_line_INTERNAL(out_homogeneous[0][3] + out_homogeneous[0][0], -delta[3] - delta[0], &t1, &t2) ||
        !pf_clip3_coord_line_INTERNAL(out_homogeneous[0][3] - out_homogeneous[0][1], -delta[3] + delta[1], &t1, &t2) ||
        !pf_clip3_coord_line_INTERNAL(out_homogeneous[0][3] + out_homogeneous[0][1], -delta[3] - delta[1], &t1, &t2) ||
        !pf_clip3_coord_line_INTERNAL(out_homogeneous[0][3] - out_homogeneous[0][2], -delta[3] + delta[2], &t1, &t2) ||
        !pf_clip3_coord_line_INTERNAL(out_homogeneous[0][3] + out_homogeneous[0][2], -delta[3] - delta[2], &t1, &t2))
    {
        *out_vertices_count = 0;
        return;
    }

    if (t2 < 1)
    {
        pf_vec4_t d;
        pf_vec4_scale_r(d, delta, t2);
        pf_vec4_add_r(out_homogeneous[1], out_homogeneous[0], d);
    }

    if (t1 > 0)
    {
        pf_vec4_t d;
        pf_vec4_scale_r(d, delta, t1);
        pf_vec4_add(out_homogeneous[0], out_homogeneous[0], d);
    }
}

/* Internal Rendering Functions */

void
pf_renderer_line3d_INTERNAL(
    pf_renderer_t* rn, const pf_vertex_t* v1, const pf_vertex_t* v2, float thick,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc)
{
    pf_vertex_t vertices[2] = { *v1, *v2 };
    pf_vec4_t homogens[2] = { 0 };
    int screen_pos[2][2] = { 0 };
    size_t num = 2;

    proc->vertex(&vertices[0], homogens[0], mat_model, mat_normal, mat_mvp, proc->uniforms);
    proc->vertex(&vertices[1], homogens[1], mat_model, mat_normal, mat_mvp, proc->uniforms);

    pf_clip3d_line_INTERNAL(rn, vertices, homogens, &num);
    if (num != 2) return;

    pf_renderer_screen_projection_INTERNAL(
        rn, homogens, vertices, num, screen_pos);

    int x1 = screen_pos[0][0];
    int y1 = screen_pos[0][1];
    int x2 = screen_pos[1][0];
    int y2 = screen_pos[1][1];
    float z1 = homogens[0][2];
    float z2 = homogens[1][2];

    pf_color_blend_fn blend = rn->conf3d->color_blend;
    pf_depth_test_fn test = rn->conf3d->depth_test;

    if (fabsf(thick) > 1) {
        if (test != NULL) {
            if (blend != NULL) {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_DEPTH({
                        PF_PIXEL_CODE_BLEND()
                    })
                })
            } else {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_DEPTH({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                })
            }
        } else {
            if (blend != NULL) {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_NODEPTH({
                        PF_PIXEL_CODE_BLEND()
                    })
                })
            } else {
                PF_LINE_THICK_TRAVEL({
                    PF_LINE_TRAVEL_NODEPTH({
                        PF_PIXEL_CODE_NOBLEND()
                    })
                })
            }
        }
    } else {
        if (test != NULL) {
            if (blend != NULL) {
                PF_LINE_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_LINE_TRAVEL_DEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        } else {
            if (blend != NULL) {
                PF_LINE_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_BLEND()
                })
            } else {
                PF_LINE_TRAVEL_NODEPTH({
                    PF_PIXEL_CODE_NOBLEND()
                })
            }
        }
    }
}
