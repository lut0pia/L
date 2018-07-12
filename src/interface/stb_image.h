#pragma once

#include <L/src/L.h>
#define STB_IMAGE_IMPLEMENTATION 
#include <stb/stb_image.h>

namespace L {
  void stb_image_loader(Resource<Texture>::Slot& slot) {
    Buffer buffer(slot.read_source_file());
    int width, height, comp;
    uint8_t* img(stbi_load_from_memory((const stbi_uc*)buffer.data(), int(buffer.size()), &width, &height, &comp, 4));
    if(img) {
      slot.value = ref<Texture>(width, height);
      slot.value->load(img);
      stbi_image_free(img);
    } else {
      out << stbi_failure_reason() << '\n';
    }
  }
}
