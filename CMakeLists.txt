cmake_minimum_required(VERSION 3.21)
project(pixelfactory)

# C standard definition
set(CMAKE_C_STANDARD 99)

# Definitions of construction constants
set(PF_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(PF_IS_MAIN ${PROJECT_IS_TOP_LEVEL})

# Set build options
option(PF_INSTALL "Install PixelFactory library and headers" OFF)
option(PF_BUILD_SHARED "Build PixelFactory as a shared library" OFF)

# Set extensions
option(PF_EXT_TEXTURE2D "Enables the texture2d extension allowing images loading via stbi and generation" OFF)

# Set example builds
option(PF_BUILD_EXAMPLES_RAYLIB "Build PixelFactory examples for raylib" OFF)

# Defining source files
file(GLOB_RECURSE SRCS ${PF_ROOT_PATH}/src/*.c)
file(GLOB HDRS ${PF_ROOT_PATH}/include/*.h)

# Common compilation options
add_compile_options(-Wall -Wextra -pedantic)

# Adding source files to the target based on the chosen option (shared or static)
if(PF_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDRS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC PF_BUILD_SHARED)
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)   # Add -fPIC
else()
    add_library(${PROJECT_NAME} STATIC ${SRCS} ${HDRS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC PF_BUILD_STATIC)
endif()

# Define PF_USE_SHARED if the option is enabled
if(PF_BUILD_SHARED AND NOT PF_IS_MAIN)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PF_USE_SHARED)
endif()

# Specify the include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${PF_ROOT_PATH}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PF_ROOT_PATH}/external)

# Activate extensions if needed
if(PF_EXT_TEXTURE2D)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PF_EXT_TEXTURE2D)
endif()
if(PF_EXT_VERTEXBUFFER)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PF_EXT_VERTEXBUFFER)
endif()

# Check for SIMD support
include(CheckCCompilerFlag)
check_c_compiler_flag(-mavx2 HAVE_AVX2)
if(HAVE_AVX2)
    message(STATUS "AVX2 support detected")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
    add_definitions(-DPF_SUPPORT_AVX2)
else()
    # Check for SSE4.1 support
    check_c_compiler_flag(-msse4.1 HAVE_SSE4_1)
    if(HAVE_SSE4_1)
        message(STATUS "SSE4.1 support detected")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4")
        add_definitions(-DPF_SUPPORT_SSE4)
    else()
        # Check for SSE2 support
        check_c_compiler_flag(-msse2 HAVE_SSE2)
        if(HAVE_SSE2)
            message(STATUS "SSE2 support detected")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
            add_definitions(-DPF_SUPPORT_SSE2)
        endif()
    endif()
endif()

# Enable OpenMP support if available
find_package(OpenMP)
if (OPENMP_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_C)
    add_definitions(-DPF_SUPPORT_OPENMP)
endif()

# Add the -pg for Debug mode (gprof) only for GCC
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

# Installation if option is enabled
if(PF_INSTALL)
    # Installing the library and headers
    install(TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION /usr/local/lib
            LIBRARY DESTINATION /usr/local/lib
    )
    install(FILES ${HDRS} DESTINATION /usr/local/include)
endif()

# Examples
include(examples/CMakeLists.txt)
