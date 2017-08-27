#include "Resource.h"

#include "../image/Bitmap.h"
#include "../font/PixelFont.h"

using namespace L;

template <> Ref<GL::Texture> L::load_resource(const char* fp) {
  if(auto bmp = Interface<Bitmap>::from_file(fp))
    return ref<GL::Texture>(*bmp, true);
  return nullptr;
}
template <> Ref<Script::CodeFunction> L::load_resource(const char* fp) {
  CFileStream fs(fp, "r");
  if(fs) return ref<Script::CodeFunction>(Script::Context::read(fs));
  else return nullptr;
}
template <> Ref<Font> L::load_resource(const char* fp) {
  if(*fp) return Interface<Font>::from_file(fp);
  else return ref<PixelFont>();
}
