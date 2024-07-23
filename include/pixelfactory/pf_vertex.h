#ifndef PF_VERTEX_H
#define PF_VERTEX_H

#include "math/pf_vec2.h"
#include "math/pf_vec4.h"
#include "pf_color.h"

typedef struct {
    pf_vec2_t position;
    pf_vec2_t texcoord;
    pf_color_t color;
    uint32_t index;
} pf_vertex2d_t;

typedef struct {
    float* positions;
    float* texcoords;
    pf_color_t* colors;
    uint32_t* indices;
    size_t num_vertices;
    size_t num_indices;
} pf_vertexbuffer2d_t;

typedef struct {
    pf_vec3_t position;
    pf_vec2_t texcoord;
    pf_vec3_t normal;
    pf_color_t color;
    uint32_t index;
} pf_vertex3d_t;

typedef struct {
    float* positions;
    float* texcoords;
    float* normals;
    pf_color_t* colors;
    uint32_t* indices;
    size_t num_vertices;
    size_t num_indices;
} pf_vertexbuffer3d_t;

#endif //PF_VERTEX_H
