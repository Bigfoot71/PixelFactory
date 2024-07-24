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

#ifndef PF_CAMERA3D_H
#define PF_CAMERA3D_H

#include "math/pf_vec3.h"
#include "pf_config.h"

typedef struct {
    pf_vec3_t position;
    pf_vec3_t target;
    pf_vec3_t up;
    float aspect;
    float fovy;
    float near;
    float far;
} pf_camera3d_t;

PFAPI void
pf_camera3d_get_right(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

PFAPI void
pf_camera3d_get_forward(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

PFAPI void
pf_camera3d_get_up(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

PFAPI void
pf_camera3d_move_right(
    pf_camera3d_t* cam, float distance,
    int move_in_world_plane);

PFAPI void
pf_camera3d_move_forward(
    pf_camera3d_t* cam, float distance,
    int move_in_world_plane);

PFAPI void
pf_camera3d_move_up(
    pf_camera3d_t* cam,
    float distance);

PFAPI void
pf_camera3d_move_to_target(
    pf_camera3d_t* cam,
    float distance);

PFAPI void
pf_camera3d_yaw(
    pf_camera3d_t* cam, float angle,
    int rotate_around_target);

PFAPI void
pf_camera3d_pitch(
    pf_camera3d_t* cam, float angle,
    int lock_view, int rotate_around_target, int rotate_up);

PFAPI void
pf_camera3d_roll(
    pf_camera3d_t* cam,
    float angle);

PFAPI void
pf_camera3d_get_direction(
    const pf_camera3d_t* cam,
    pf_vec3_t dst);

PFAPI void
pf_camera3d_set_direction(
    pf_camera3d_t* cam,
    const pf_vec3_t dir);

PFAPI void
pf_camera3d_translate(
    pf_camera3d_t* cam,
    const pf_vec3_t delta);

PFAPI void
pf_camera3d_rotate(
    pf_camera3d_t* cam,
    const pf_vec3_t delta,
    int lock_view);

PFAPI void
pf_camera3d_set_aspect(
    pf_camera3d_t* cam,
    const int viewport_dimension[2]);

PFAPI void
pf_camera3d_get_view_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst);

PFAPI void
pf_camera3d_get_perspective_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst);

PFAPI void
pf_camera3d_get_orthographic_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst);

#endif //PF_CAMERA3D_H
