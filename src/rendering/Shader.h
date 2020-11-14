#pragma once

#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../macros.h"
#include "../text/Symbol.h"
#include "Renderer.h"

namespace L {
  class Shader {
    L_NOCOPY(Shader)
  public:
    struct Intermediate {
      Buffer binary;
      ShaderStage stage;
      Array<ShaderBinding> bindings;
    };
  protected:
    ShaderImpl* _impl;
    ShaderStage _stage;
    Array<ShaderBinding> _bindings;
  public:
    Shader(const Intermediate& intermediate);
    ~Shader();

    inline ShaderImpl* get_impl() const { return _impl; }
    inline ShaderStage stage() const { return _stage; }
    inline const Array<ShaderBinding>& bindings() const { return _bindings; }

    friend inline void resource_write(Stream& s, const Intermediate& v) { s <= v.binary <= v.stage <= v.bindings; }
    friend inline void resource_read(Stream& s, Intermediate& v) { s >= v.binary >= v.stage >= v.bindings; }
  };
}
