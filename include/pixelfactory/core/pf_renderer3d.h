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

#ifndef PF_RENDER3D_H
#define PF_RENDER3D_H

#include "../components/pf_processors.h"
#include "../core/pf_vertexbuffer.h"
#include "../math/pf_mat3.h"
#include "pf_framebuffer.h"
#include "pf_depthbuffer.h"
#include "pf_texture2d.h"
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

typedef void(*pf_renderer3d_map_fn)(
    struct pf_renderer3d* rn,
    pf_color_t* out_color,
    float* out_depth,
    int x, int y,
    float u,
    float v);

typedef struct pf_renderer3d {
    pf_mat4_t           mat_view;
    pf_mat4_t           mat_proj;
    pf_framebuffer_t    fb;
    pf_depthbuffer_t    zb;
    pf_color_blend_fn   blend;
    pf_depth_test_fn    test;
    int                 viewport_position[2];
    int                 viewport_dimension[2];
    pf_cullmode_e       cull_mode;
} pf_renderer3d_t;


/* Renderer Mangement Functions */

PFAPI pf_renderer3d_t
pf_renderer3d_create(
    uint32_t w, uint32_t h,
    pf_color_blend_fn blend,
    pf_depth_test_fn test);

PFAPI void
pf_renderer3d_delete(
    pf_renderer3d_t* rn);

PFAPI void
pf_renderer3d_clear(
    pf_renderer3d_t* rn,
    pf_color_t clear_color,
    float clear_depth);

PFAPI void
pf_renderer3d_map(
    pf_renderer3d_t* rn,
    pf_renderer3d_map_fn func);

PFAPI void
pf_renderer3d_viewport(
    pf_renderer3d_t* rn,
    int x, int y,
    int w, int h);


/* Renderer 2D Buffer Drawing */

PFAPI void
pf_renderer3d_vertexbuffer(
    pf_renderer3d_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_vertexbuffer_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_vertexbuffer_points(
    pf_renderer3d_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_vertexbuffer_points_thick(
    pf_renderer3d_t* rn, const pf_vertexbuffer_t* vb, float radius,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_vertexbuffer_lines(
    pf_renderer3d_t* rn, const pf_vertexbuffer_t* vb,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_vertexbuffer_lines_thick(
    pf_renderer3d_t* rn, const pf_vertexbuffer_t* vb, float thick,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

/* Renderer 3D Points */

PFAPI void
pf_renderer3d_point(
    pf_renderer3d_t* rn, const pf_vertex_t* point,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_point_thick(
    pf_renderer3d_t* rn, const pf_vertex_t* point, float radius,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

/* Renderer 3D Lines */

PFAPI void
pf_renderer3d_line(
    pf_renderer3d_t* rn, const pf_vertex_t* v1, const pf_vertex_t* v2,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

PFAPI void
pf_renderer3d_line_thick(
    pf_renderer3d_t* rn, const pf_vertex_t* v1, const pf_vertex_t* v2, float thick,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

/* Renderer 3D Triangles */

PFAPI void
pf_renderer3d_triangle(
    pf_renderer3d_t* rn, const pf_vertex_t* v1, const pf_vertex_t* v2, const pf_vertex_t* v3,
    const pf_mat4_t transform, const pf_proc3d_t* proc);

#endif //PF_RENDERER3D_H
