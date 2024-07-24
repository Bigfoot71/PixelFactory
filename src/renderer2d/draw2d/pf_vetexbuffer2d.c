#include "pixelfactory/pf_renderer2d.h"
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
    _Pragma("omp parallel for schedule(dynamic)")                                       \
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
pf_renderer2d_vertex_buffer(pf_renderer2d_t* rn, const pf_vertexbuffer2d_t* vb, const pf_mat3_t transform, pf_proc2d_vertex_fn vert_proc, pf_proc2d_fragment_fn frag_proc, void* attr)
{
    pf_renderer2d_vertex_buffer_ex(rn, vb, transform, vert_proc, pf_proc2d_rasterizer_default, frag_proc, attr);
}

void
pf_renderer2d_vertex_buffer_ex(pf_renderer2d_t* rn, const pf_vertexbuffer2d_t* vb, const pf_mat3_t transform, pf_proc2d_vertex_fn vert_proc, pf_proc2d_rasterizer_fn rast_proc, pf_proc2d_fragment_fn frag_proc, void* attr)
{
    if (vert_proc == NULL) {
        vert_proc = pf_proc2d_vertex_default;
    }
    if (rast_proc == NULL) {
        rast_proc = pf_proc2d_rasterizer_default;
    }
    if (frag_proc == NULL) {
        frag_proc = pf_proc2d_fragment_default;
    }

    float* positions = vb->positions;
    if (positions == NULL) return;

    pf_mat3_t mat;
    if (transform != NULL) {
        pf_mat3_mul_r(mat, rn->mat_view, transform);
    } else {
        memcpy(mat, rn->mat_view, sizeof(pf_mat3_t));
    }

    float* texcoords = vb->texcoords;
    uint32_t* indices = vb->indices;
    pf_color_t* colors = vb->colors;

    uint8_t has_indices = (indices != NULL);
    size_t num = (has_indices) ? vb->num_indices : vb->num_vertices;

    for (uint32_t i = 0; i < num; i += 3) {

        /* Calculating vertex and array indexes */

        uint32_t index_1 = (indices != NULL) ? indices[i + 0] : i + 0;
        uint32_t index_2 = (indices != NULL) ? indices[i + 1] : i + 1;
        uint32_t index_3 = (indices != NULL) ? indices[i + 2] : i + 2;

        size_t i1 = 2 * index_1;
        size_t i2 = 2 * index_2;
        size_t i3 = 2 * index_3;

        /* Retrieving vertices and calling the vertex code */

        pf_vertex2d_t v1 = { 0 };
        pf_vertex2d_t v2 = { 0 };
        pf_vertex2d_t v3 = { 0 };

        pf_vec2_copy(v1.position, positions + i1);
        pf_vec2_copy(v2.position, positions + i2);
        pf_vec2_copy(v3.position, positions + i3);

        if (texcoords != NULL) {
            pf_vec2_copy(v1.texcoord, texcoords + i1);
            pf_vec2_copy(v2.texcoord, texcoords + i2);
            pf_vec2_copy(v3.texcoord, texcoords + i3);
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

        vert_proc(&v1, mat, attr);
        vert_proc(&v2, mat, attr);
        vert_proc(&v3, mat, attr);

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
                rast_proc(&vertex, &v1, &v2, &v3, bary, attr);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_MESH_TRIANGLE_TRAVEL_OMP({
                pf_vertex2d_t vertex;
                rast_proc(&vertex, &v1, &v2, &v3, bary, attr);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = final_color;
            })
        }
#else
        if (rn->blend != NULL) {
            PF_MESH_TRIANGLE_TRAVEL({
                pf_vertex2d_t vertex;
                rast_proc(&vertex, &v1, &v2, &v3, bary, attr);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = rn->blend(*ptr, final_color);
            })
        } else {
            PF_MESH_TRIANGLE_TRAVEL({
                pf_vertex2d_t vertex;
                rast_proc(&vertex, &v1, &v2, &v3, bary, attr);

                pf_color_t* ptr = rn->fb.buffer + offset;
                pf_color_t final_color = *ptr;

                frag_proc(rn, &vertex, &final_color, attr);
                *ptr = final_color;
            })
        }
#endif
    }
}





