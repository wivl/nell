# nell

Nell is a toy-level, [Ray-Tracing-in-One-Weekend]()-style raytracer based on OpenGL. The whole project is made for learning purpose.


## Features

Nell has implemented a Ray-Tracing-in-One-Weekend-style raytracer in OpenGL, which has features:

* Lambertian, metal, dielectric and emit material
* Spheres and triangles intersection
* Mesh assets import
* Skybox
* Progressive rendering

## TODO

* Consider system design and remake
* Acceleration structure
* Path tracing

## Gallery

![bunny](readme-assets%2Fbunnies_256spp.png)

![cbox](readme-assets%2Fcbox_1000spp.png)

![chess](readme-assets%2Fchess.png)

![cornellbox](readme-assets%2Fcbox.png)

![material dielectric](readme-assets%2Fmaterial_dielectric_256spp.png)

![material lambertian](readme-assets%2Fmaterial_lambertian_256spp.png)

![material metal](readme-assets%2Fmaterial_metal_256spp.png)


## Build

> The code base is written on an x86-64 Windows 11 machine for MSVC compiler.

### Dependencies

* vcpkg
* glad
* glfw
* glm
* Assimp
* imGui
* stb

1. Install vcpkg:

```cmd
scoop bucket add main
scoop install main/vcpkg
```

2. Install dependencies:

```cmd
vcpkg install glad glfw3 glm assimp imgui stb
```

3. **Locate your toolchain**: <code>C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake</code>


4. **Edit it in the CMakeList.txt file:([more info](https://stackoverflow.com/a/64143507/20364686))

```cmake
include(C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake)
```

```cmd
mkdir build
cd build
cmake ..
```
