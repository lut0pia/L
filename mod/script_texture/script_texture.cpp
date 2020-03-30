#include <L/src/container/Handle.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Color.h>
#include <L/src/rendering/Texture.h>
#include <L/src/script/ScriptContext.h>
#include <L/src/script/script_binding.h>

using namespace L;

class ScriptTexture : public Handled<ScriptTexture> {
protected:
  Texture::Intermediate& _texture;
public:
  inline ScriptTexture(Texture::Intermediate& intermediate)
    : Handled<ScriptTexture>(this), _texture(intermediate) {}

  void create(float width, float height);
  void set_pixel(float x, float y, const Color& c);
  void rect(float x, float y, float w, float h, const Color& c);

protected:
  inline void set_pixel_internal(uintptr_t x, uintptr_t y, const Color& c) {
    *(Color*)_texture.binary.data((x + y * _texture.width) * sizeof(Color)) = c;
  }
};

void ScriptTexture::create(float fwidth, float fheight) {
  _texture.width = uint32_t(fwidth);
  _texture.height = uint32_t(fheight);
  _texture.binary = Buffer(_texture.width*_texture.height * 4);
  _texture.format = VK_FORMAT_R8G8B8A8_UNORM;
  memset(_texture.binary.data(), 0, _texture.binary.size());
}
void ScriptTexture::set_pixel(float x, float y, const Color& c) {
  set_pixel_internal(uintptr_t(x), uintptr_t(y), c);
}
void ScriptTexture::rect(float x, float y, float w, float h, const Color& c) {
  const uintptr_t x0 = uintptr_t(x);
  const uintptr_t y0 = uintptr_t(y);
  const uintptr_t x1 = uintptr_t(x + w);
  const uintptr_t y1 = uintptr_t(y + h);

  for(uintptr_t i = x0; i < x1; i++) {
    for(uintptr_t j = y0; j < y1; j++) {
      set_pixel_internal(i, j, c);
    }
  }
}

bool script_texture_loader(ResourceSlot& slot, Texture::Intermediate& intermediate) {
  ScriptFunction::Intermediate script_intermediate;
  if(ResourceLoading<ScriptFunction>::load_internal(slot, script_intermediate)) {
    ScriptTexture script_texture(intermediate);
    ScriptContext context(script_texture.handle());
    context.execute(ref<ScriptFunction>(script_intermediate));
    return true;
  }
  return false;
}

void script_texture_module_init() {
  ResourceLoading<Texture>::add_loader(script_texture_loader);

  L_SCRIPT_METHOD(ScriptTexture, "create", 2, create(c.param(0), c.param(1)));
  L_SCRIPT_METHOD(ScriptTexture, "set_pixel", 3, set_pixel(c.param(0), c.param(1), c.param(2)));
  L_SCRIPT_METHOD(ScriptTexture, "rect", 5, rect(c.param(0), c.param(1), c.param(2), c.param(3), c.param(4)));
}
