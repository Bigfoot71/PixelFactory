# CMakeLists.txt for PixelFactory examples

if (PF_BUILD_EXAMPLES_RAYLIB)
    add_subdirectory(${PF_ROOT_PATH}/examples/raylib)
endif()

if (PF_BUILD_EXAMPLES_SDL2)
    add_subdirectory(${PF_ROOT_PATH}/examples/SDL2)
endif()
