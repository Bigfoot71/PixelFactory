cmake_minimum_required(VERSION 3.21)

# Path to example resources
add_definitions(-DRESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../resources/")

# 2D - Circle example
add_executable(circle_2d ${PF_ROOT_PATH}/examples/raylib/2d/circle.c)
target_link_libraries(circle_2d PRIVATE pixelfactory raylib m)
target_include_directories(circle_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Framebuffer example
add_executable(framebuffer ${PF_ROOT_PATH}/examples/raylib/2d/framebuffer.c)
target_link_libraries(framebuffer PRIVATE pixelfactory raylib m)
target_include_directories(framebuffer PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Lines example
add_executable(lines_2d ${PF_ROOT_PATH}/examples/raylib/2d/lines.c)
target_link_libraries(lines_2d PRIVATE pixelfactory raylib m)
target_include_directories(lines_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Mesh2D example
add_executable(mesh_2d ${PF_ROOT_PATH}/examples/raylib/2d/mesh2d.c)
target_link_libraries(mesh_2d PRIVATE pixelfactory raylib m)
target_include_directories(mesh_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Rectangle example
add_executable(rect_2d ${PF_ROOT_PATH}/examples/raylib/2d/rect.c)
target_link_libraries(rect_2d PRIVATE pixelfactory raylib m)
target_include_directories(rect_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Texture2D example
add_executable(texture_2d ${PF_ROOT_PATH}/examples/raylib/2d/texture2d.c)
target_link_libraries(texture_2d PRIVATE pixelfactory raylib m)
target_include_directories(texture_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Triangle example
add_executable(triangle_2d ${PF_ROOT_PATH}/examples/raylib/2d/triangle.c)
target_link_libraries(triangle_2d PRIVATE pixelfactory raylib m)
target_include_directories(triangle_2d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Line example
add_executable(lines_3d ${PF_ROOT_PATH}/examples/raylib/3d/lines.c)
target_link_libraries(lines_3d PRIVATE pixelfactory raylib m)
target_include_directories(lines_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Triangle example
add_executable(triangle_3d ${PF_ROOT_PATH}/examples/raylib/3d/triangle.c)
target_link_libraries(triangle_3d PRIVATE pixelfactory raylib m)
target_include_directories(triangle_3d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Model example
add_executable(model_3d ${PF_ROOT_PATH}/examples/raylib/3d/model.c)
target_link_libraries(model_3d PRIVATE pixelfactory raylib m)
target_include_directories(model_3d PRIVATE ${PF_ROOT_PATH}/src)
