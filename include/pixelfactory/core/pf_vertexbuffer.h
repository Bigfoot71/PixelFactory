#ifndef PF_VERTEXBUFFER_H
#define PF_VERTEXBUFFER_H

#include "../components/pf_attribute.h"
#include "../components/pf_color.h"

/* Vertex Buffer Types */

typedef struct {
    pf_attribute_t attributes[PF_MAX_ATTRIBUTES];
    uint16_t* indices;
    uint32_t num_vertices;
    uint32_t num_indices;
} pf_vertexbuffer_t;

/* Helper Vertex Buffer Functions */

PFAPI pf_vertexbuffer_t
pf_vertexbuffer_create_2d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    pf_color_t* colors);

PFAPI pf_vertexbuffer_t
pf_vertexbuffer_create_3d(
    uint32_t num_vertices,
    float* positions,
    float* texcoords,
    float* normals,
    pf_color_t* colors);

PFAPI void
pf_vertexbuffer_delete(
    pf_vertexbuffer_t* vb);

/* VERTEX BUFFER EXTENSION */


#ifdef PF_EXT_VERTEXBUFFER

/* Loading Functions */

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_load_obj(
    const char* file_path,
    bool triangulate);

PFAPI pf_vertexbuffer_t*
pfext_vertexbuffer_load_gltf(
    const char* file_path,
    int* num_vertexbuffer);

/* Generation Functions */

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_cylinder(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_cone(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_disk(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_torus(
    int slices, int stacks, float radius);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_sphere(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_subdivided_sphere(
    int nsubdivisions);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_klein_bottle(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_trefoil_knot(
    int slices, int stacks, float radius);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_hemisphere(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_plane(
    int slices, int stacks);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_icosahedron();

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_dodecahedron();

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_octahedron();

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_tetrahedron();

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_cube();

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_disk_ex(
    float radius, int slices,
    const pf_vec3_t center,
    const pf_vec3_t normal);

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_empty();

PFAPI pf_vertexbuffer_t
pfext_vertexbuffer_gen_rock(
    int seed, int nsubdivisions);

#endif //PF_EXT_VERTEXBUFFER
#endif //PF_VERTEXBUFFER_H
