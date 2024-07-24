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

#include "pixelfactory/pf_camera3d.h"

void
pf_camera3d_get_right(
    const pf_camera3d_t* cam,
    pf_vec3_t dst)
{
    pf_camera3d_get_forward(cam, dst);
    pf_vec3_cross(dst, dst, cam->up);
}

void
pf_camera3d_get_forward(
    const pf_camera3d_t* cam,
    pf_vec3_t dst)
{
    pf_vec3_sub(dst, cam->target, cam->position);
    pf_vec3_normalize(dst, dst);
}

void
pf_camera3d_get_up(
    const pf_camera3d_t* cam,
    pf_vec3_t dst)
{
    pf_vec3_normalize_r(dst, cam->up);
}

void
pf_camera3d_move_right(
    pf_camera3d_t* cam, float distance,
    int move_in_world_plane)
{
    pf_vec3_t right;
    pf_camera3d_get_right(cam, right);

    if (move_in_world_plane)
    {
        // Project vector onto world plane
        right[1] = 0;
        pf_vec3_normalize(right, right);
    }

    // Scale by distance
    pf_vec3_scale(right, right, distance);

    // Move position and target
    pf_vec3_add(cam->position, cam->position, right);
    pf_vec3_add(cam->target, cam->target, right);
}

void
pf_camera3d_move_forward(
    pf_camera3d_t* cam, float distance,
    int move_in_world_plane)
{
    pf_vec3_t forward;
    pf_camera3d_get_forward(cam, forward);

    if (move_in_world_plane)
    {
        // Project vector onto world plane
        forward[1] = 0;
        pf_vec3_normalize(forward, forward);
    }

    // Scale by distance
    pf_vec3_scale(forward, forward, distance);

    // Move position and target
    pf_vec3_add(cam->position, cam->position, forward);
    pf_vec3_add(cam->target, cam->target, forward);
}

void
pf_camera3d_move_up(
    pf_camera3d_t* cam,
    float distance)
{
    pf_mat4_t up;
    pf_camera3d_get_up(cam, up);

    // Scale by distance
    pf_vec3_scale(up, up, distance);

    // Move position and target
    pf_vec3_add(cam->position, cam->position, up);
    pf_vec3_add(cam->target, cam->target, up);
}

void
pf_camera3d_move_to_target(
    pf_camera3d_t* cam,
    float distance)
{
    pf_vec3_t direction;
    
    pf_vec3_direction_r(direction, cam->target, cam->position);
    pf_vec3_scale(direction, direction, distance);

    pf_vec3_add(cam->position, cam->position, direction);
    pf_vec3_add(cam->target, cam->target, direction);
}

void
pf_camera3d_yaw(
    pf_camera3d_t* cam, float angle,
    int rotate_around_target)
{
    pf_vec3_t up, target_position;

    // Get the rotation axis (up)
    pf_camera3d_get_up(cam, up);

    // Calculate the view vector
    pf_vec3_sub(target_position, cam->target, cam->position);

    // Create the rotation matrix
    pf_mat4_t rotation_matrix;
    pf_mat4_rotate_axis_angle(rotation_matrix, up[0], up[1], up[2], angle);

    // Apply the rotation to the view vector
    pf_vec3_transform(target_position, target_position, rotation_matrix);

    if (rotate_around_target) {
        // Move the position relative to the target
        pf_vec3_sub(cam->position, cam->target, target_position);
    }
    else {
        // Move the target relative to the position
        pf_vec3_add(cam->target, cam->position, target_position);
    }
}

void
pf_camera3d_pitch(
    pf_camera3d_t* cam, float angle,
    int lock_view, int rotate_around_target, int rotate_up)
{
    pf_vec3_t target_position, right;

    // Calculate the view vector
    pf_vec3_sub(target_position, cam->target, cam->position);

    if (lock_view) {
        // Limit the pitch angle to avoid looking straight up or down
        float max_angle_up = pf_vec3_angle(cam->up, target_position) - 0.001f;
        if (angle > max_angle_up) angle = max_angle_up;
        
        pf_vec3_t neg_up;
        pf_vec3_scale(neg_up, cam->up, -1.0f);
        float max_angle_down = pf_vec3_angle(neg_up, target_position) + 0.001f;
        if (angle < max_angle_down) angle = max_angle_down;
    }

    // Get the rotation axis (right)
    pf_camera3d_get_right(cam, right);

    // Create the rotation matrix
    pf_mat4_t rotation_matrix;
    pf_mat4_rotate_axis_angle(rotation_matrix, right[0], right[1], right[2], angle);

    // Apply the rotation to the view vector
    pf_vec3_transform(target_position, target_position, rotation_matrix);

    if (rotate_around_target) {
        // Move the position relative to the target
        pf_vec3_sub(cam->position, cam->target, target_position);
    } else {
        // Move the target relative to the position
        pf_vec3_add(cam->target, cam->position, target_position);
    }

    if (rotate_up)
    {
        // Rotate the "up" direction around the "right" axis
        pf_vec3_transform(cam->up, cam->up, rotation_matrix);
    }
}

