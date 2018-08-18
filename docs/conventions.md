# Conventions

## Mathematical and geometric conventions
- X is right, Y is forward, Z is up
- Matrices and Vectors are column-major

## Source conventions
- #pragma once (less name repetition and preprocessor pollution)
- 2-spaces indentation

## C++ code snippet
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
