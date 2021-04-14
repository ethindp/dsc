# The Data Structure Analyzer
An application to help teach data structures to computer science students

## What is this?

This tool is an application that helps teach data structures to computer science students. It is usable by both individuals with and without disabilities.

### Warning

This tool is in alpha status.

## Dependencies

This tool uses [vcpkg](https://github.com/microsoft/vcpkg) for reproduceable builds. This eliminates package management hell. As such, you must set up vcpkg as outlined [here](https://docs.microsoft.com/en-us/cpp/build/install-vcpkg). Once done, install the following packages at minimum:

* `sdl2[core]`
* `fmt[core]`
* `freetype[core]`
* `plog[core]`
* `glad[core]`

Some of the above dependencies have optional extras. These extras may or may not have any benefit when building the DSA. At the time of writing, these available extras are:

* `sdl2[vulkan]`: enables [Vulkan](https://www.khronos.org/vulkan) support for SDL2
* `freetype[brotli]`: enables the ability to decompress [Web Open Font Format (WOFF) 2.0](https://www.w3.org/TR/WOFF2) streams compressed with the [brotli](https://github.com/google/brotli) compression algorithm
* `freetype[bzip2]`: Enables decompression of fonts compressed with the BZIP2 compression algorithm
* `freetype[png]`: Enables use of PNG compressed OpenType embedded bitmaps
* `freetype[zlib]`: Force freetype to use the zlib library instead of internal library
* `glad[egl]`: Use the EGL specification
* `glad[extensions]`: Enables all extensions
* `glad[gl-api-{10|11|12|13|14|15|20|21|30|31|32|33|40|41|42|43|44|45|46|latest}]`: Import extensions from the OpenGL 1.0-1.5, 2.0-2.1, 3.0-3.3, 4.0-4.6, and/or latest specifications (allows individua extension version selection)
* `glad[gles1-api-{10|latest}]`: Imports extensions from the OpenGL ES 1.0 and/or latest specifications (allows individual extension version selection)
* `glad[gles2-api-{20|30|31|32|latest}]`: Imports extensions from the OpenGL ES 2 2.0-2.1, 3.0-3.2  and/or latest specifications (allows individual extension version selection)
* `glad[glsc2-api-{20|latest}]`: Imports extensions from the OpenGL SC 2 2.0 and/or latest specifications (allows individual extension version selection)
* `glad[glx|wgl]`: forces glad to use GLX (only available on Linux) or WGL (only available on Windows) for extensions
* `glad[loader]`: Generate loader logic

Note: the majority of options are not needed here. For glad, it is strongly recommended that you install the core and extensions modules, and do not select a loader or specification version unless your building for a special configuration. The vulkan extension for SDL2 is currently unused by the DSA and therefore will provide no benefit. The modules for freetype will allow loading of fonts that are compressed in various formats; if you would like the widest array of these, it is strongly encouraged that you enable everything.

To install the aforementioned recommended configuration, run the command:

```none
vcpkg install --triplet x64-windows sdl2 fmt freetype[*] plog glad[extensions]
```

## Building

1. Download and set up Visual Studio 2019 with the C++ workload. Be sure to also install cmake.
2. Set up vcpkg as described above.
3. Install the dependencies above.
4. Clone this repository.
5. Run cmake and set the `CMAKE_TOOLCHAIN_FILE` definition to `$VCPKGROOT/scripts/buildsystems/vcpkg.cmake`, where `$VCPKGROOT` is the directory where you downloaded vcpkg. For windows, also set the generator to `NMake Makefiles`. Alternatively, use `cmake-gui`.
6. Run `nmake`/`make`.
