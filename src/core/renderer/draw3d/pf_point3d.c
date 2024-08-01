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

#include "pixelfactory/core/pf_renderer.h"

/* Internal Functions Defintions */

void
pf_renderer_point3d_INTERNAL(
    pf_renderer_t* rn, const pf_vertex_t* point, float radius,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc);


/* Public API Functions */

void
pf_renderer_point(
    pf_renderer_t* rn,
    const pf_vec3_t point,
    pf_color_t color)
{
    if (rn->conf3d == NULL) {
        return;
    }

    const float mat_identity[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    pf_mat4_t mat_mvp;
    pf_mat4_mul(mat_mvp,
        rn->conf3d->mat_view,
        rn->conf3d->mat_proj);

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    pf_vertex_t vertex = { 0 };

    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_ATTRIB_FLOAT;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].used = true;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 3;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_ATTRIB_UBYTE;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].used = true;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;

    for (int_fast8_t i = 0; i < 3; ++i) {
        vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[i].v_float = point[i];
    }

    for (int_fast8_t i = 0; i < 4; ++i) {
        vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[i].v_uint8_t = color.a[i];
    }

    pf_renderer_point3d_INTERNAL(rn, &vertex, 0, mat_identity, mat_identity, mat_mvp, &processor);
}

void
pf_renderer_point_thick(
    pf_renderer_t* rn,
    const pf_vec3_t point,
    float radius,
    pf_color_t color)
{
    if (rn->conf3d == NULL) {
        return;
    }

    const float mat_identity[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    pf_mat4_t mat_mvp;
    pf_mat4_mul(mat_mvp,
        rn->conf3d->mat_view,
        rn->conf3d->mat_proj);

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    pf_vertex_t vertex = { 0 };

    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].type = PF_ATTRIB_FLOAT;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].used = true;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].comp = 3;

    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].type = PF_ATTRIB_UBYTE;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].used = true;
    vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].comp = 4;

    for (int_fast8_t i = 0; i < 3; ++i) {
        vertex.elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[i].v_float = point[i];
    }

    for (int_fast8_t i = 0; i < 4; ++i) {
        vertex.elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX].value[i].v_uint8_t = color.a[i];
    }

    pf_renderer_point3d_INTERNAL(rn, &vertex, radius, mat_identity, mat_identity, mat_mvp, &processor);
}
