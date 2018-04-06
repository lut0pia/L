#pragma once

#include <L/src/L.h>
#define STB_IMAGE_IMPLEMENTATION 
#include <stb/stb_image.h>

namespace L {
  class STB_image : public Interface<Texture> {
    static STB_image instance;
  public:
    inline STB_image() : Interface{"png","bmp","jpeg","jpg"} {}
    Ref<Texture> from(const byte* data, size_t size) override {
      int width, height, comp;
      byte* img(stbi_load_from_memory(data, int(size), &width, &height, &comp, 4));
      if(img) {
        TaskSystem::change_thread_mask(1); // Go to main thread
        Ref<Texture> wtr(ref<Texture>(0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img));
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
