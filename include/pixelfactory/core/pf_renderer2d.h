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

#ifndef PF_RENDERER2D_H
#define PF_RENDERER2D_H

#include "../components/pf_processors.h"
#include "../components/pf_vertex.h"
#include "../math/pf_mat3.h"
#include "pf_framebuffer.h"
#include "pf_texture2d.h"

typedef struct pf_renderer2d {
    pf_mat3_t mat_view;
    pf_framebuffer_t fb;
    pf_color_blend_fn blend;
} pf_renderer2d_t;


/* Renderer Mangement Functions */

PFAPI pf_renderer2d_t
pf_renderer2d_create(
    uint32_t w, uint32_t h,
    pf_color_blend_fn blend);

PFAPI void
pf_renderer2d_delete(
    pf_renderer2d_t* rn);

PFAPI void
pf_renderer2d_clear(
    pf_renderer2d_t* rn,
    pf_color_t clear_color);


/* Renderer 2D Buffer Drawing */

PFAPI void
pf_renderer2d_vertex_buffer(
    pf_renderer2d_t* rn,
    const pf_vertex_buffer_t* vb,
    const pf_mat3_t transform,
    pf_proc2d_t* proc);

/* Renderer 2D Shapes Drawing Functions */

PFAPI void
pf_renderer2d_line(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color);

PFAPI void
pf_renderer2d_line_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t c1,
    pf_color_t c2);

PFAPI void
pf_renderer2d_line_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_line_thick(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t color);

PFAPI void
pf_renderer2d_line_thick_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t c1,
    pf_color_t c2);

PFAPI void
pf_renderer2d_line_thick_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_rect(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color);

PFAPI void
pf_renderer2d_rect_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl);

PFAPI void
pf_renderer2d_rect_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_rect_lines(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color);

PFAPI void
pf_renderer2d_rect_lines_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl);

PFAPI void
pf_renderer2d_rect_lines_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_rect_lines_thick(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t color);

PFAPI void
pf_renderer2d_rect_lines_thick_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick, pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl);

PFAPI void
pf_renderer2d_rect_lines_thick_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_point(
    pf_renderer2d_t* rn, int x, int y,
    pf_color_t color);

PFAPI void
pf_renderer2d_circle(
    pf_renderer2d_t* rn, int cx, int cy,
    int radius, pf_color_t color);

PFAPI void
pf_renderer2d_circle_gradient(
    pf_renderer2d_t* rn, int cx, int cy, int radius,
    pf_color_t c1, pf_color_t c2);

PFAPI void
pf_renderer2d_circle_map(
    pf_renderer2d_t* rn, int cx, int cy, int radius,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_circle_lines(
    pf_renderer2d_t* rn, int cx, int cy,
    int radius, pf_color_t color);

PFAPI void
pf_renderer2d_circle_lines_map(
    pf_renderer2d_t* rn, int cx, int cy, int radius,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_circle_lines_thick(
    pf_renderer2d_t* rn, int cx, int cy,
    int radius, int thick, pf_color_t color);

PFAPI void
pf_renderer2d_circle_lines_thick_map(
    pf_renderer2d_t* rn, int cx, int cy, int radius, int thick,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_triangle(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color);

PFAPI void
pf_renderer2d_triangle_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3);

PFAPI void
pf_renderer2d_triangle_map(
    pf_renderer2d_t *rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_triangle_lines(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color);

PFAPI void
pf_renderer2d_triangle_lines_gradient(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3);

PFAPI void
pf_renderer2d_triangle_lines_map(
    pf_renderer2d_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_triangle_fan(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color);

PFAPI void
pf_renderer2d_triangle_fan_map(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_triangle_fan_lines(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color);

PFAPI void
pf_renderer2d_triangle_strip(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color);

PFAPI void
pf_renderer2d_triangle_strip_map(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_proc2d_t* proc);

PFAPI void
pf_renderer2d_triangle_strip_lines(
    pf_renderer2d_t* rn,
    int* points, int count,
    pf_color_t color);


/* Renderer 2D Texture Drawing Functions */

PFAPI void
pf_renderer2d_texture2d(
    pf_renderer2d_t* rn,
    const pf_texture2d_t* tex,
    int x, int y);

PFAPI void
pf_renderer2d_texture2d_tint(
    pf_renderer2d_t* rn,
    const pf_texture2d_t* tex,
    int x, int y, pf_color_t tint);

PFAPI void
pf_renderer2d_texture2d_ex(
    pf_renderer2d_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy);

PFAPI void
pf_renderer2d_texture2d_ex_tint(
    pf_renderer2d_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy,
    pf_color_t tint);

PFAPI void
pf_renderer2d_texture2d_ex_map(
    pf_renderer2d_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy,
    pf_proc2d_fragment_fn frag_proc);

PFAPI void
pf_renderer2d_texture2d_rec(
    pf_renderer2d_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy);

PFAPI void
pf_renderer2d_texture2d_rec_tint(
    pf_renderer2d_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy,
    pf_color_t tint);

PFAPI void
pf_renderer2d_texture2d_rec_map(
    pf_renderer2d_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy,
    pf_proc2d_fragment_fn frag_proc);

PFAPI void
pf_renderer2d_texture2d_mat(
    pf_renderer2d_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform);

PFAPI void
pf_renderer2d_texture2d_mat_tint(
    pf_renderer2d_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform,
    pf_color_t tint);

PFAPI void
pf_renderer2d_texture2d_mat_map(
    pf_renderer2d_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform,
    pf_proc2d_fragment_fn frag_proc);

#endif //PF_RENDERER2D_H
