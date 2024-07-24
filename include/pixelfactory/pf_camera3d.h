#ifndef PF_CAMERA3D_H
#define PF_CAMERA3D_H

#include "math/pf_vec3.h"

typedef struct {
    pf_vec3_t position;
    pf_vec3_t target;
    pf_vec3_t up;
    float aspect;
    float fovy;
    float near;
    float far;
} pf_camera3d_t;

void
pf_camera3d_get_right(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

void
pf_camera3d_get_forward(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

void
pf_camera3d_get_up(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

void
pf_camera3d_move_right(
    pf_camera3d_t* cam, float distance,
    int move_in_world_plane);

void
pf_camera3d_move_forward(
    pf_camera3d_t* cam, float distance,
    int move_in_world_plane);

void
pf_camera3d_move_up(
    pf_camera3d_t* cam,
    float distance);

void
pf_camera3d_move_to_target(
    pf_camera3d_t* cam,
    float distance);

void
pf_camera3d_yaw(
    pf_camera3d_t* cam, float angle,
    int rotate_around_target);

void
pf_camera3d_pitch(
    pf_camera3d_t* cam, float angle,
    int lock_view, int rotate_around_target, int rotate_up);

void
pf_camera3d_roll(
    pf_camera3d_t* cam,
    float angle);

void
pf_camera3d_get_direction(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

void
pf_camera3d_set_direction(
    pf_camera3d_t* cam,
    const pf_vec3_t dir);

void
pf_camera3d_translate(
    pf_camera3d_t* cam,
    const pf_vec3_t delta);

void
pf_camera3d_rotate(
    pf_camera3d_t* cam,
    const pf_vec3_t delta,
    int lock_view);

void
pf_camera3d_set_aspect(
    pf_camera3d_t* cam,
    const int viewport_dimension[2]);

void
pf_camera3d_get_view_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst);

void
pf_camera3d_get_perspective_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst);

void
pf_camera3d_get_orthographic_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst);

#endif //PF_CAMERA3D_H
