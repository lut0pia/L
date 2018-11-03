#pragma once

#include "Vulkan.h"
#include "Pipeline.h"
#include "GPUBuffer.h"

namespace L {
  class DescriptorSet {
    L_NOCOPY(DescriptorSet)
  protected:
    const Pipeline& _pipeline;
    VkDescriptorSet _set;
    Array<GPUBuffer> _buffers;
  public:
    DescriptorSet(const Pipeline& pipeline);
    ~DescriptorSet();

    bool set_descriptor(const Symbol& name, VkDescriptorBufferInfo buffer_info);
    bool set_descriptor(const Symbol& name, VkDescriptorImageInfo image_info);
    bool set_value(const Symbol& name, const void* data, size_t size);
    template <class T> bool set_value(const Symbol& name, const T& value) { return set_value(name, &value, sizeof(value)); }

    inline const VkDescriptorSet& set() const { return _set; }
    inline VkDescriptorSetLayout layout() const { return _pipeline.desc_set_layout(); }
  };
}
