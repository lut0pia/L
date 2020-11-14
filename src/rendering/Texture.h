#pragma once

#include "../container/Buffer.h"
#include "../macros.h"
#include "../math/Vector.h"
#include "Renderer.h"

namespace L {
  class Texture {
    L_NOCOPY(Texture)
  protected:
    uint32_t _width, _height, _layer_count;
    RenderFormat _format;
    TextureImpl* _impl;
  public:
    struct Intermediate {
      Buffer binary;
      uint32_t width, height;
      RenderFormat format;
    };
    inline Texture(const Intermediate& intermediate)
      : Texture(intermediate.width, intermediate.height, intermediate.format, intermediate.binary, intermediate.binary.size()) {
    }
    Texture(uint32_t width, uint32_t height, RenderFormat format = RenderFormat::R8G8B8A8_UNorm, const void* data = nullptr, size_t size = 0);
    ~Texture();
    inline void load(const void* data, size_t size) { load(data, size, {}, Vector3i {int32_t(_width), int32_t(_height), 1}); }
    void load(const void* data, size_t size, Vector3i offset, Vector3i extent);

    inline TextureImpl* get_impl() const { return _impl; }
    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline RenderFormat format() const { return _format; }

    static const Texture& black();

    friend inline void resource_write(Stream& s, const Intermediate& v) { s <= v.binary <= v.width <= v.height <= v.format; }
    friend inline void resource_read(Stream& s, Intermediate& v) { s >= v.binary >= v.width >= v.height >= v.format; }
  };
}
