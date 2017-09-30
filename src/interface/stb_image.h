#pragma once

#include <L/src/L.h>
#define STB_IMAGE_IMPLEMENTATION 
#include <stb/stb_image.h>

namespace L {
  class STB_image : public Interface<GL::Texture> {
    static STB_image instance;
  public:
    inline STB_image() : Interface{"png","bmp","jpeg","jpg"} {}
    Ref<GL::Texture> from(const byte* data, size_t size) override {
      int width, height, comp;
      byte* img(stbi_load_from_memory(data, int(size), &width, &height, &comp, 4));
      if(img) {
        auto wtr = ref<GL::Texture>(0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        wtr->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        wtr->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        wtr->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        wtr->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        wtr->generate_mipmap();
        stbi_image_free(img);
        return wtr;
      } else {
        out << stbi_failure_reason() << '\n';
        return nullptr;
      }
    }
  };
  STB_image STB_image::instance;
}
