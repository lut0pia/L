#pragma once

#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../macros.h"
#include "../math/Vector.h"
#include "Renderer.h"

namespace L {
  class Texture {
    L_NOCOPY(Texture)
  protected:
    uint32_t _width, _height;
    RenderFormat _format;
    TextureImpl* _impl;

  public:
    struct Intermediate {
      Array<Buffer> mips;
      uint32_t width, height;
      RenderFormat format;
    };
    Texture(const Intermediate& intermediate);
    Texture(uint32_t width, uint32_t height, RenderFormat format = RenderFormat::R8G8B8A8_UNorm, const void* data = nullptr, size_t size = 0);
    Texture(uint32_t width, uint32_t height, RenderFormat format, const void** data, size_t* size, uint32_t mips);
    ~Texture();
    inline void load(const void* data, size_t size) { load(data, size, {}, Vector3i{int32_t(_width), int32_t(_height), 1}); }
    void load(const void* data, size_t size, Vector3i offset, Vector3i extent);

    inline TextureImpl* get_impl() const { return _impl; }
    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline RenderFormat format() const { return _format; }

    static const Texture& black();

    friend inline void resource_write(Stream& s, const Intermediate& v) { s <= v.mips <= v.width <= v.height <= v.format; }
    friend inline void resource_read(Stream& s, Intermediate& v) { s >= v.mips >= v.width >= v.height >= v.format; }
  };
  size_t resource_gpu_size(const Texture::Intermediate& v);
}
