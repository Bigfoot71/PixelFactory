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
#include "pixelfactory/core/pf_renderer3d.h"
#include <stdint.h>
#include <string.h>

/* Internal Functions Declarations */

void
pf_renderer3d_triangle_INTERNAL(
    pf_renderer3d_t* rn, pf_vertex_t vertices[PF_MAX_CLIPPED_POLYGON_VERTICES],
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal,
    const pf_mat4_t mat_mvp, const pf_proc3d_t* proc);

/* Triangle rasterization functions */

void
pf_renderer3d_triangle(
    pf_renderer3d_t* rn,
    const pf_vec3_t p1,
    const pf_vec3_t p2,
    const pf_vec3_t p3,
    pf_color_t color)
{
    const float mat_identity[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    pf_mat4_t mat_mvp;
    pf_mat4_mul(mat_mvp, rn->mat_view, rn->mat_proj);

    pf_proc3d_t processor = { 0 };
    processor.vertex = pf_proc3d_vertex_default;
    processor.fragment = pf_proc3d_fragment_default;

    pf_vertex_t vertices[12] = { 0 };

    for (int_fast8_t i = 0; i < 3; ++i) {
        pf_attrib_elem_t* pos = &vertices[i].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX];
        pf_attrib_elem_t* col = &vertices[i].elements[PF_DEFAULT_ATTRIBUTE_COLOR_INDEX];

        pos->used = true, pos->comp = 3, pos->type = PF_ATTRIB_FLOAT;
        col->used = true, col->comp = 4, col->type = PF_ATTRIB_UBYTE;

        for (int_fast8_t i = 0; i < 4; ++i) {
            col->value[i].v_uint8_t = color.a[i];
        }
    }

    vertices[0].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[0].v_float = p1[0];
    vertices[0].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[1].v_float = p1[1];
    vertices[0].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[2].v_float = p1[2];

    vertices[1].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[0].v_float = p2[0];
    vertices[1].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[1].v_float = p2[1];
    vertices[1].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[2].v_float = p2[2];

    vertices[2].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[0].v_float = p3[0];
    vertices[2].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[1].v_float = p3[1];
    vertices[2].elements[PF_DEFAULT_ATTRIBUTE_POSITION_INDEX].value[2].v_float = p3[2];

    pf_renderer3d_triangle_INTERNAL(
        rn, vertices, mat_identity, mat_identity, mat_mvp, &processor);
}
