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

#include "pixelfactory/core/pf_renderer2d.h"
#include <string.h>

/* Internal Macros */

#define PF_MESH_TRIANGLE_TRAVEL(PIXEL_CODE)                                             \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            if ((w1 | w2 | w3) >= 0) {                                                  \
                size_t offset = y_offset + x;                                           \
                pf_vec3_t bary = {                                                      \
                    w1 * inv_w_sum,                                                     \
                    w2 * inv_w_sum,                                                     \
                    w3 * inv_w_sum                                                      \
                };                                                                      \
                PIXEL_CODE                                                              \
            }                                                                           \
            w1 += w1_x_step;                                                            \
            w2 += w2_x_step;                                                            \
            w3 += w3_x_step;                                                            \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }

#define PF_MESH_TRIANGLE_TRAVEL_OMP(PIXEL_CODE)                                         \
    _Pragma("omp parallel for schedule(dynamic)                                         \
        if ((xmax - xmin) * (ymax - ymin) >= PF_OMP_TRIANGLE_AABB_THRESHOLD)")          \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x = xmin; x <= xmax; ++x) {                                            \
            if ((w1 | w2 | w3) >= 0) {                                                  \
                size_t offset = y_offset + x;                                           \
                pf_vec3_t bary = {                                                      \
                    w1 * inv_w_sum,                                                     \
                    w2 * inv_w_sum,                                                     \
                    w3 * inv_w_sum                                                      \
                };                                                                      \
                PIXEL_CODE                                                              \
            }                                                                           \
            w1 += w1_x_step;                                                            \
            w2 += w2_x_step;                                                            \
            w3 += w3_x_step;                                                            \
        }                                                                               \
    }


/* Public API Functions */

