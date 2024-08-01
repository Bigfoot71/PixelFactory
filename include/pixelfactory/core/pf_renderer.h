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

#ifndef PF_RENDER_H
#define PF_RENDER_H

#include "../components/pf_processors.h"
#include "../core/pf_vertexbuffer.h"
#include "../core/pf_texture2d.h"
#include "../utils/pf_camera2d.h"
#include "../utils/pf_camera3d.h"
#include "pf_framebuffer.h"
#include "pf_depthbuffer.h"
#include <stdint.h>

typedef enum {
    PF_BACK,
    PF_FRONT
} pf_face_e;

typedef enum {
    PF_CULL_NONE,
    PF_CULL_BACK,
    PF_CULL_FRONT
} pf_cullmode_e;

typedef void(*pf_renderer_map2d_fn)(
    struct pf_renderer* rn,
    pf_color_t* out_color,
    int x, int y,
    float u,
    float v);

typedef void(*pf_renderer_map3d_fn)(
    struct pf_renderer* rn,
    pf_color_t* out_color,
    float* out_depth,
    int x, int y,
    float u,
    float v);

typedef enum {
    PF_RENDERER_2D = 0x01,
    PF_RENDERER_3D = 0x02
} pf_renderer_flag_e;

typedef struct {
    pf_mat3_t           mat_view;
    pf_color_blend_fn   color_blend;
} pf_renderer_config_2d_t;

typedef struct {
    pf_mat4_t           mat_view;
    pf_mat4_t           mat_proj;
    int                 viewport_pos[2];
    int                 viewport_dim[2];
    pf_color_blend_fn   color_blend;
    pf_depth_test_fn    depth_test;
    pf_cullmode_e       cull_mode;
} pf_renderer_config_3d_t;

typedef struct pf_renderer {
    pf_framebuffer_t fb;
    pf_depthbuffer_t zb;
    pf_renderer_config_3d_t* conf3d;
    pf_renderer_config_2d_t* conf2d;
} pf_renderer_t;


/* Renderer Mangement Functions */

PFAPI pf_renderer_t
pf_renderer_load(
    uint32_t w, uint32_t h,
    pf_renderer_flag_e flags);

PFAPI void
pf_renderer_delete(
    pf_renderer_t* rn);

PFAPI bool
pf_renderer_is_valid(
    pf_renderer_t* rn,
    pf_renderer_flag_e flags);

PFAPI void
pf_renderer_clear2d(
    pf_renderer_t* rn,
    pf_color_t clear_color);

PFAPI void
pf_renderer_clear3d(
    pf_renderer_t* rn,
    pf_color_t clear_color,
    float clear_depth);

PFAPI void
pf_renderer_map3d(
    pf_renderer_t* rn,
    pf_renderer_map3d_fn func);

PFAPI void
pf_renderer_map2d(
    pf_renderer_t* rn,
    pf_renderer_map2d_fn func);

PFAPI void
pf_renderer_viewport(
    pf_renderer_t* rn,
    int x, int y, int w, int h);

PFAPI void
pf_renderer_apply_camera2d(
    pf_renderer_t* rn,
    pf_camera2d_t* cam);

PFAPI void
pf_renderer_apply_camera3d(
    pf_renderer_t* rn,
    pf_camera3d_t* cam);

PFAPI void
pf_renderer_apply_camera3d_orthographic(
    pf_renderer_t* rn,
    pf_camera3d_t* cam);

PFAPI void
pf_renderer_apply_camera3d_perspective(
    pf_renderer_t* rn,
    pf_camera3d_t* cam);

/* Renderer 2D Buffer Drawing */

PFAPI void
pf_renderer_vertexbuffer2d(
    pf_renderer_t* rn,
    const pf_vertexbuffer_t* vb,
    const pf_mat3_t transform,
    const pf_proc2d_t* proc);

/* Renderer 2D Shapes Drawing Functions */

PFAPI void
pf_renderer_line2d(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color);

PFAPI void
pf_renderer_line2d_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t c1,
    pf_color_t c2);

PFAPI void
pf_renderer_line2d_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_line2d_thick(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t color);

PFAPI void
pf_renderer_line2d_thick_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t c1,
    pf_color_t c2);

PFAPI void
pf_renderer_line2d_thick_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_rect2d(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color);

PFAPI void
pf_renderer_rect2d_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl);

PFAPI void
pf_renderer_rect2d_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_rect2d_lines(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t color);

PFAPI void
pf_renderer_rect2d_lines_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl);

PFAPI void
pf_renderer_rect2d_lines_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_rect_lines_thick(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    pf_color_t color);

PFAPI void
pf_renderer_rect2d_lines_thick_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick, pf_color_t col_tl,
    pf_color_t col_tr,
    pf_color_t col_br,
    pf_color_t col_bl);

PFAPI void
pf_renderer_rect2d_lines_thick_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int thick,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_point2d(
    pf_renderer_t* rn, int x, int y,
    pf_color_t color);

