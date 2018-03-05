#pragma once

#include <L/src/L.h>
#define STB_IMAGE_IMPLEMENTATION 
#include <stb/stb_image.h>

namespace L {
  class STB_image : public Interface<Texture> {
    static STB_image instance;
    struct TaskPayload {
      int width, height, comp;
      byte* img;
      Ref<Texture> wtr;
    };
  public:
    inline STB_image() : Interface{"png","bmp","jpeg","jpg"} {}
    Ref<Texture> from(const byte* data, size_t size) override {
      TaskPayload payload;
      payload.img = stbi_load_from_memory(data, int(size), &payload.width, &payload.height, &payload.comp, 4);
      if(payload.img) {
        TaskSystem::push([](void* p) {
          TaskPayload& payload(*(TaskPayload*)p);
          payload.wtr = ref<Texture>(0, GL_RGBA8, payload.width, payload.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, payload.img);
          payload.wtr->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          payload.wtr->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          payload.wtr->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          payload.wtr->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          payload.wtr->generate_mipmap();
          stbi_image_free(payload.img);
        }, &payload, TaskSystem::MainThread);
        TaskSystem::join();
        return payload.wtr;
      } else {
        out << stbi_failure_reason() << '\n';
        return nullptr;
      }
    }
  };
  STB_image STB_image::instance;
}
