#include "pixelfactory/pf_renderer3d.h"

/* Internal Functions Declarations */

void
pf_renderer3d_triangle_INTERNAL(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t mat_model, const pf_mat4_t mat_normal, const pf_mat4_t mat_mvp,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_rasterizer_fn rast_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


/* Public API Functions */

void
pf_renderer3d_vertex_buffer(pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
                            pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
                            void* attr)
{
    pf_renderer3d_vertex_buffer_ex(rn, vb, transform, vert_proc, NULL, NULL, NULL, frag_proc, attr);
}

void
pf_renderer3d_vertex_buffer_ex(pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
                               pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
                               pf_proc3d_screen_projection_fn proj_proc,
                               pf_proc3d_rasterizer_fn rast_proc,
                               pf_proc3d_fragment_fn frag_proc,
                               void* attr)
{
    float* positions = vb->positions;
    if (positions == NULL) return;

    float* texcoords = vb->texcoords;
    pf_color_t* colors = vb->colors;
    uint32_t* indices = vb->indices;
    float* normals = vb->normals;

    /* Preparation of matrices */

    pf_mat4_t mat_model;
    pf_mat4_t mat_normal;
    if (transform == NULL) {
        pf_mat4_identity(mat_model);
        pf_mat4_identity(mat_normal);
    } else {
        pf_mat4_copy(mat_model, transform);
        pf_mat4_inverse(mat_normal, mat_model);
        pf_mat4_transpose(mat_normal, mat_normal);
    }

    pf_mat4_t mat_mvp;
    pf_mat4_mul_r(mat_mvp, mat_model, rn->mat_view);
    pf_mat4_mul(mat_mvp, mat_mvp, rn->mat_proj);

    /* Set default processors if not set */

    if (vert_proc == NULL) vert_proc = pf_proc3d_vertex_default;
    if (clip_proc == NULL) clip_proc = pf_proc3d_clip_triangle;
    if (proj_proc == NULL) proj_proc = pf_proc3d_screen_projection_perspective_correct;
    if (rast_proc == NULL) rast_proc = pf_proc3d_rasterizer_perspective_correct;
    if (frag_proc == NULL) frag_proc = pf_proc3d_fragment_default;

    /* Iterates through all vertices in the vertex buffer */

    uint8_t has_indices = (indices != NULL);
    size_t num = (has_indices) ? vb->num_vertices : vb->num_indices;

    for (size_t i = 0; i < num; i += 3) {
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

        size_t i1 = 3 * index_1;
        size_t i2 = 3 * index_2;
        size_t i3 = 3 * index_3;

        pf_vertex3d_t v1 = { 0 };
        pf_vertex3d_t v2 = { 0 };
        pf_vertex3d_t v3 = { 0 };

        pf_vec3_copy(v1.position, positions + i1);
        pf_vec3_copy(v2.position, positions + i2);
        pf_vec3_copy(v3.position, positions + i3);

        if (texcoords != NULL) {
            pf_vec2_copy(v1.texcoord, texcoords + i1);
            pf_vec2_copy(v2.texcoord, texcoords + i2);
            pf_vec2_copy(v3.texcoord, texcoords + i3);
        }

        if (normals != NULL) {
            pf_vec3_copy(v1.normal, normals + i1);
            pf_vec3_copy(v2.normal, normals + i2);
            pf_vec3_copy(v3.normal, normals + i3);
        }

        if (colors != NULL) {
            v1.color = colors[i];
            v2.color = colors[i + 1];
            v3.color = colors[i + 2];
        } else {
            v1.color = PF_WHITE;
            v2.color = PF_WHITE;
            v3.color = PF_WHITE;
        }

        v1.index = index_1;
        v2.index = index_2;
        v3.index = index_3;

        pf_renderer3d_triangle_INTERNAL(
            rn, &v1, &v2, &v3, mat_model, mat_normal, mat_mvp,
            vert_proc, clip_proc, proj_proc,
            rast_proc, frag_proc,
            attr);
    }
}
