#pragma once

#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../macros.h"
#include "../text/Symbol.h"
#include "Vulkan.h"

namespace L {
  class Shader {
    L_NOCOPY(Shader)
  public:
    enum BindingType {
      None,
      UniformConstant,
      Uniform,
      Input,
      PushConstant,
    };
    struct Binding {
      Symbol name;
      int32_t offset, size, index, binding;
      BindingType type;
      VkFormat format;
      VkShaderStageFlags stage;
    };
    struct Intermediate {
      Buffer binary;
      VkShaderStageFlagBits stage;
      Array<Binding> bindings;
    };
  protected:
    VkShaderModule _module;
    VkShaderStageFlagBits _stage;
    Array<Binding> _bindings;
  public:
    Shader(const Intermediate& intermediate);
    ~Shader();

    inline VkShaderModule module() const { return _module; }
    inline VkShaderStageFlagBits stage() const { return _stage; }
    inline const Array<Binding>& bindings() const { return _bindings; }

    static void reflect(Intermediate& intermediate);

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.binary <= v.stage <= v.bindings; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.binary >= v.stage >= v.bindings; }
    friend inline Stream& operator<=(Stream& s, const Binding& v) { return s <= v.name <= v.offset <= v.size <= v.index <= v.binding <= v.type <= v.format <= v.stage; }
    friend inline Stream& operator>=(Stream& s, Binding& v) { return s >= v.name >= v.offset >= v.size >= v.index >= v.binding >= v.type >= v.format >= v.stage; }
  };
}
