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

// TODO: Improve the management of “varyings” by making it automatic

#ifndef PF_PROCESSORS_H
#define PF_PROCESSORS_H

#include "pf_vertex.h"

struct pf_renderer;
struct pf_renderer;

/* Processor 2D Prototypes */

typedef void (*pf_proc2d_vertex_fn)(
    pf_vertex_t* out_vertex,
    const pf_mat3_t transform,
    const void* uniforms);

typedef void (*pf_proc2d_fragment_fn)(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms);

/* Processor 3D Prototypes */

typedef void (*pf_proc3d_vertex_fn)(
    pf_vertex_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* uniforms);

typedef void (*pf_proc3d_fragment_fn)(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms);

/* 2D Processors Structs */

typedef struct {
    pf_proc2d_vertex_fn             vertex;
    pf_proc2d_fragment_fn           fragment;
    const void*                     uniforms;
} pf_proc2d_t;

/* 3D Processors Structs */

typedef struct {
    pf_proc3d_vertex_fn             vertex;
    pf_proc3d_fragment_fn           fragment;
    const void*                     uniforms;
} pf_proc3d_t;

/* Default Processor 2D Functions */

PFAPI void
pf_proc2d_vertex_default(
    pf_vertex_t* out_vertex,
    const pf_mat3_t transform,
    const void* uniforms);

PFAPI void
pf_proc2d_fragment_default(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms);

void
pf_proc2d_fragment_texture_as_uniform(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms);

/* Default Processor 3D Functions */

PFAPI void
pf_proc3d_vertex_default(
    pf_vertex_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* uniforms);

PFAPI void
pf_proc3d_vertex_normal_transform(
    pf_vertex_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* uniforms);

PFAPI void
pf_proc3d_fragment_default(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms);

#endif //PF_PROCESSORS_H
