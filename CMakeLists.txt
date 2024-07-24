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

# Set optional supports
option(PF_SUPPORT_AVX2 "Enable support for AVX2" OFF)
option(PF_SUPPORT_OPENMP "Enable support for OpenMP parallelization" OFF)

# Set example builds
option(PF_BUILD_EXAMPLES_RAYLIB "Build PixelFactory examples for raylib" OFF)

# Defining source files
file(GLOB_RECURSE SRCS ${PF_ROOT_PATH}/src/*.c)
file(GLOB HDRS ${PF_ROOT_PATH}/src/*.h)

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

# Specify the include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Enable AVX2 support if PF_SUPPORT_AVX2 is ON
if(PF_SUPPORT_AVX2)
    message(STATUS "AVX2 support enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PF_SUPPORT_AVX2)
endif()

# Enable OpenMP support if PF_SUPPORT_OPENMP is ON
if(PF_SUPPORT_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_C)
        target_compile_definitions(${PROJECT_NAME} PUBLIC PF_SUPPORT_OPENMP)
    endif()
endif()

# TEMP !!! Add the -pg flag for Debug mode (gprof)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

# Installation if option is enabled
if(PF_INSTALL)
    # Installing the library and headers
    install(TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION /usr/local/lib
            LIBRARY DESTINATION /usr/local/lib
            RUNTIME DESTINATION /usr/local/bin
    )
    install(FILES ${HDRS} DESTINATION /usr/local/include)
endif()

# Examples
include(examples/CMakeLists.txt)