PFAPI void
pf_renderer_circle2d(
    pf_renderer_t* rn, int cx, int cy,
    int radius, pf_color_t color);

PFAPI void
pf_renderer_circle2d_gradient(
    pf_renderer_t* rn, int cx, int cy, int radius,
    pf_color_t c1, pf_color_t c2);

PFAPI void
pf_renderer_circle2d_map(
    pf_renderer_t* rn, int cx, int cy, int radius,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_circle2d_lines(
    pf_renderer_t* rn, int cx, int cy,
    int radius, pf_color_t color);

PFAPI void
pf_renderer_circle2d_lines_map(
    pf_renderer_t* rn, int cx, int cy, int radius,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_circle2d_lines_thick(
    pf_renderer_t* rn, int cx, int cy,
    int radius, int thick, pf_color_t color);

PFAPI void
pf_renderer_circle2d_lines_thick_map(
    pf_renderer_t* rn, int cx, int cy, int radius, int thick,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_triangle2d(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color);

PFAPI void
pf_renderer_triangle2d_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3);

PFAPI void
pf_renderer_triangle2d_map(
    pf_renderer_t *rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_triangle2d_lines(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t color);

PFAPI void
pf_renderer_triangle2d_lines_gradient(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    pf_color_t c1,
    pf_color_t c2,
    pf_color_t c3);

PFAPI void
pf_renderer_triangle2d_lines_map(
    pf_renderer_t* rn,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_triangle2d_fan(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color);

PFAPI void
pf_renderer_triangle2d_fan_map(
    pf_renderer_t* rn,
    int* points, int count,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_triangle2d_fan_lines(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color);

PFAPI void
pf_renderer_triangle2d_strip(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color);

PFAPI void
pf_renderer_triangle2d_strip_map(
    pf_renderer_t* rn,
    int* points, int count,
    const pf_proc2d_t* proc);

PFAPI void
pf_renderer_triangle_strip_lines(
    pf_renderer_t* rn,
    int* points, int count,
    pf_color_t color);


/* Renderer 2D Texture Drawing Functions */

PFAPI void
pf_renderer_texture2d(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    int x, int y);

PFAPI void
pf_renderer_texture2d_tint(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    int x, int y, pf_color_t tint);

PFAPI void
pf_renderer_texture2d_ex(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy);

PFAPI void
pf_renderer_texture2d_ex_tint(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy,
    pf_color_t tint);

PFAPI void
pf_renderer_texture2d_ex_map(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    int x, int y, float sx, float sy,
    float r, int ox, int oy,
    pf_proc2d_fragment_fn frag_proc);

PFAPI void
pf_renderer_texture2d_rec(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy);

PFAPI void
pf_renderer_texture2d_rec_tint(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy,
    pf_color_t tint);

PFAPI void
pf_renderer_texture2d_rec_map(
    pf_renderer_t *rn, const pf_texture2d_t *tex,
    const int* src_rect, const int* dst_rect,
    float r, int ox, int oy,
    pf_proc2d_fragment_fn frag_proc);

PFAPI void
pf_renderer_texture2d_mat(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform);

PFAPI void
pf_renderer_texture2d_mat_tint(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform,
    pf_color_t tint);

PFAPI void
pf_renderer_texture2d_mat_map(
    pf_renderer_t* rn,
    const pf_texture2d_t* tex,
    pf_mat3_t transform,
    pf_proc2d_fragment_fn frag_proc);

/* Renderer 3D Buffer Drawing */

PFAPI void
pf_renderer_vertexbuffer3d(
    pf_renderer_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer_vertexbuffer3d_ex(
    pf_renderer_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer_vertexbuffer3d_points(
    pf_renderer_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer_vertexbuffer3d_points_thick(
    pf_renderer_t* rn, const pf_vertexbuffer_t* vb, float radius,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer_vertexbuffer3d_lines(
    pf_renderer_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer_vertexbuffer3d_lines_thick(
    pf_renderer_t* rn, const pf_vertexbuffer_t* vb, float thick,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

/* Renderer 3D Points */

PFAPI void
pf_renderer_point3d(
    pf_renderer_t* rn,
    const pf_vec3_t point,
    pf_color_t color);

PFAPI void
pf_renderer_point3d_thick(
    pf_renderer_t* rn,
    const pf_vec3_t point,
    float radius,
    pf_color_t color);

/* Renderer 3D Lines */

PFAPI void
pf_renderer_line3d(
    pf_renderer_t* rn,
    const pf_vec3_t p1,
    const pf_vec3_t p2,
    pf_color_t color);

PFAPI void
pf_renderer_line3d_thick(
    pf_renderer_t* rn,
    const pf_vec3_t p1,
    const pf_vec3_t p2,
    float thickness,
    pf_color_t color);

/* Renderer 3D Triangles */

PFAPI void
pf_renderer_triangle3d(
    pf_renderer_t* rn,
    const pf_vec3_t p1,
    const pf_vec3_t p2,
    const pf_vec3_t p3,
    pf_color_t color);

#endif //PF_RENDERER3D_H
