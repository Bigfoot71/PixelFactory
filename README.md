![PixelFactory](examples/resources/images/PixelFactory.png)

# PixelFactory

PixelFactory is a lightweight software rendering library written in standard C99, designed for versatile rendering without any external dependencies beyond the C standard library.

## Features

- **Specialized in 2D and 3D Rendering**: PixelFactory excels in both 2D and 3D rendering.
- **Renderer System**: Manage rendering specifically for 2D or 3D, or even mix the two as needed.
- **Processors System**: Customize each rendering stage, from vertex transformation to clipping and fragment rendering, similar to shaders.
- **Bilinear Filtering**: Supports optional bilinear filtering, with the ability to define custom sampling functions.
- **Perspective Correction**: Applies perspective correction to texture coordinates during 3D rendering, customizable through processing functions.
- **Pixel Formats**: Supports various commonly used pixel formats and allows user-defined getter/setter functions for each texture.
- **Blend Modes**: Offers several blend modes for color blending, including addition, subtraction, multiplication, simple averaging, and alpha blending. Custom color blending functions can be provided via function pointers.
- **Depth Testing**: Toggle depth testing for 3D rendering management. Several basic depth testing functions are included, and custom functions can be provided via function pointers. The clear depth value is also adjustable.
- **Face Culling**: Supports face culling options such as back-face culling, front-face culling, and no culling.
- **OpenMP Support**: Utilizes OpenMP to parallelize triangle rasterization loops, with an adjustable threshold for activating parallelization, significantly improving rasterization performance for large triangles.
- **SIMD Support**: Some rendering tasks benefit from optional SSE/AVX2 support, with plans to expand SIMD compatibility in the future.

## Extensions

- **PF_EXT_TEXTURE2D**: Enables image loading via [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) (MIT) as well as the generation of basic textures like gradients or checkerboards.
- **PF_EXT_VERTEXBUFFER**: Enables mesh loading from OBJ files via [tinyobj_loader_c.h](https://github.com/syoyo/tinyobjloader-c/blob/master/tinyobj_loader_c.h) (MIT) and GLTF files via [cgltf.h](https://github.com/jkuhlmann/cgltf/blob/master/cgltf.h) (MIT), but also allows the generation of vertex buffers using [par_shapes.h](https://github.com/prideout/par/blob/master/par_shapes.h) (MIT).

**note**: The headers of the `external` directory are only used for library extensions.

## Why?

Previously, I worked on another software rendering library with an OpenGL-style API specialized in 3D rendering, [PixelForge](https://github.com/Bigfoot71/PixelForge). However, design choices quickly led to limitations that would require a complete redesign, also diverging from the OpenGL style because of my desires. I decided to start over with a new library, this time also specializing in 2D rendering, to create something lightweight and even more versatile. This project is primarily a hobby and a learning experience, but I hope it can be useful to others as well.

## Usage

### Prerequisites

Before you start, make sure you have [CMake](https://cmake.org/install/) and a C99-compatible compiler installed on your system.

### Installing PixelFactory

1. **Clone the repository:**

   ```bash
   git clone https://github.com/Bigfoot71/PixelFactory.git
   ```

2. **Navigate to the project directory:**

   ```bash
   cd PixelFactory
   ```

3. **Create a build directory:**

   ```bash
   mkdir build
   cd build
   ```

4. **Configure the project with CMake:**

   By default, CMake will generate the files needed to compile PixelFactory as a static library. You can customize the configuration by enabling or disabling certain options (such as OpenMP or AVX2) using CMake configuration options.

   For a basic (static) configuration, run:

   ```bash
   cmake ..
   ```

   To enable building a shared library, use:

   ```bash
   cmake -DPF_BUILD_SHARED=ON ..
   ```

   To enable extensions, use:
   ```bash
   cmake -DPF_EXT_TEXTURE2D=ON -DPF_EXT_VERTEXBUFFER=ON ..
   ```

5. **Build the project:**

   ```bash
   cmake --build .
   ```

   If you enabled installation, you can also install PixelFactory on your system:

   ```bash
   cmake --build . --target install
   ```

### Integrating into Your Project

1. **Include the PixelFactory header file in your source code:**

   ```c
   #include "pixelfactory/pf.h"
   ```

2. **Compile and link your project with the PixelFactory library.**

   Here is an example `CMakeLists.txt` file to integrate PixelFactory into your own project:

   ```cmake
   cmake_minimum_required(VERSION 3.21)
   project(myproject C)

   # Add the directory containing PixelFactory
   set(PIXELFACTORY_ROOT_PATH /path/to/PixelFactory)

   # Add the PixelFactory subdirectory
   add_subdirectory(${PIXELFACTORY_ROOT_PATH} ${CMAKE_BINARY_DIR}/pixelfactory)

   # Define your project's source files
   set(SOURCES
       src/main.c
       # Add other source files here
   )

   # Create the executable
   add_executable(myproject ${SOURCES})

   # Link your project with PixelFactory
   target_link_libraries(myproject PRIVATE PixelFactory)
   ```

   Replace `/path/to/PixelFactory` with the path to the directory where PixelFactory is located on your system.

### Examples

If you want to build the examples provided with PixelFactory, enable the corresponding option in CMake:

```bash
cmake -DPF_BUILD_EXAMPLES_RAYLIB=ON -DPF_BUILD_EXAMPLES_SDL2=ON ..
```

Then build the examples as described above.

## License

This library is released under the [Zlib License](LICENSE).

## Contribution

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## GIFs

![lighting_3d](examples/resources/gifs/raylib_lighting_3d.gif)
![lines_3d](examples/resources/gifs/raylib_lines_3d.gif)
![texture_2d](examples/resources/gifs/raylib_texture_2d.gif)
