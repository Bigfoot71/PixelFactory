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

#include "pixelfactory/core/pf_renderer2d.h"

/* Macros */

#define PF_RECT_TRAVEL(PIXEL_CODE)                                                      \
    _Pragma("omp parallel for")                                                         \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            size_t offset = y_offset + x;                                               \
            PIXEL_CODE                                                                  \
        }                                                                               \
    }

#define PF_RECT_TRANSFORM_TRAVEL(PIXEL_CODE)                                            \
    _Pragma("omp parallel for")                                                         \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            pf_vec2_t p = { x, y };                                                     \
            pf_vec2_t p_transformed;                                                    \
            pf_vec2_transform(p_transformed, p, mat_view_inv);                          \
            if (p_transformed[0] >= x1 && p_transformed[0] <= x2 &&                     \
                p_transformed[1] >= y1 && p_transformed[1] <= y2) {                     \
                size_t offset = y_offset + x;                                           \
                PIXEL_CODE                                                              \
            }                                                                           \
        }                                                                               \
    }

/* Public API */

void
pf_renderer2d_rect(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color)
{
    if (x1 > x2) PF_SWAP(x1, x2);
    if (y1 > y2) PF_SWAP(y1, y2);

    if (pf_mat3_is_identity(rn->mat_view) != 0) {
        int xmin = PF_CLAMP(x1, 0, (int)rn->fb.w - 1);
        int ymin = PF_CLAMP(y1, 0, (int)rn->fb.h - 1);
        int xmax = PF_CLAMP(x2, 0, (int)rn->fb.w - 1);
        int ymax = PF_CLAMP(y2, 0, (int)rn->fb.h - 1);
        if (rn->blend == NULL) {
            PF_RECT_TRAVEL({
                rn->fb.buffer[offset] = color;
            })
        } else {
            PF_RECT_TRAVEL({
                pf_color_t* ptr = rn->fb.buffer + offset;
                *ptr = rn->blend(*ptr, color);
            })
        }
    } else {
        pf_vec2_t p1 = { x1, y1 };
        pf_vec2_t p2 = { x2, y1 };
        pf_vec2_t p3 = { x2, y2 };
        pf_vec2_t p4 = { x1, y2 };

        // Transform rectangle corners
        pf_vec2_transform(p1, p1, rn->mat_view);
        pf_vec2_transform(p2, p2, rn->mat_view);
        pf_vec2_transform(p3, p3, rn->mat_view);
        pf_vec2_transform(p4, p4, rn->mat_view);

        // Determine bounding box boundaries
        int xmin = PF_CLAMP(PF_MIN(PF_MIN(p1[0], p2[0]), PF_MIN(p3[0], p4[0])), 0, (int)rn->fb.w - 1);
        int ymin = PF_CLAMP(PF_MIN(PF_MIN(p1[1], p2[1]), PF_MIN(p3[1], p4[1])), 0, (int)rn->fb.h - 1);
        int xmax = PF_CLAMP(PF_MAX(PF_MAX(p1[0], p2[0]), PF_MAX(p3[0], p4[0])), 0, (int)rn->fb.w - 1);
        int ymax = PF_CLAMP(PF_MAX(PF_MAX(p1[1], p2[1]), PF_MAX(p3[1], p4[1])), 0, (int)rn->fb.h - 1);

        // Invert View Matrix
        pf_mat3_t mat_view_inv;
        pf_mat3_inverse(mat_view_inv, rn->mat_view);

        // Iterate over each pixel in the bounding box and check if it is in the transformed rectangle
        if (rn->blend == NULL) {
            PF_RECT_TRANSFORM_TRAVEL({
                rn->fb.buffer[offset] = color;
            })
        } else {
            PF_RECT_TRANSFORM_TRAVEL({
                pf_color_t* ptr = rn->fb.buffer + offset;
                *ptr = rn->blend(*ptr, color);
            })
        }
    }
}

