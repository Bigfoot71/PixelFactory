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

/* Helper Function Declarations */

void
pf_renderer3d_triangle_interpolation_INTERNAL(
    pf_vertex_t* out_vertex,
    pf_vertex_t* v1,
    pf_vertex_t* v2,
    pf_vertex_t* v3,
    pf_vec3_t bary,
    float z_depth);

/* Public API Functions */

void
pf_renderer2d_vertex_buffer(
    pf_renderer2d_t* rn,
    const pf_vertex_buffer_t* vb,
    const pf_mat3_t transform,
    const pf_proc2d_t* proc)
{
    /* Preparation of matrices */

    pf_mat3_t mat;
    if (transform != NULL) {
        pf_mat3_mul_r(mat, rn->mat_view, transform);
    } else {
        memcpy(mat, rn->mat_view, sizeof(pf_mat3_t));
    }

    /* Setup processors */

    pf_proc2d_t processor = { 0 };
    processor.vertex = pf_proc2d_vertex_default;
    processor.fragment = pf_proc2d_fragment_default;

    if (proc != NULL) {
        if (proc->vertex != NULL) processor.vertex = proc->vertex;
        if (proc->fragment != NULL) processor.fragment = proc->fragment;
        if (proc->uniforms != NULL) processor.uniforms = proc->uniforms;
    }

    /* Iterates through all vertices in the vertex buffer */

    const uint16_t* indices = indices;
    uint8_t has_indices = (vb->indices != NULL);
    uint32_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {

        /* Calculating vertex and array indexes */

        uint32_t index_1 = i + 0;
        uint32_t index_2 = i + 1;
        uint32_t index_3 = i + 2;

        if (has_indices) {
            index_1 = indices[index_1];
            index_2 = indices[index_2];
            index_3 = indices[index_3];
        }

        /* Retrieving vertices and calling the vertex code */

        pf_vertex_t v1 = { 0 };
        pf_vertex_t v2 = { 0 };
        pf_vertex_t v3 = { 0 };

        for (uint32_t j = 0; j < PF_MAX_ATTRIBUTES; ++j) {
            const pf_attribute_t* attr = &vb->attributes[j];
            if (attr->used != 0) {
                v1.elements[j] = pf_attribute_get_elem(attr, index_1);
                v2.elements[j] = pf_attribute_get_elem(attr, index_2);
                v3.elements[j] = pf_attribute_get_elem(attr, index_3);
            }
        }

        /* Transform Vertices */

        processor.vertex(&v1, mat, processor.uniforms);
        processor.vertex(&v2, mat, processor.uniforms);
        processor.vertex(&v3, mat, processor.uniforms);

        /*
            Calculate the 2D bounding box of the triangle
            Determine the minimum and maximum x and y coordinates of the triangle vertices
        */

        pf_vec3_t p1, p2, p3;
        pf_vertex_get_vec(&v1, PF_DEFAULT_ATTRIBUTE_POSITION_INDEX, p1);
        pf_vertex_get_vec(&v2, PF_DEFAULT_ATTRIBUTE_POSITION_INDEX, p2);
        pf_vertex_get_vec(&v3, PF_DEFAULT_ATTRIBUTE_POSITION_INDEX, p3);

        int xmin = (int)PF_MAX(PF_MIN(p1[0], PF_MIN(p2[0], p3[0])), 0);
        int ymin = (int)PF_MAX(PF_MIN(p1[1], PF_MIN(p2[1], p3[1])), 0);
        int xmax = (int)PF_MIN(PF_MAX(p1[0], PF_MAX(p2[0], p3[0])), (int)rn->fb.w - 1);
        int ymax = (int)PF_MIN(PF_MAX(p1[1], PF_MAX(p2[1], p3[1])), (int)rn->fb.h - 1);

        /*
            Check the order of the vertices to determine if it's a front or back face
            NOTE: if signed_area is equal to 0, the face is degenerate
        */

        float signed_area = (p2[0] - p1[0]) *
                            (p3[1] - p1[1]) -
                            (p3[0] - p1[0]) *
                            (p2[1] - p1[1]);

        int_fast8_t is_back_face = (signed_area > 0);

        /*
            Barycentric interpolation setup
            Calculate the step increments for the barycentric coordinates
        */

        int w1_x_step = p3[1] - p2[1];
        int w1_y_step = p2[0] - p3[0];
        int w2_x_step = p1[1] - p3[1];
        int w2_y_step = p3[0] - p1[0];
        int w3_x_step = p2[1] - p1[1];
        int w3_y_step = p1[0] - p2[0];

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

        int w1_row = (xmin - p2[0]) * w1_x_step + w1_y_step * (ymin - p2[1]);
        int w2_row = (xmin - p3[0]) * w2_x_step + w2_y_step * (ymin - p3[1]);
        int w3_row = (xmin - p1[0]) * w3_x_step + w3_y_step * (ymin - p1[1]);

        /*
            Calculate the inverse of the sum of the barycentric coordinates for normalization
            NOTE: This sum remains constant throughout the triangle
        */

        float inv_w_sum = 1.0f / (float)(w1_row + w2_row + w3_row);

        /* Rendering of the triangle */

#if defined(_OPENMP)
        if (rn->blend != NULL) {
            PF_MESH_TRIANGLE_TRAVEL_OMP({
                pf_vertex_t vertex;
                pf_renderer3d_triangle_interpolation_INTERNAL(
                    &vertex, &v1, &v2, &v3, bary, 1);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_MESH_TRIANGLE_TRAVEL_OMP({
                pf_vertex_t vertex;
                pf_renderer3d_triangle_interpolation_INTERNAL(
                    &vertex, &v1, &v2, &v3, bary, 1);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms);
                *ptr = final_color;
            })
        }
#else
        if (rn->blend != NULL) {
            PF_MESH_TRIANGLE_TRAVEL({
                pf_vertex_t vertex;
                pf_renderer3d_triangle_interpolation_INTERNAL(
                    &vertex, &v1, &v2, &v3, bary, 1);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_MESH_TRIANGLE_TRAVEL({
                pf_vertex_t vertex;
                pf_renderer3d_triangle_interpolation_INTERNAL(
                    &vertex, &v1, &v2, &v3, bary, 1);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                processor.fragment(rn, &vertex, &final_color, processor.uniforms);
                *ptr = final_color;
            })
        }
#endif
    }
}
