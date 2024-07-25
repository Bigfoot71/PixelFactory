cmake_minimum_required(VERSION 3.21)

# Path to example resources
add_definitions(-DRESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../resources/")

# 2D - Circle example
add_executable(raylib_circle_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_circle_2d.c)
target_link_libraries(raylib_circle_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_circle_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Framebuffer example
add_executable(raylib_framebuffer_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_framebuffer_2d.c)
target_link_libraries(raylib_framebuffer_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_framebuffer_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Lines example
add_executable(raylib_lines_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_lines_2d.c)
target_link_libraries(raylib_lines_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_lines_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Mesh2D example
add_executable(raylib_mesh_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_mesh_2d.c)
target_link_libraries(raylib_mesh_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_mesh_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Rectangle example
add_executable(raylib_rect_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_rect_2d.c)
target_link_libraries(raylib_rect_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_rect_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Texture2D example
add_executable(raylib_texture_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_texture_2d.c)
target_link_libraries(raylib_texture_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_texture_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Triangle example
add_executable(raylib_triangle_2d ${PF_ROOT_PATH}/examples/raylib/2d/raylib_triangle_2d.c)
target_link_libraries(raylib_triangle_2d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_triangle_2d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - First Person example
add_executable(raylib_first_person_3d ${PF_ROOT_PATH}/examples/raylib/3d/raylib_first_person_3d.c)
target_link_libraries(raylib_first_person_3d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_first_person_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Lighting example
add_executable(raylib_lighting_3d ${PF_ROOT_PATH}/examples/raylib/3d/raylib_lighting_3d.c)
target_link_libraries(raylib_lighting_3d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_lighting_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Lines example
add_executable(raylib_lines_3d ${PF_ROOT_PATH}/examples/raylib/3d/raylib_lines_3d.c)
target_link_libraries(raylib_lines_3d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_lines_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Model example
add_executable(raylib_model_3d ${PF_ROOT_PATH}/examples/raylib/3d/raylib_model_3d.c)
target_link_libraries(raylib_model_3d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_model_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Points example
add_executable(raylib_points_3d ${PF_ROOT_PATH}/examples/raylib/3d/raylib_points_3d.c)
target_link_libraries(raylib_points_3d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_points_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Triangle example
add_executable(raylib_triangle_3d ${PF_ROOT_PATH}/examples/raylib/3d/raylib_triangle_3d.c)
target_link_libraries(raylib_triangle_3d PRIVATE pixelfactory raylib m)
target_include_directories(raylib_triangle_3d PRIVATE ${PF_ROOT_PATH}/src)
