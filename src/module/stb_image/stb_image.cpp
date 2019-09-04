#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Texture.h>
#include <L/src/stream/CFileStream.h>

L_PUSH_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

L_POP_NO_WARNINGS

using namespace L;

static const Symbol bmp_symbol("bmp"), jpeg_symbol("jpeg"), jpg_symbol("jpg"), png_symbol("png"), tga_symbol("tga");

bool stb_image_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  if(slot.ext != bmp_symbol && slot.ext != jpeg_symbol && slot.ext != jpg_symbol && slot.ext != png_symbol && slot.ext != tga_symbol) {
    return false;
  }

  int width, height, components;
  uint8_t* img;
  {
    Buffer buffer(slot.read_source_file());
    L_SCOPE_MARKER("stbi_load_from_memory");
    img = stbi_load_from_memory((const stbi_uc*)buffer.data(), int(buffer.size()), &width, &height, &components, 4);
  }
  if(img) {
    intermediate.width = width;
    intermediate.height = height;
    intermediate.format = VK_FORMAT_R8G8B8A8_UNORM;
    intermediate.binary = Buffer(img, width * height * 4);
    stbi_image_free(img);
    return true;
  } else {
    out << stbi_failure_reason() << '\n';
    return false;
  }
}

void stb_image_module_init() {
  ResourceLoading<Texture>::add_loader(stb_image_loader);
}
