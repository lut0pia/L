#include "Resource.h"

#include "../image/Bitmap.h"
#include "../font/PixelFont.h"

using namespace L;

template<> Ref<Script::CodeFunction> L::load_resource(const char* path, ResourceSettings& settings) {
  CFileStream fs(path, "r");
  if(fs) return ref<Script::CodeFunction>(Script::Context::read(fs));
  return nullptr;
}
template<> Ref<Font> L::load_resource(const char* path, ResourceSettings& settings) {
  if(*path) return Interface<Font>::from_path(path);
  else {
    static Ref<PixelFont> pixel_font(ref<PixelFont>());
    settings.persistent = true;
    return pixel_font;
  }
}