void
pf_camera3d_roll(
    pf_camera3d_t* cam,
    float angle)
{
    pf_vec3_t forward;

    // Get the rotation axis (forward)
    pf_camera3d_get_forward(cam, forward);

    // Create the rotation matrix
    pf_mat4_t rotation_matrix;
    pf_mat4_rotate_axis_angle(rotation_matrix, forward[0], forward[1], forward[2], angle);

    // Apply the rotation to the "up" direction
    pf_vec3_transform(cam->up, cam->up, rotation_matrix);
}

void
pf_camera3d_get_direction(
    const pf_camera3d_t* cam,
    pf_vec3_t dst)
{
    pf_vec3_sub(dst, cam->target, cam->position);
    pf_vec3_normalize(dst, dst);
}

void
pf_camera3d_set_direction(
    pf_camera3d_t* cam,
    const pf_vec3_t dir)
{
    pf_vec3_t normalized_dir;
    pf_vec3_normalize(normalized_dir, dir);
    pf_vec3_add(cam->target, cam->position, normalized_dir);
}

void
pf_camera3d_translate(
    pf_camera3d_t* cam,
    const pf_vec3_t delta)
{
    pf_vec3_add(cam->position, cam->position, delta);
    pf_vec3_add(cam->target, cam->target, delta);
}

void
pf_camera3d_rotate(
    pf_camera3d_t* cam,
    const pf_vec3_t delta,
    int lock_view)
{
    pf_vec3_t dt;
    pf_vec3_t right, up, forward, new_forward;
    pf_mat4_t rotationMatrixX, rotationMatrixY, rotationMatrixZ, rotationMatrix;

    pf_vec3_copy(dt, delta);

    // Get the right, up, and forward vectors of the camera
    pf_camera3d_get_right(cam, right);
    pf_camera3d_get_up(cam, up);
    pf_camera3d_get_forward(cam, forward);

    // Create the rotation matrices for each axis
    pf_mat4_rotate_axis_angle(rotationMatrixX, right[0], right[1], right[2], delta[0]);         // rotation around the X axis (pitch)
    pf_mat4_rotate_axis_angle(rotationMatrixY, up[0], up[1], up[2], delta[1]);                  // rotation around the Y axis (yaw)
    pf_mat4_rotate_axis_angle(rotationMatrixZ, forward[0], forward[1], forward[2], delta[2]);   // rotation around the Z axis (roll)

    // Combine the rotations
    pf_mat4_mul(rotationMatrix, rotationMatrixY, rotationMatrixX); // Yaw * Pitch
    pf_mat4_mul(rotationMatrix, rotationMatrix, rotationMatrixZ);  // (Yaw * Pitch) * Roll

    // Apply the rotation to the forward vector
    pf_vec3_transform(new_forward, forward, rotationMatrix);

    // Update the camera's target
    pf_vec3_add(cam->target, cam->position, new_forward);

    if (lock_view)
    {
        // If lock_view is enabled, limit the pitch angles to prevent looking directly up or down
        float maxAngleUp = pf_vec3_angle(up, new_forward) - 0.001f;
        if (dt[0] > maxAngleUp) dt[0] = maxAngleUp;
        
        float maxAngleDown = -pf_vec3_angle(up, new_forward) + 0.001f;
        if (dt[0] < maxAngleDown) dt[0] = maxAngleDown;

        // Recalculate the rotation with the limited angles
        pf_mat4_rotate_axis_angle(rotationMatrixX, right[0], right[1], right[2], delta[0]);
        pf_mat4_mul(rotationMatrix, rotationMatrixY, rotationMatrixX); // Yaw * Pitch
        pf_mat4_mul(rotationMatrix, rotationMatrix, rotationMatrixZ);  // (Yaw * Pitch) * Roll
        pf_vec3_transform(new_forward, forward, rotationMatrix);
        pf_vec3_add(cam->target, cam->position, new_forward);
    }

    // Update the camera's up vector if necessary (after rotation)
    pf_vec3_transform(cam->up, cam->up, rotationMatrix);
}

void
pf_camera3d_set_aspect(
    pf_camera3d_t* cam,
    const int viewport_dimension[2])
{
    cam->aspect = (viewport_dimension[0] > viewport_dimension[1])
        ? (float)viewport_dimension[0] / viewport_dimension[1]
        : (float)viewport_dimension[1] / viewport_dimension[0];
}


void
pf_camera3d_get_view_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst)
{
    pf_mat4_look_at(dst, cam->position, cam->target, cam->up);
}

void
pf_camera3d_get_perspective_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst)
{
    pf_mat4_perspective(dst, cam->fovy, cam->aspect, cam->near, cam->far);
}

void
pf_camera3d_get_orthographic_matrix(
    const pf_camera3d_t* cam,
    pf_mat4_t dst)
{
    double top = cam->fovy / 2.0;
    double right = top * cam->aspect;
    pf_mat4_ortho(dst, -right, right, -top, top, cam->near, cam->far);
}
