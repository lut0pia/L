#include "Resource.h"

#include "../image/Bitmap.h"
#include "../font/PixelFont.h"

using namespace L;

template<> Ref<Font> L::load_resource(const char* path, ResourceSettings& settings) {
  if(*path) return Interface<Font>::from_path(path);
  else {
    static Ref<PixelFont> pixel_font(ref<PixelFont>());
    settings.persistent = true;
    return pixel_font;
  }
}
