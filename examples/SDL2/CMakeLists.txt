cmake_minimum_required(VERSION 3.21)

# Path to example resources
add_definitions(-DRESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../resources/")

# 2D - Circle example
add_executable(SDL2_circle_2d ${PF_ROOT_PATH}/examples/SDL2/2d/SDL2_circle_2d.c)
target_link_libraries(SDL2_circle_2d PRIVATE pixelfactory SDL2 m)
target_include_directories(SDL2_circle_2d PRIVATE ${PF_ROOT_PATH}/src)

# 2D - Lines example
add_executable(SDL2_lines_2d ${PF_ROOT_PATH}/examples/SDL2/2d/SDL2_lines_2d.c)
target_link_libraries(SDL2_lines_2d PRIVATE pixelfactory SDL2 m)
target_include_directories(SDL2_lines_2d PRIVATE ${PF_ROOT_PATH}/src)

# 3D - Triangle example
add_executable(SDL2_triangle_3d ${PF_ROOT_PATH}/examples/SDL2/3d/SDL2_triangle_3d.c)
target_link_libraries(SDL2_triangle_3d PRIVATE pixelfactory SDL2 m)
target_include_directories(SDL2_triangle_3d PRIVATE ${PF_ROOT_PATH}/src)