void
pf_renderer2d_rect_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl)
{
    if (x1 > x2) PF_SWAP(x1, x2);
    if (y1 > y2) PF_SWAP(y1, y2);

    int w = abs(x2 - x1);
    int h = abs(y2 - y1);

    if (pf_mat3_is_identity(rn->mat_view) != 0) {
        int xmin = PF_CLAMP(x1, 0, (int)rn->fb.w - 1);
        int ymin = PF_CLAMP(y1, 0, (int)rn->fb.h - 1);
        int xmax = PF_CLAMP(x2, 0, (int)rn->fb.w - 1);
        int ymax = PF_CLAMP(y2, 0, (int)rn->fb.h - 1);
        if (rn->blend == NULL) {
            PF_RECT_TRAVEL({
                int ix = x - x1;
                int iy = y - y1;
                pf_color_t color_top = pf_color_lerpi(col_tl, col_tr, ix, w);
                pf_color_t color_bottom = pf_color_lerpi(col_bl, col_br, ix, w);
                rn->fb.buffer[offset] = pf_color_lerpi(color_top, color_bottom, iy, h);
            })
        } else {
            PF_RECT_TRAVEL({
                int ix = x - x1;
                int iy = y - y1;
                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t color_top = pf_color_lerpi(col_tl, col_tr, ix, w);
                pf_color_t color_bottom = pf_color_lerpi(col_bl, col_br, ix, w);
                *ptr = rn->blend(*ptr, pf_color_lerpi(color_top, color_bottom, iy, h));
            })
        }
    } else {
        pf_vec2_t p1 = { x1, y1 };
        pf_vec2_t p2 = { x2, y1 };
        pf_vec2_t p3 = { x2, y2 };
        pf_vec2_t p4 = { x1, y2 };

        // Transform rectangle corners
        pf_vec2_transform(p1, p1, rn->mat_view);
        pf_vec2_transform(p2, p2, rn->mat_view);
        pf_vec2_transform(p3, p3, rn->mat_view);
        pf_vec2_transform(p4, p4, rn->mat_view);

        // Determine bounding box boundaries
        int xmin = PF_CLAMP(PF_MIN(PF_MIN(p1[0], p2[0]), PF_MIN(p3[0], p4[0])), 0, (int)rn->fb.w - 1);
        int ymin = PF_CLAMP(PF_MIN(PF_MIN(p1[1], p2[1]), PF_MIN(p3[1], p4[1])), 0, (int)rn->fb.h - 1);
        int xmax = PF_CLAMP(PF_MAX(PF_MAX(p1[0], p2[0]), PF_MAX(p3[0], p4[0])), 0, (int)rn->fb.w - 1);
        int ymax = PF_CLAMP(PF_MAX(PF_MAX(p1[1], p2[1]), PF_MAX(p3[1], p4[1])), 0, (int)rn->fb.h - 1);

        // Invert View Matrix
        pf_mat3_t mat_view_inv;
        pf_mat3_inverse(mat_view_inv, rn->mat_view);

        // Iterate over each pixel in the bounding box and check if it is in the transformed rectangle
        if (rn->blend == NULL) {
            PF_RECT_TRANSFORM_TRAVEL({
                int ix = x - x1;
                int iy = y - y1;
                pf_color_t color_top = pf_color_lerpi(col_tl, col_tr, ix, w);
                pf_color_t color_bottom = pf_color_lerpi(col_bl, col_br, ix, w);
                rn->fb.buffer[offset] = pf_color_lerpi(color_top, color_bottom, iy, h);
            })
        } else {
            PF_RECT_TRANSFORM_TRAVEL({
                int ix = x - x1;
                int iy = y - y1;
                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t color_top = pf_color_lerpi(col_tl, col_tr, ix, w);
                pf_color_t color_bottom = pf_color_lerpi(col_bl, col_br, ix, w);
                *ptr = rn->blend(*ptr, pf_color_lerpi(color_top, color_bottom, iy, h));
            })
        }
    }
}

