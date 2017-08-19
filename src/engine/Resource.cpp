#include "Resource.h"

#include "../image/Bitmap.h"
#include "../font/PixelFont.h"

using namespace L;

template <> Ref<GL::Texture> L::load_resource(const char* fp) {
  if(auto bmp = Interface<Bitmap>::fromFile(fp))
    return ref<GL::Texture>(*bmp, true);
  return nullptr;
}
template <> Ref<Script::CodeFunction> L::load_resource(const char* fp) {
  return ref<Script::CodeFunction>(Script::Context::read(CFileStream(fp, "r")));
}
template <> Ref<Font> L::load_resource(const char* fp) {
  if(*fp) return Interface<Font>::fromFile(fp);
  else return ref<PixelFont>();
}
