#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.h>
#include <L/src/rendering/Texture.h>
#include <L/src/stream/CFileStream.h>

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

using namespace L;

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

void stb_image_module_init() {
  Resource<Texture>::add_loader("jpeg", stb_image_loader);
  Resource<Texture>::add_loader("jpg", stb_image_loader);
  Resource<Texture>::add_loader("png", stb_image_loader);
  Resource<Texture>::add_loader("bmp", stb_image_loader);
}
