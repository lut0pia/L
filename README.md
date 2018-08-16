# L Engine

## About
The L game engine is made partly for education purposes and for technological independence.
It will try to stay as simple as possible and avoid any dependencies.

## Building

### Windows
Simply run [premake](https://github.com/premake/premake-core/wiki/Using-Premake) with the action you want,
then open the solution file and build from there. Alternatively `run.sh` should do that for you.

### Unix
You can either run the [premake](https://github.com/premake/premake-core/wiki/Using-Premake) and build from there, or you can run the `run.sh [dbg|dev|rls] [run]` commandline that will fetch premake, build it, build the engine and sample (in configuration debug, development or release) and then optionally run the sample. All this is abstracted away by the VSCode settings if you're willing to use VSCode.

## Dependencies
- Premake (binaries supplied)
- Vulkan

### Windows
- WinAPI

### Unix
- Xlib

## Conventions
- #pragma once (less name repetition and preprocessor pollution)
- 2-spaces indentation
- X is right, Y is forward, Z is up
- Matrices and Vectors are column-major
``` C++
#pragma once

#define L_MACRO(x) ((x)*(x))

namespace L {
  enum EnumType {
    OnePossibility,
    AnotherThing,
  };
  class ClassName {
  protected:
    float _attribute_name;
  public:
    void method_name(int p1, float p2) {
      int var_name(4);
      return function_name(p2, var_name, p1);
    }
  };
}
```

## Modules
Modules, found in `src/module`, are subparts of the engine that are not relied on by the core engine, therefore they do not constitute dependencies.

### Loaders
Loaders are modules that register functions to the engine for it to be able to create resources from files or even just a single url. Here's a list of asset loaders already included:
- stb_image
- stb_truetype
- stb_vorbis
- JSON
- Wavefront OBJ
- Midi
- WAVE