void
pf_renderer2d_rect_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    if (x1 > x2) PF_SWAP(x1, x2);
    if (y1 > y2) PF_SWAP(y1, y2);

    if (pf_mat3_is_identity(rn->mat_view) != 0) {
        int xmin = PF_CLAMP(x1, 0, (int)rn->fb.w - 1);
        int ymin = PF_CLAMP(y1, 0, (int)rn->fb.h - 1);
        int xmax = PF_CLAMP(x2, 0, (int)rn->fb.w - 1);
        int ymax = PF_CLAMP(y2, 0, (int)rn->fb.h - 1);
        if (rn->blend != NULL) {
            PF_RECT_TRAVEL({
                pf_vertex2d_t vertex;
                vertex.position[0] = x;
                vertex.position[1] = y;
                vertex.texcoord[0] = 0;
                vertex.texcoord[1] = 0;
                vertex.color = PF_WHITE;

                pf_color_t *ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_RECT_TRAVEL({
                pf_vertex2d_t vertex;
                vertex.position[0] = x;
                vertex.position[1] = y;
                vertex.texcoord[0] = 0;
                vertex.texcoord[1] = 0;
                vertex.color = PF_WHITE;

                pf_color_t *ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = final_color;
            })
        }
    } else {
        pf_vec2_t p1 = { x1, y1 };
        pf_vec2_t p2 = { x2, y1 };
        pf_vec2_t p3 = { x2, y2 };
        pf_vec2_t p4 = { x1, y2 };

        // Transform rectangle corners
        pf_vec2_transform(p1, p1, rn->mat_view);
        pf_vec2_transform(p2, p2, rn->mat_view);
        pf_vec2_transform(p3, p3, rn->mat_view);
        pf_vec2_transform(p4, p4, rn->mat_view);

        // Determine bounding box boundaries
        int xmin = PF_CLAMP(PF_MIN(PF_MIN(p1[0], p2[0]), PF_MIN(p3[0], p4[0])), 0, (int)rn->fb.w - 1);
        int ymin = PF_CLAMP(PF_MIN(PF_MIN(p1[1], p2[1]), PF_MIN(p3[1], p4[1])), 0, (int)rn->fb.h - 1);
        int xmax = PF_CLAMP(PF_MAX(PF_MAX(p1[0], p2[0]), PF_MAX(p3[0], p4[0])), 0, (int)rn->fb.w - 1);
        int ymax = PF_CLAMP(PF_MAX(PF_MAX(p1[1], p2[1]), PF_MAX(p3[1], p4[1])), 0, (int)rn->fb.h - 1);

        // Invert View Matrix
        pf_mat3_t mat_view_inv;
        pf_mat3_inverse(mat_view_inv, rn->mat_view);

        // Iterate over each pixel in the bounding box and check if it is in the transformed rectangle
        if (rn->blend != NULL) {
            PF_RECT_TRAVEL({
                pf_vertex2d_t vertex;
                vertex.position[0] = x;
                vertex.position[1] = y;
                vertex.texcoord[0] = 0;
                vertex.texcoord[1] = 0;
                vertex.color = PF_WHITE;

                pf_color_t *ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_RECT_TRAVEL({
                pf_vertex2d_t vertex;
                vertex.position[0] = x;
                vertex.position[1] = y;
                vertex.texcoord[0] = 0;
                vertex.texcoord[1] = 0;
                vertex.color = PF_WHITE;

                pf_color_t *ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = final_color;
            })
        }
    }
}

void
pf_renderer2d_rect_lines(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color)
{
    pf_renderer2d_line(rn, x1, y1, x2, y1, color);
    pf_renderer2d_line(rn, x2, y1, x2, y2, color);
    pf_renderer2d_line(rn, x2, y2, x1, y2, color);
    pf_renderer2d_line(rn, x1, y2, x1, y1, color);
}

void
pf_renderer2d_rect_lines_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl)
{
    pf_renderer2d_line_gradient(rn, x1, y1, x2, y1, col_tl, col_tr);
    pf_renderer2d_line_gradient(rn, x2, y1, x2, y2, col_tr, col_br);
    pf_renderer2d_line_gradient(rn, x2, y2, x1, y2, col_br, col_bl);
    pf_renderer2d_line_gradient(rn, x1, y2, x1, y1, col_bl, col_tl);
}

void
pf_renderer2d_rect_lines_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    pf_renderer2d_line_map(rn, x1, y1, x2, y1, frag_proc, attr);
    pf_renderer2d_line_map(rn, x2, y1, x2, y2, frag_proc, attr);
    pf_renderer2d_line_map(rn, x2, y2, x1, y2, frag_proc, attr);
    pf_renderer2d_line_map(rn, x1, y2, x1, y1, frag_proc, attr);
}

void
pf_renderer2d_rect_lines_thick(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t color)
{
    pf_renderer2d_line_thick(rn, x1, y1, x2, y1, thick, color);
    pf_renderer2d_line_thick(rn, x2, y1, x2, y2, thick, color);
    pf_renderer2d_line_thick(rn, x2, y2, x1, y2, thick, color);
    pf_renderer2d_line_thick(rn, x1, y2, x1, y1, thick, color);
}

void
pf_renderer2d_rect_lines_thick_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick, pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl)
{
    pf_renderer2d_line_thick_gradient(rn, x1, y1, x2, y1, thick, col_tl, col_tr);
    pf_renderer2d_line_thick_gradient(rn, x2, y1, x2, y2, thick, col_tr, col_br);
    pf_renderer2d_line_thick_gradient(rn, x2, y2, x1, y2, thick, col_br, col_bl);
    pf_renderer2d_line_thick_gradient(rn, x1, y2, x1, y1, thick, col_bl, col_tl);
}

void
pf_renderer2d_rect_lines_thick_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_proc2d_fragment_fn frag_proc,
    const void* attr)
{
    pf_renderer2d_line_thick_map(rn, x1, y1, x2, y1, thick, frag_proc, attr);
    pf_renderer2d_line_thick_map(rn, x2, y1, x2, y2, thick, frag_proc, attr);
    pf_renderer2d_line_thick_map(rn, x2, y2, x1, y2, thick, frag_proc, attr);
    pf_renderer2d_line_thick_map(rn, x1, y2, x1, y1, thick, frag_proc, attr);
}