void
pf_renderer2d_vertex_buffer(
    pf_renderer2d_t* rn,
    const pf_vertexbuffer2d_t* vb,
    const pf_mat3_t transform,
    pf_proc2d_triangle_t* proc)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    uint16_t* indices = vb->indices;
    pf_color_t* colors = vb->colors;

    uint8_t has_indices = (indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    /* Preparation of matrices */

    pf_mat3_t mat;
    if (transform != NULL) {
        pf_mat3_mul_r(mat, rn->mat_view, transform);
    } else {
        memcpy(mat, rn->mat_view, sizeof(pf_mat3_t));
    }

    /* Setup processors */

    pf_proc2d_triangle_t processor = { 0 };
    processor.vertex = pf_proc2d_vertex_default;
    processor.fragment = pf_proc2d_fragment_default;
    processor.rasterizer = pf_proc2d_rasterizer_default;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->rasterizer != NULL) processor.rasterizer = proc->rasterizer;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
        if (proc->varying != NULL) processor.varying = proc->varying;
    }

    for (uint32_t i = 0; i < num; i += 3) {

        /* Calculating vertex and array indexes */

        uint32_t index_1, index_2, index_3;

        if (has_indices) {
            index_1 = indices[i + 0];
            index_2 = indices[i + 1];
            index_3 = indices[i + 2];
        } else {
            index_1 = i + 0;
            index_2 = i + 1;
            index_3 = i + 2;
        }

        uint32_t i1 = 2 * index_1;
        uint32_t i2 = 2 * index_2;
        uint32_t i3 = 2 * index_3;

        /* Retrieving vertices and calling the vertex code */

        pf_vertex2d_t v1 = { 0 };
        pf_vertex2d_t v2 = { 0 };
        pf_vertex2d_t v3 = { 0 };

        pf_vec2_copy_f(v1.position, positions + i1);
        pf_vec2_copy_f(v2.position, positions + i2);
        pf_vec2_copy_f(v3.position, positions + i3);

        if (texcoords != NULL) {
            pf_vec2_copy_f(v1.texcoord, texcoords + i1);
            pf_vec2_copy_f(v2.texcoord, texcoords + i2);
            pf_vec2_copy_f(v3.texcoord, texcoords + i3);
        }

        if (colors != NULL) {
            v1.color = colors[i];
            v2.color = colors[i+1];
            v3.color = colors[i+2];
        } else {
            v1.color = PF_WHITE;
            v2.color = PF_WHITE;
            v3.color = PF_WHITE;
        }

        v1.index = index_1;
        v2.index = index_2;
        v3.index = index_3;

        /* Transform Vertices */

        processor.vertex(&v1, mat, processor.uniforms, processor.varying);
        processor.vertex(&v2, mat, processor.uniforms, processor.varying);
        processor.vertex(&v3, mat, processor.uniforms, processor.varying);

        /*
            Calculate the 2D bounding box of the triangle
            Determine the minimum and maximum x and y coordinates of the triangle vertices
        */

        int xmin = (int)PF_MAX(PF_MIN(v1.position[0], PF_MIN(v2.position[0], v3.position[0])), 0);
        int ymin = (int)PF_MAX(PF_MIN(v1.position[1], PF_MIN(v2.position[1], v3.position[1])), 0);
        int xmax = (int)PF_MIN(PF_MAX(v1.position[0], PF_MAX(v2.position[0], v3.position[0])), (int)rn->fb.w - 1);
        int ymax = (int)PF_MIN(PF_MAX(v1.position[1], PF_MAX(v2.position[1], v3.position[1])), (int)rn->fb.h - 1);

        /*
            Check the order of the vertices to determine if it's a front or back face
            NOTE: if signed_area is equal to 0, the face is degenerate
        */

        float signed_area = (v2.position[0] - v1.position[0]) *
                            (v3.position[1] - v1.position[1]) -
                            (v3.position[0] - v1.position[0]) *
                            (v2.position[1] - v1.position[1]);

        int_fast8_t is_back_face = (signed_area > 0);

        /*
            Barycentric interpolation setup
            Calculate the step increments for the barycentric coordinates
        */

        int w1_x_step = v3.position[1] - v2.position[1];
        int w1_y_step = v2.position[0] - v3.position[0];
        int w2_x_step = v1.position[1] - v3.position[1];
        int w2_y_step = v3.position[0] - v1.position[0];
        int w3_x_step = v2.position[1] - v1.position[1];
        int w3_y_step = v1.position[0] - v2.position[0];

        /*
            If the triangle is a back face, invert the steps
        */

        if (is_back_face) {
            w1_x_step = -w1_x_step, w1_y_step = -w1_y_step;
            w2_x_step = -w2_x_step, w2_y_step = -w2_y_step;
            w3_x_step = -w3_x_step, w3_y_step = -w3_y_step;
        }

        /*
            Calculate the initial barycentric coordinates
            for the top-left point of the bounding box
        */

        int w1_row = (xmin - v2.position[0]) * w1_x_step + w1_y_step * (ymin - v2.position[1]);
        int w2_row = (xmin - v3.position[0]) * w2_x_step + w2_y_step * (ymin - v3.position[1]);
        int w3_row = (xmin - v1.position[0]) * w3_x_step + w3_y_step * (ymin - v1.position[1]);

        /*
            Calculate the inverse of the sum of the barycentric coordinates for normalization
            NOTE: This sum remains constant throughout the triangle
        */

        float inv_w_sum = 1.0f / (float)(w1_row + w2_row + w3_row);

        /* Rendering of the triangle */

#if defined(_OPENMP)
        if (rn->blend != NULL) {
            PF_MESH_TRIANGLE_TRAVEL_OMP({
                pf_vertex2d_t vertex;
                processor.rasterizer(&vertex, &v1, &v2, &v3, bary, processor.varying);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms, processor.varying);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_MESH_TRIANGLE_TRAVEL_OMP({
                pf_vertex2d_t vertex;
                processor.rasterizer(&vertex, &v1, &v2, &v3, bary, processor.varying);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms, processor.varying);
                *ptr = final_color;
            })
        }
#else
        if (rn->blend != NULL) {
            PF_MESH_TRIANGLE_TRAVEL({
                pf_vertex2d_t vertex;
                processor.rasterizer(&vertex, &v1, &v2, &v3, bary, processor.varying);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms, processor.varying);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_MESH_TRIANGLE_TRAVEL({
                pf_vertex2d_t vertex;
                processor.rasterizer(&vertex, &v1, &v2, &v3, bary, processor.varying);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms, processor.varying);
                *ptr = final_color;
            })
        }
#endif
    }
}
