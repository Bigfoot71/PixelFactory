#include "pixelfactory/pf_processors.h"
#include "pixelfactory/math/pf_vec4.h"
#include "pixelfactory/pf_color.h"
#include "pixelfactory/pf_renderer2d.h"
#include "pixelfactory/pf_renderer3d.h"
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/* Internal Helper Functions */

static pf_vertex3d_t
pf_vertex3d_lerp(const pf_vertex3d_t* start, const pf_vertex3d_t* end, float t)
{
    pf_vertex3d_t result = { 0 };

    const uint8_t *startCol = (const uint8_t*)(&start->color);
    const uint8_t *endCol = (const uint8_t*)(&end->color);
    uint8_t *resultCol = (uint8_t*)(&result.color);
    uint8_t uT = 255*t;

#   ifdef PF_SUPPORT_OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++)
    {
        resultCol[i] = startCol[i] + (uT*((int)endCol[i] - startCol[i]))/255;

        if (i < 2) {
            result.texcoord[i] = start->texcoord[i] + t*(end->texcoord[i] - start->texcoord[i]);
        }

        if (i < 3) {
            result.position[i] = start->position[i] + t*(end->position[i] - start->position[i]);
            result.normal[i] = start->normal[i] + t*(end->normal[i] - start->normal[i]);
        }
    }

    return result;
}


/* Default Processor 2D Functions */

void
pf_proc2d_vertex_default(pf_vertex2d_t* out_vertex, const pf_mat3_t transform, const void* attr)
{
    (void)attr;

    pf_vec2_transform(out_vertex->position, out_vertex->position, transform);
}

void
pf_proc2d_rasterizer_default(pf_vertex2d_t* out_vertex, pf_vertex2d_t* vertex_1, pf_vertex2d_t* vertex_2, pf_vertex2d_t* vertex_3, pf_vec3_t bary, void* attr)
{
    (void)attr;

    pf_vec2_bary_v_r(out_vertex->position, vertex_1->position, vertex_2->position, vertex_3->position, bary);
    pf_vec2_bary_v_r(out_vertex->texcoord, vertex_1->texcoord, vertex_2->texcoord, vertex_3->texcoord, bary);
    out_vertex->color = pf_color_bary_v(vertex_1->color, vertex_2->color, vertex_3->color, bary);
}

void
pf_proc2d_fragment_default(struct pf_renderer2d* rn, pf_vertex2d_t* vertex, pf_color_t* out_color, const void* attr)
{
    (void)rn;
    (void)attr;

    *out_color = vertex->color;
}


/* Default Processor 3D Functions */

void pf_proc3d_vertex_default(pf_vertex3d_t* out_vertex, pf_vec4_t out_homogeneous, const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp, const void* attr)
{
    (void)attr;

    pf_vec4_t position;
    position[3] = 1.0f;
    pf_vec3_copy(position, out_vertex->position);
    pf_vec4_transform(out_homogeneous, position, mat_mvp);

    pf_vec3_transform(out_vertex->position, out_vertex->position, mat_model);
    pf_vec3_transform(out_vertex->normal, out_vertex->normal, mat_normal);
}