/* KEEP */

/*
#define PF_MESH_TRIANGLE_TRAVEL_AVX(PIXEL_CODE)                                         \
    __m256 inv_w_sum_v = _mm256_set1_ps(inv_w_sum);                                     \
    __m256i c1_r = _mm256_set1_epi32(c1.c.r),                                           \
            c1_g = _mm256_set1_epi32(c1.c.g),                                           \
            c1_b = _mm256_set1_epi32(c1.c.b),                                           \
            c1_a = _mm256_set1_epi32(c1.c.a);                                           \
    __m256i c2_r = _mm256_set1_epi32(c2.c.r),                                           \
            c2_g = _mm256_set1_epi32(c2.c.g),                                           \
            c2_b = _mm256_set1_epi32(c2.c.b),                                           \
            c2_a = _mm256_set1_epi32(c2.c.a);                                           \
    __m256i c3_r = _mm256_set1_epi32(c3.c.r),                                           \
            c3_g = _mm256_set1_epi32(c3.c.g),                                           \
            c3_b = _mm256_set1_epi32(c3.c.b),                                           \
            c3_a = _mm256_set1_epi32(c3.c.a);                                           \
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        int w1 = w1_row;                                                                \
        int w2 = w2_row;                                                                \
        int w3 = w3_row;                                                                \
        for (int x8 = xmin; x8 <= xmax; x8 += 8) {                                      \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            __m256i mask = _mm256_and_si256(_mm256_and_si256(                           \
                _mm256_cmpgt_epi32(w1_v, _mm256_setzero_si256()),                       \
                _mm256_cmpgt_epi32(w2_v, _mm256_setzero_si256())),                      \
                _mm256_cmpgt_epi32(w3_v, _mm256_setzero_si256()));                      \
            int mask_int = _mm256_movemask_epi8(mask);                                  \
            __m256 w1_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w1_v), inv_w_sum_v);    \
            __m256 w2_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w2_v), inv_w_sum_v);    \
            __m256 w3_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w3_v), inv_w_sum_v);    \
            __m256i colors = pf_color_bary_avx(                                         \
                c1_r, c1_g, c1_b, c1_a,                                                 \
                c2_r, c2_g, c2_b, c2_a,                                                 \
                c3_r, c3_g, c3_b, c3_a,                                                 \
                w1_norm_v,                                                              \
                w2_norm_v,                                                              \
                w3_norm_v);                                                             \
            for (int i = 0; i < 8; i++) {                                               \
                if (mask_int & (1 << (i * 4))) {                                        \
                    int x = x8 + i;                                                     \
                    if (x <= xmax) {                                                    \
                        size_t offset = y_offset + x;                                   \
                        float aw1 = (float)w1*inv_w_sum;                                \
                        float aw2 = (float)w2*inv_w_sum;                                \
                        float aw3 = (float)w3*inv_w_sum;                                \
                        pf_color_t fragColor;                                           \
                        _mm_storeu_si32(&fragColor.v, _mm256_castsi256_si128(           \
                            _mm256_permutevar8x32_epi32(colors, _mm256_set1_epi32(i))));\
                        pf_vec2_t tc;                                                   \
                        pf_vec2_bary_r(tc, tc1, tc2, tc3, aw1, aw2, aw3);               \
                        PIXEL_CODE                                                      \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            w1 += 8 * w1_x_step;                                                        \
            w2 += 8 * w2_x_step;                                                        \
            w3 += 8 * w3_x_step;                                                        \
        }                                                                               \
        w1_row += w1_y_step;                                                            \
        w2_row += w2_y_step;                                                            \
        w3_row += w3_y_step;                                                            \
    }                                                                                   \

#define PF_MESH_TRIANGLE_TRAVEL_OMP_AVX(PIXEL_CODE)                                     \
    __m256 inv_w_sum_v = _mm256_set1_ps(inv_w_sum);                                     \
    __m256i c1_r = _mm256_set1_epi32(c1.c.r),                                           \
            c1_g = _mm256_set1_epi32(c1.c.g),                                           \
            c1_b = _mm256_set1_epi32(c1.c.b),                                           \
            c1_a = _mm256_set1_epi32(c1.c.a);                                           \
    __m256i c2_r = _mm256_set1_epi32(c2.c.r),                                           \
            c2_g = _mm256_set1_epi32(c2.c.g),                                           \
            c2_b = _mm256_set1_epi32(c2.c.b),                                           \
            c2_a = _mm256_set1_epi32(c2.c.a);                                           \
    __m256i c3_r = _mm256_set1_epi32(c3.c.r),                                           \
            c3_g = _mm256_set1_epi32(c3.c.g),                                           \
            c3_b = _mm256_set1_epi32(c3.c.b),                                           \
            c3_a = _mm256_set1_epi32(c3.c.a);                                           \
    __m256i offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);                         \
    __m256i w1_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w1_x_step), offset);     \
    __m256i w2_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w2_x_step), offset);     \
    __m256i w3_x_step_v = _mm256_mullo_epi32(_mm256_set1_epi32(w3_x_step), offset);     \
    _Pragma("omp parallel for")                                                         \
    for (int y = ymin; y <= ymax; ++y) {                                                \
        size_t y_offset = y * rn->fb.w;                                                 \
        int iy = y - ymin;                                                              \
        int w1 = w1_row + iy * w1_y_step;                                               \
        int w2 = w2_row + iy * w2_y_step;                                               \
        int w3 = w3_row + iy * w3_y_step;                                               \
        for (int x8 = xmin; x8 <= xmax; x8 += 8) {                                      \
            __m256i w1_v = _mm256_add_epi32(_mm256_set1_epi32(w1), w1_x_step_v);        \
            __m256i w2_v = _mm256_add_epi32(_mm256_set1_epi32(w2), w2_x_step_v);        \
            __m256i w3_v = _mm256_add_epi32(_mm256_set1_epi32(w3), w3_x_step_v);        \
            __m256i mask = _mm256_and_si256(_mm256_and_si256(                           \
                _mm256_cmpgt_epi32(w1_v, _mm256_setzero_si256()),                       \
                _mm256_cmpgt_epi32(w2_v, _mm256_setzero_si256())),                      \
                _mm256_cmpgt_epi32(w3_v, _mm256_setzero_si256()));                      \
            int mask_int = _mm256_movemask_epi8(mask);                                  \
            __m256 w1_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w1_v), inv_w_sum_v);    \
            __m256 w2_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w2_v), inv_w_sum_v);    \
            __m256 w3_norm_v = _mm256_mul_ps(_mm256_cvtepi32_ps(w3_v), inv_w_sum_v);    \
            __m256i colors = pf_color_bary_avx(                                         \
                c1_r, c1_g, c1_b, c1_a,                                                 \
                c2_r, c2_g, c2_b, c2_a,                                                 \
                c3_r, c3_g, c3_b, c3_a,                                                 \
                w1_norm_v,                                                              \
                w2_norm_v,                                                              \
                w3_norm_v);                                                             \
            for (int i = 0; i < 8; i++) {                                               \
                if (mask_int & (1 << (i * 4))) {                                        \
                    int x = x8 + i;                                                     \
                    if (x <= xmax) {                                                    \
                        size_t offset = y_offset + x;                                   \
                        float aw1 = (float)w1*inv_w_sum;                                \
                        float aw2 = (float)w2*inv_w_sum;                                \
                        float aw3 = (float)w3*inv_w_sum;                                \
                        pf_color_t fragColor;                                           \
                        _mm_storeu_si32(&fragColor.v, _mm256_castsi256_si128(           \
                            _mm256_permutevar8x32_epi32(colors, _mm256_set1_epi32(i))));\
                        pf_vec2_t tc;                                                   \
                        pf_vec2_bary_r(tc, tc1, tc2, tc3, aw1, aw2, aw3);               \
                        PIXEL_CODE                                                      \
                    }                                                                   \
                }                                                                       \
                w1 += w1_x_step;                                                        \
                w2 += w2_x_step;                                                        \
                w3 += w3_x_step;                                                        \
            }                                                                           \
        }                                                                               \
    }
*/
