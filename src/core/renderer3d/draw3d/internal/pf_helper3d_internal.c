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

#include "pixelfactory/core/pf_renderer3d.h"

void
pf_renderer3d_screen_projection_INTERNAL(
    const pf_renderer3d_t* rn,
    pf_vec4_t homogeneous[],
    pf_vertex_t vertices[],
    size_t vertices_count,
    int screen_pos[][2])
{
    for (size_t i = 0; i < vertices_count; ++i)
    {
        pf_vec4_t* h = &homogeneous[i];

        // Calculation of the reciprocal of HZ (to get correct depth)
        (*h)[2] = 1.0f / (*h)[2];

        // Perspective correct is needed
        // Division by the HZ axis (perspective correct)
        if (fabsf(*h[3] - 1.0f) > PF_EPSILON) {
            pf_vertex_t* v = &vertices[i];
            pf_vertex_scale_vec(v, PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX, (*h)[2]);
            pf_vertex_scale_vec(v, PF_DEFAULT_ATTRIBUTE_COLOR_INDEX, (*h)[2]);
        }

        // Division of XY coordinates by weight
        float inv_hw = 1.0f / (*h)[3];
        (*h)[0] *= inv_hw;
        (*h)[1] *= inv_hw;

        // Convert homogeneous coordinates to screen coordinates
        screen_pos[i][0] = (rn->viewport_position[0] + ((*h)[0] + 1.0f) * 0.5f * rn->viewport_dimension[0]) + 0.5f;
        screen_pos[i][1] = (rn->viewport_position[1] + (1.0f - (*h)[1]) * 0.5f * rn->viewport_dimension[1]) + 0.5f;
    }
}

void
pf_renderer3d_triangle_interpolation_INTERNAL(
    pf_vertex_t* out_vertex,
    pf_vertex_t* v1,
    pf_vertex_t* v2,
    pf_vertex_t* v3,
    pf_vec3_t bary,
    float z_depth)
{
    pf_vertex_bary(out_vertex, v1, v2, v3, bary[0], bary[1], bary[2]);
    pf_vertex_scale_vec(out_vertex, PF_DEFAULT_ATTRIBUTE_TEXCOORD_INDEX, z_depth);
    pf_vertex_scale_vec(out_vertex, PF_DEFAULT_ATTRIBUTE_COLOR_INDEX, z_depth);
}
