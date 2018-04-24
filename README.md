# L Engine

## About
The L game engine is made partly for education purposes and for technological independence.
It will try to stay as simple as possible and avoid any dependencies.

## Building

### Windows
Simply run [premake](https://github.com/premake/premake-core/wiki/Using-Premake) with the action you want,
then open the solution file and build from there.

### Unix
You can either run the [premake](https://github.com/premake/premake-core/wiki/Using-Premake) and build from there, or you can run the run script that will run the premake for gmake, build the engine and run the test project.

## Dependencies
- Premake (binaries supplied)
- OpenGL 3.0

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

## Interfaces included
Interfaces are small binding files (found in src/interface) that register in/out functions to the engine for it to be able to read/write files.
They are not part of the engine building process as they are not needed by the engine itself and do not constitute dependencies.
- stb_image
- stb_truetype
- stb_vorbis
- JSON
- Wavefront OBJ
- FreeType 2
- Midi
- WAVE
