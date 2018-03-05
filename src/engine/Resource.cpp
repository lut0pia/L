#include "Resource.h"

#include "../image/Bitmap.h"
#include "../font/PixelFont.h"

using namespace L;

template<> void L::load_resource(ResourceSlot<Font>& slot) {
  if(*slot.path) slot.value = Interface<Font>::from_path(slot.path);
  else {
    static Ref<PixelFont> pixel_font(ref<PixelFont>());
    slot.persistent = true;
    slot.value = pixel_font;
  }
}
