#pragma once

#include "Vulkan.h"
#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../macros.h"
#include "../text/Symbol.h"

namespace L {
  class Shader {
    L_NOCOPY(Shader)
  public:
    enum BindingType {
      None,
      Uniform,
      UniformBlock,
      VertexAttribute,
    };
    struct Binding {
      Symbol name;
      int32_t offset, size, index, binding;
      BindingType type;
      VkFormat format;
      VkShaderStageFlags stage;
    };
  protected:
    VkShaderModule _module;
    VkShaderStageFlagBits _stage;
    Array<Binding> _bindings;
  public:
    inline Shader(const Buffer& buffer, VkShaderStageFlagBits stage) : Shader(buffer.data(), buffer.size(), stage) {}
    Shader(const void* binary, size_t size, VkShaderStageFlagBits stage);
    inline Shader(Shader&& other) : _module(other._module), _stage(other._stage) {
      other._module = VK_NULL_HANDLE;
    }
    ~Shader();

    inline void add_binding(Binding binding) { binding.stage = _stage; _bindings.push(binding); }
    inline const Array<Binding>& bindings() const { return _bindings; }

    inline VkShaderModule module() const { return _module; }
    inline VkShaderStageFlagBits stage() const { return _stage; }
  };
}
