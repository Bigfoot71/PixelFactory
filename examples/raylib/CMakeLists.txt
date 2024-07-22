cmake_minimum_required(VERSION 3.21)

# Path to example resources
add_definitions(-DRESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../resources/")

# Circle example
add_executable(circle ${PF_ROOT_PATH}/examples/raylib/circle.c)
target_link_libraries(circle PRIVATE pixelfactory raylib m)
target_include_directories(circle PRIVATE ${PF_ROOT_PATH}/src)

# Framebuffer example
add_executable(framebuffer ${PF_ROOT_PATH}/examples/raylib/framebuffer.c)
target_link_libraries(framebuffer PRIVATE pixelfactory raylib m)
target_include_directories(framebuffer PRIVATE ${PF_ROOT_PATH}/src)

# Lines example
add_executable(lines ${PF_ROOT_PATH}/examples/raylib/lines.c)
target_link_libraries(lines PRIVATE pixelfactory raylib m)
target_include_directories(lines PRIVATE ${PF_ROOT_PATH}/src)

# Mesh2D example
add_executable(mesh2d ${PF_ROOT_PATH}/examples/raylib/mesh2d.c)
target_link_libraries(mesh2d PRIVATE pixelfactory raylib m)
target_include_directories(mesh2d PRIVATE ${PF_ROOT_PATH}/src)

# Rectangle example
add_executable(rect ${PF_ROOT_PATH}/examples/raylib/rect.c)
target_link_libraries(rect PRIVATE pixelfactory raylib m)
target_include_directories(rect PRIVATE ${PF_ROOT_PATH}/src)

# Texture2D example
add_executable(texture2d ${PF_ROOT_PATH}/examples/raylib/texture2d.c)
target_link_libraries(texture2d PRIVATE pixelfactory raylib m)
target_include_directories(texture2d PRIVATE ${PF_ROOT_PATH}/src)

# Triangle example
add_executable(triangle ${PF_ROOT_PATH}/examples/raylib/triangle.c)
target_link_libraries(triangle PRIVATE pixelfactory raylib m)
target_include_directories(triangle PRIVATE ${PF_ROOT_PATH}/src)
