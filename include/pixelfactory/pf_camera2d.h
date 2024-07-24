#ifndef PF_CAMERA2D_H
#define PF_CAMERA2D_H

#include "math/pf_vec2.h"
#include "pf_helper.h"

typedef struct {
    pf_vec2_t target;
    pf_vec2_t offset;
    float rotation;
    float zoom;
} pf_camera2d_t;

void
pf_camera2d_translate(
    pf_camera2d_t* cam,
    float dx, float dy,
    int zoom_dependent);

void
pf_camera2d_zoom(
    pf_camera2d_t* cam, float inc,
    float min, float max,
    int zoom_dependent);

void
pf_camera2d_zoom_to_world_pos(
    pf_camera2d_t* cam, const pf_vec2_t world_pos,
    float inc, float min, float max,
    int zoom_dependent);

void
pf_camera2d_zoom_to_screen_pos(
    pf_camera2d_t* cam, const pf_vec2_t screen_pos,
    float inc, float min, float max,
    int zoom_dependent);

void
pf_camera2d_to_world(
    const pf_camera2d_t* cam,
    pf_vec2_t screen_pos);

void
pf_camera2d_to_screen(
    const pf_camera2d_t* cam,
    pf_vec2_t wolrd_pos);

void
pf_camera2d_get_view_matrix(
    const pf_camera2d_t* cam,
    pf_mat3_t dst);

#endif //PF_CAMERA2D_H
