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

#include "pixelfactory/components/pf_processors.h"
#include "pixelfactory/core/pf_texture2d.h"
#include "pixelfactory/math/pf_vec2.h"

/* Default Processor 2D Functions */

void
pf_proc2d_vertex_default(
    pf_vertex_t* out_vertex,
    const pf_mat3_t transform,
    const void* uniforms)
{
    (void)uniforms;

    pf_vertex_transform_vec_mat3(out_vertex,
        PF_ATTRIB_POSITION, transform);
}

void
pf_proc2d_fragment_default(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms)
{
    (void)rn;
    (void)uniforms;

    out_color->a[0] = vertex->elements[PF_ATTRIB_COLOR].value[0].v_uint8_t;
    out_color->a[1] = vertex->elements[PF_ATTRIB_COLOR].value[1].v_uint8_t;
    out_color->a[2] = vertex->elements[PF_ATTRIB_COLOR].value[2].v_uint8_t;
    out_color->a[3] = vertex->elements[PF_ATTRIB_COLOR].value[3].v_uint8_t;
}

void
pf_proc2d_fragment_texture_as_uniform(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms)
{
    (void)rn;
    (void)uniforms;

    const pf_texture2d_t* tex = uniforms;

    pf_vec2_t texcoord;
    pf_vertex_get_vec(vertex, PF_ATTRIB_TEXCOORD, texcoord);

    pf_color_t color;
    pf_vertex_get_vec(vertex, PF_ATTRIB_COLOR, color.a);

    *out_color = tex->sampler(tex, texcoord[0], texcoord[1]);
    *out_color = pf_color_blend_mul(*out_color, color);
}

/* Default Processor 3D Functions */

void
pf_proc3d_vertex_default(
    pf_vertex_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* uniforms)
{
    (void)uniforms;
    (void)mat_normal;

    pf_vec4_t position;
    position[3] = 1.0f;
    pf_vertex_get_vec(out_vertex, PF_ATTRIB_POSITION, position);
    pf_vec4_transform(out_homogeneous, position, mat_mvp);

    pf_vertex_transform_vec_mat4(out_vertex, PF_ATTRIB_POSITION, mat_model);
}

void
pf_proc3d_vertex_normal_transform(
    pf_vertex_t* out_vertex,
    pf_vec4_t out_homogeneous,
    const pf_mat4_t mat_model,
    const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp,
    const void* uniforms)
{
    (void)uniforms;

    pf_vec4_t position;
    position[3] = 1.0f;
    pf_vertex_get_vec(out_vertex, PF_ATTRIB_POSITION, position);
    pf_vec4_transform(out_homogeneous, position, mat_mvp);

    pf_vertex_transform_vec_mat4(out_vertex, PF_ATTRIB_POSITION, mat_model);
    pf_vertex_transform_vec_mat4(out_vertex, PF_ATTRIB_NORMAL, mat_normal);
}

void
pf_proc3d_fragment_default(
    struct pf_renderer* rn,
    pf_vertex_t* vertex,
    pf_color_t* out_color,
    const void* uniforms)
{
    (void)rn;
    (void)uniforms;

    pf_vertex_get_vec(vertex, PF_ATTRIB_COLOR, out_color);
}
