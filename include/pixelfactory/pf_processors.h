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

#ifndef PF_PROCESSORS_H
#define PF_PROCESSORS_H

#include "pf_vertex.h"
#include "pixelfactory/math/pf_vec4.h"

struct pf_renderer2d;
struct pf_renderer3d;

/* Processor 2D Prototypes */

typedef void (*pf_proc2d_vertex_fn)(
    pf_vertex2d_t* out_vertex,
    const pf_mat3_t transform,
    const void* attr);

typedef void (*pf_proc2d_rasterizer_fn)(
    pf_vertex2d_t* out_vertex,
    pf_vertex2d_t* vertex_1,
    pf_vertex2d_t* vertex_2,
    pf_vertex2d_t* vertex_3,
    pf_vec3_t bary,
    void* attr);

typedef void (*pf_proc2d_fragment_fn)(
    struct pf_renderer2d* rn,
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* attr);

/* Processor 3D Prototypes */

typedef void (*pf_proc3d_vertex_fn)(
    pf_vertex3d_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* attr);

typedef void (*pf_proc3d_clip_fn)(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count);

typedef void (*pf_proc3d_screen_projection_fn)(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* vertices,
    pf_vec4_t homogeneous[],
    size_t vertices_count,
    int screen_pos[][2]);

typedef void (*pf_proc3d_rasterizer_fn)(
    pf_vertex3d_t* out_vertex,
    pf_vertex3d_t* v1,
    pf_vertex3d_t* v2,
    pf_vertex3d_t* v3,
    pf_vec3_t bary,
    float z_depth,
    void* attr);

typedef void (*pf_proc3d_fragment_fn)(
    struct pf_renderer3d* rn,
    pf_vertex3d_t* vertex,
    pf_color_t* out_color,
    const void* attr);

/* Default Processor 2D Functions */

PFAPI void
pf_proc2d_vertex_default(
    pf_vertex2d_t* out_vertex,
    const pf_mat3_t transform,
    const void* attr);

PFAPI void
pf_proc2d_rasterizer_default(
    pf_vertex2d_t* out_vertex,
    pf_vertex2d_t* v1,
    pf_vertex2d_t* v2,
    pf_vertex2d_t* v3,
    pf_vec3_t bary,
    void* attr);

PFAPI void
pf_proc2d_fragment_default(
    struct pf_renderer2d* rn,
    pf_vertex2d_t* vertex,
    pf_color_t* out_color,
    const void* attr);

/* Default Processor 3D Functions */

PFAPI void
pf_proc3d_vertex_default(
    pf_vertex3d_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* attr
);

PFAPI void
pf_proc3d_clip_point(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count);

PFAPI void
pf_proc3d_clip_line(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count);

PFAPI void
pf_proc3d_clip_triangle(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* out_vertices,
    pf_vec4_t out_homogeneous[],
    size_t* out_vertices_count);

PFAPI void
pf_proc3d_screen_projection_default(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* vertices,
    pf_vec4_t homogeneous[],
    size_t vertices_count,
    int screen_pos[][2]);

PFAPI void
pf_proc3d_screen_projection_perspective_correct(
    const struct pf_renderer3d* rn,
    pf_vertex3d_t* vertices,
    pf_vec4_t homogeneous[],
    size_t vertices_count,
    int screen_pos[][2]);

PFAPI void
pf_proc3d_rasterizer_default(
    pf_vertex3d_t* out_vertex,
    pf_vertex3d_t* v1,
    pf_vertex3d_t* v2,
    pf_vertex3d_t* v3,
    pf_vec3_t bary,
    float z_depth,
    void* attr);

PFAPI void
pf_proc3d_rasterizer_perspective_correct(
    pf_vertex3d_t* out_vertex,
    pf_vertex3d_t* v1,
    pf_vertex3d_t* v2,
    pf_vertex3d_t* v3,
    pf_vec3_t bary,
    float z_depth,
    void* attr);

PFAPI void
pf_proc3d_fragment_default(
    struct pf_renderer3d* rn,
    pf_vertex3d_t* vertex,
    pf_color_t* out_color,
    const void* attr);

#endif //PF_PROCESSORS_H
