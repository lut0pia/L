#include "Font.h"

#include "PixelFont.h"
#include "../rendering/Program.h"
#include "../rendering/shader_lib.h"
#include "../text/encoding.h"

using namespace L;

const Font::Glyph& Font::glyph(uint32_t utf32) {
  Glyph* glyph;
  if(utf32<128) glyph = _ascii + utf32;
  else glyph = &_glyphs[utf32];
  if(!glyph->init) {
    load_glyph(utf32, *glyph, _bmp);
    glyph->atlas_coords = _atlas.add(_bmp);
    glyph->size.x() = _bmp.width();
    glyph->size.y() = _bmp.height();
    glyph->init = true;
  }
  return *glyph;
}
Program& glyphProgram() {
  static Program program(Shader(
    L_GLSL_INTRO
    L_SHAREDUNIFORM
    "layout (location = 0) in vec2 vertex;"
    "layout (location = 1) in vec2 texcoords;"
    "uniform vec2 position;"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = texcoords;"
    "gl_Position = vec4(((position+vertex)/screen.xy)*vec2(2.f/(viewport.z-viewport.x),-2.f/(viewport.w-viewport.y))-vec2(1.f,-1.f),0.f,1.f);"
    "}", GL_VERTEX_SHADER),
    Shader(
      L_GLSL_INTRO
      L_SHAREDUNIFORM
      "in vec2 ftexcoords;"
      "out vec4 fragcolor;"
      "uniform sampler2D atlas;"
      "void main(){"
      "vec4 color = texture(atlas,ftexcoords);"
      "fragcolor = color;"
      "}", GL_FRAGMENT_SHADER));
  return program;
}
Font::TextMesh& Font::text_mesh(const char* str) {
  const uint32_t h(hash(str));
  if(auto found = _text_meshes.find(h))
    return *found;
  else {
    update();
    TextMesh& wtr(_text_meshes[h]);
    wtr.str = str;
    Array<Vector4f> buffer;
    buffer.growTo(strlen(str)*6);
    wtr.dimensions.y() = _lineheight;
    int x(0), y(0);
    while(*str) {
      const uint32_t utf32(ReadUTF8(str));
      if(utf32=='\n') { // End line
        x = 0;
        y += _lineheight;
        wtr.dimensions.y() = max(wtr.dimensions.y(), y+_lineheight);
      } else { // Character
        const Glyph& g(glyph(utf32));
        const Vector4f tl(x+g.origin.x(), y+g.origin.y(), g.atlas_coords.min().x(), g.atlas_coords.min().y());
        const Vector4f tr(tl.x()+g.size.x(), tl.y(), g.atlas_coords.max().x(), tl.w());
        const Vector4f bl(tl.x(), tl.y()+g.size.y(), tl.z(), g.atlas_coords.max().y());
        const Vector4f br(tr.x(), bl.y(), tr.z(), bl.w());
        buffer.pushMultiple(tl, bl, br, tl, br, tr);
        x += g.advance;
        wtr.dimensions.x() = max(wtr.dimensions.x(), x);
      }
    }
    wtr.mesh.load(GL_TRIANGLES, buffer.size(), &buffer[0], sizeof(Vector4f)*buffer.size(), {
      Mesh::Attribute{2,GL_FLOAT,GL_FALSE},
      Mesh::Attribute{2,GL_FLOAT,GL_FALSE}
    });
    return wtr;
  }
}
void Font::draw(int x, int y, const char* str, Vector2f anchor) {
  Program& p(glyphProgram());
  TextMesh& tm(text_mesh(str));

  p.use();
  p.uniform("atlas", _atlas.texture());
  p.uniform("position", Vector2f(x-anchor.x()*tm.dimensions.x(), y-anchor.y()*tm.dimensions.y()));

  tm.mesh.draw();
  tm.last_used = Time::now();
}

void Font::update() {
  static const Time second(0, 0, 1);
  const Time now(Time::now());
  if(now-_last_update>second) {
    _last_update = now;
  } else return;

  static Array<uint32_t> obsoleteTextMeshes;
  obsoleteTextMeshes.clear();
  for(auto& textMesh : _text_meshes)
    if(now-textMesh.value().last_used>second) // Not used for a second
      obsoleteTextMeshes.push(textMesh.key());
  for(uint32_t obsoleteTextMesh : obsoleteTextMeshes)
    _text_meshes.remove(obsoleteTextMesh);
}

template<> void L::load_resource(ResourceSlot<Font>& slot) {
  if(*slot.path) slot.value = Interface<Font>::from_path(slot.path);
  else {
    static Ref<PixelFont> pixel_font(ref<PixelFont>());
    slot.persistent = true;
    slot.value = pixel_font;
  }
}