// TODO: Fix the issue related to the depth which appears following clipping in certain situations
void
pf_proc3d_clip_triangle(const pf_renderer3d_t* rn, pf_vertex3d_t* out_vertices, pf_vec4_t out_homogeneous[], size_t* out_vertices_count)
{
    (void)rn;

    pf_vec4_t input_homogen[12];
    pf_vertex3d_t input_vt[12];
    int_fast8_t input_count;

    // CLIP W
    memcpy(input_homogen, out_homogeneous, (*out_vertices_count) * sizeof(pf_vec4_t));
    memcpy(input_vt, out_vertices, (*out_vertices_count) * sizeof(pf_vertex3d_t));
    input_count = *out_vertices_count;
    *out_vertices_count = 0;

    pf_vec4_t* prev_homogen = &input_homogen[input_count - 1];
    pf_vertex3d_t* prev_vt = &input_vt[input_count - 1];

    int_fast8_t prevDot = ((*prev_homogen)[3] < 1e-5f) ? -1 : 1;

    for (int_fast8_t i = 0; i < input_count; i++) {
        int_fast8_t currDot = (input_homogen[i][3] < 1e-5f) ? -1 : 1;

        if (prevDot * currDot < 0) {
            float t = (1e-5f - (*prev_homogen)[3]) / (input_homogen[i][3] - (*prev_homogen)[3]);
            pf_vec4_lerp_r(out_homogeneous[*out_vertices_count], *prev_homogen, input_homogen[i], t);
            out_vertices[*out_vertices_count] = pf_vertex3d_lerp(prev_vt, &input_vt[i], t);
            (*out_vertices_count)++;
        }

        if (currDot > 0) {
            pf_vec4_copy(out_homogeneous[*out_vertices_count], input_homogen[i]);
            out_vertices[*out_vertices_count] = input_vt[i];
            (*out_vertices_count)++;
        }

        prev_homogen = &input_homogen[i];
        prev_vt = &input_vt[i];
        prevDot = currDot;
    }

    if (*out_vertices_count == 0) {
        return;
    }

    // CLIP XYZ
    for (int_fast8_t iAxis = 0; iAxis < 3; iAxis++) {
        pf_vec4_t input_homogen[12];
        pf_vertex3d_t input_vt[12];
        int_fast8_t input_count;

        memcpy(input_homogen, out_homogeneous, (*out_vertices_count) * sizeof(pf_vec4_t));
        memcpy(input_vt, out_vertices, (*out_vertices_count) * sizeof(pf_vertex3d_t));
        input_count = *out_vertices_count;
        *out_vertices_count = 0;

        pf_vec4_t* prev_homogen = &input_homogen[input_count - 1];
        pf_vertex3d_t* prev_vt = &input_vt[input_count - 1];

        int_fast8_t prevDot = ((*prev_homogen)[iAxis] <= (*prev_homogen)[3]) ? 1 : -1;

        for (int_fast8_t i = 0; i < input_count; i++) {
            int_fast8_t currDot = (input_homogen[i][iAxis] <= input_homogen[i][3]) ? 1 : -1;

            if (prevDot * currDot <= 0) {
                float t = ((*prev_homogen)[3] - (*prev_homogen)[iAxis]) / 
                          (((*prev_homogen)[3] - (*prev_homogen)[iAxis]) - (input_homogen[i][3] - input_homogen[i][iAxis]));
                pf_vec4_lerp_r(out_homogeneous[*out_vertices_count], *prev_homogen, input_homogen[i], t);
                out_vertices[*out_vertices_count] = pf_vertex3d_lerp(prev_vt, &input_vt[i], t);
                (*out_vertices_count)++;
            }

            if (currDot > 0) {
                pf_vec4_copy(out_homogeneous[*out_vertices_count], input_homogen[i]);
                out_vertices[*out_vertices_count] = input_vt[i];
                (*out_vertices_count)++;
            }

            prev_homogen = &input_homogen[i];
            prev_vt = &input_vt[i];
            prevDot = currDot;
        }

        if (*out_vertices_count == 0) {
            return;
        }

        memcpy(input_homogen, out_homogeneous, (*out_vertices_count) * sizeof(pf_vec4_t));
        memcpy(input_vt, out_vertices, (*out_vertices_count) * sizeof(pf_vertex3d_t));
        input_count = *out_vertices_count;
        *out_vertices_count = 0;

        prev_homogen = &input_homogen[input_count - 1];
        prev_vt = &input_vt[input_count - 1];
        prevDot = (-(*prev_homogen)[iAxis] <= (*prev_homogen)[3]) ? 1 : -1;

        for (int_fast8_t i = 0; i < input_count; i++) {
            int_fast8_t currDot = (-input_homogen[i][iAxis] <= input_homogen[i][3]) ? 1 : -1;

            if (prevDot * currDot <= 0) {
                float t = ((*prev_homogen)[3] + (*prev_homogen)[iAxis]) / 
                          (((*prev_homogen)[3] + (*prev_homogen)[iAxis]) - (input_homogen[i][3] + input_homogen[i][iAxis]));
                pf_vec4_lerp_r(out_homogeneous[*out_vertices_count], *prev_homogen, input_homogen[i], t);
                out_vertices[*out_vertices_count] = pf_vertex3d_lerp(prev_vt, &input_vt[i], t);
                (*out_vertices_count)++;
            }

            if (currDot > 0) {
                pf_vec4_copy(out_homogeneous[*out_vertices_count], input_homogen[i]);
                out_vertices[*out_vertices_count] = input_vt[i];
                (*out_vertices_count)++;
            }

            prev_homogen = &input_homogen[i];
            prev_vt = &input_vt[i];
            prevDot = currDot;
        }

        if (*out_vertices_count == 0) {
            return;
        }
    }
}

void
pf_proc3d_screen_projection_default(const pf_renderer3d_t* rn, pf_vertex3d_t* vertices, pf_vec4_t homogeneous[], size_t vertices_count, int screen_pos[][2])
{
    for (size_t i = 0; i < vertices_count; i++)
    {
        pf_vertex3d_t* v = &vertices[i];
        pf_vec4_t* h = &homogeneous[i];

        // Calculation of the reciprocal of Z for the perspective correct
        (*h)[2] = 1.0f / (*h)[2];

        // Division of texture coordinates by the Z axis (perspective correct)
        pf_vec2_scale(v->texcoord, v->texcoord, (*h)[2]);

        // Division of vertex colors by the Z axis (perspective correct)
        v->color = pf_color_scale(v->color, (*h)[2]);

        // Division of XY coordinates by weight
        float invW = 1.0f / (*h)[3];
        (*h)[0] *= invW;
        (*h)[1] *= invW;

        // Convert homogeneous coordinates to screen coordinates
        screen_pos[i][0] = (rn->viewport_position[0] + ((*h)[0] + 1.0f) * 0.5f * rn->viewport_dimension[0]) + 0.5f;
        screen_pos[i][1] = (rn->viewport_position[1] + (1.0f - (*h)[1]) * 0.5f * rn->viewport_dimension[1]) + 0.5f;
    }
}

void
pf_proc3d_rasterizer_default(pf_vertex3d_t* out_vertex, pf_vertex3d_t* v1, pf_vertex3d_t* v2, pf_vertex3d_t* v3, pf_vec3_t bary, float z_depth, void* attr)
{
    (void)attr;

    // Interpolation of texture coordinates with correct perspective
    pf_vec2_bary_v_r(out_vertex->texcoord, v1->texcoord, v2->texcoord, v3->texcoord, bary);
    pf_vec2_scale(out_vertex->texcoord, out_vertex->texcoord, z_depth);

    // Interpolation of vertex colors with correct perspective
    out_vertex->color = pf_color_bary_v(v1->color, v2->color, v3->color, bary);
    out_vertex->color = pf_color_scale(out_vertex->color, z_depth);

    pf_vec3_bary_v(out_vertex->position, v1->position, v2->position, v3->position, bary);
    pf_vec3_bary_v(out_vertex->normal, v1->normal, v2->normal, v3->normal, bary);
}

void
pf_proc3d_fragment_default(struct pf_renderer3d* rn, pf_vertex3d_t* vertex, pf_color_t* out_color, const void* attr)
{
    (void)rn;
    (void)attr;

    *out_color = vertex->color;
}
