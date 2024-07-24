#ifndef PF_RENDER3D_H
#define PF_RENDER3D_H

#include "pf_framebuffer.h"
#include "pf_depthbuffer.h"
#include "pf_processors.h"
#include "math/pf_mat3.h"
#include "pf_texture2d.h"
#include "pf_vertex.h"
#include "pf_depth.h"

typedef enum {
    PF_BACK,
    PF_FRONT
} pf_face_e;

typedef enum {
    PF_CULL_NONE,
    PF_CULL_BACK,
    PF_CULL_FRONT
} pf_cullmode_e;

typedef struct pf_renderer3d {
    pf_mat4_t           mat_view;
    pf_mat4_t           mat_proj;
    pf_framebuffer_t    fb;
    pf_depthbuffer_t    zb;
    pf_color_blend_fn   blend;
    pf_depth_test_fn    test;
    int                 viewport_position[2];
    int                 viewport_dimension[2];
    pf_cullmode_e       cull_mode;
} pf_renderer3d_t;


/* Renderer Mangement Functions */

pf_renderer3d_t
pf_renderer3d_create(
    uint32_t w, uint32_t h,
    pf_color_blend_fn blend,
    pf_depth_test_fn test);

void
pf_renderer3d_delete(
    pf_renderer3d_t* rn);

void
pf_renderer3d_clear(
    pf_renderer3d_t* rn,
    pf_color_t clear_color,
    float clear_depth);

void
pf_renderer3d_viewport(
    pf_renderer3d_t* rn,
    int x, int y,
    int w, int h);


/* Renderer 2D Buffer Drawing */

void
pf_renderer3d_vertex_buffer(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_rasterizer_fn rast_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_points(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_points_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_points_thick(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_points_thick_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_lines(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_lines_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_lines_thick(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_vertex_buffer_lines_thick_ex(
    pf_renderer3d_t* rn, const pf_vertexbuffer3d_t* vb, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


/* Renderer 3D Points */

void
pf_renderer3d_point(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_point_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_point_thick(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_point_thick_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* point, float radius, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


/* Renderer 3D Lines */

void
pf_renderer3d_line(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_line_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_line_thick(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_line_thick_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, float thick, const pf_mat4_t transform,
    pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


/* Renderer 3D Triangles */

void
pf_renderer3d_triangle(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t transform, pf_proc3d_vertex_fn vert_proc, pf_proc3d_fragment_fn frag_proc,
    void* attr);

void
pf_renderer3d_triangle_ex(
    pf_renderer3d_t* rn, const pf_vertex3d_t* v1, const pf_vertex3d_t* v2, const pf_vertex3d_t* v3,
    const pf_mat4_t transform, pf_proc3d_vertex_fn vert_proc, pf_proc3d_clip_fn clip_proc,
    pf_proc3d_screen_projection_fn proj_proc, pf_proc3d_rasterizer_fn rast_proc,
    pf_proc3d_fragment_fn frag_proc,
    void* attr);


#endif //PF_RENDERER3D_H
