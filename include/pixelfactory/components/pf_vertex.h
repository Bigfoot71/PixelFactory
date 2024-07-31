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

#ifndef PF_VERTEX_H
#define PF_VERTEX_H

#include "../math/pf_vec2.h"
#include "../math/pf_vec4.h"
#include "pf_attribute.h"
#include "pf_color.h"

/* Vertex Types */

typedef struct {
    pf_attrib_elem_t elements[PF_MAX_ATTRIBUTES];
} pf_vertex_t;

/* Helper Vertex Functions */

PFAPI pf_vertex_t
pf_vertex_create_2d(
    float x, float y,
    float u, float v,
    pf_color_t color);

PFAPI pf_vertex_t
pf_vertex_create_3d(
    float x, float y, float z,
    float u, float v,
    pf_color_t color);

static inline void
pf_vertex_get_vec(
    const pf_vertex_t* v, int index,
    void* out)
{
    pf_attrib_elem_get_vec(&v->elements[index], out);
}

static inline void
pf_vertex_set_vec(
    pf_vertex_t* v, int index,
    const void* in)
{
    pf_attrib_elem_set_vec(&v->elements[index], in);
}

PFAPI void
pf_vertex_scale_vec(
    pf_vertex_t* v, int index,
    float scale);

PFAPI void
pf_vertex_transform_vec_mat3(
    pf_vertex_t* v, int element_index,
    const pf_mat3_t transform);

PFAPI void
pf_vertex_transform_vec_mat4(
    pf_vertex_t* v, int element_index,
    const pf_mat4_t transform);

PFAPI void
pf_vertex_lerp(
    pf_vertex_t* restrict result,
    const pf_vertex_t* restrict start,
    const pf_vertex_t* restrict end,
    float t);

PFAPI void
pf_vertex_bary(
    pf_vertex_t* restrict result,
    const pf_vertex_t* restrict v1,
    const pf_vertex_t* restrict v2,
    const pf_vertex_t* restrict v3,
    const pf_vec3_t bary);

#endif //PF_VERTEX_H
