#include "Font.h"

#include "../text/encoding.h"
#include "../engine/SharedUniform.h"

using namespace L;

const Font::Glyph& Font::glyph(uint32_t utf32) {
  Glyph* glyph;
  if(utf32<128) glyph = _ascii + utf32;
  else glyph = &_glyphs[utf32];
  if(glyph->bmp.empty()) {
    *glyph = loadGlyph(utf32);
    glyph->atlasCoords = _atlas.add(glyph->bmp);
  }
  return *glyph;
}
GL::Program& glyphProgram() {
  static GL::Program program(GL::Shader(
    "#version 330 core\n"
    L_SHAREDUNIFORM
    "layout (location = 0) in vec2 vertex;"
    "layout (location = 1) in vec2 texcoords;"
    "uniform vec2 position;"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = texcoords;"
    "gl_Position = vec4(((position+vertex)/screen.xy)*vec2(2.f/(viewport.z-viewport.x),-2.f/(viewport.w-viewport.y))-vec2(1.f,-1.f),0.f,1.f);"
    "}", GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
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
Font::TextMesh& Font::textMesh(const char* str) {
  const uint32_t h(hash(str));
  if(auto found = _textMeshes.find(h))
    return *found;
  else {
    TextMesh& wtr(_textMeshes[h]);
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
        const Vector4f tl(x+g.origin.x(), y+g.origin.y(), g.atlasCoords.min().x(), g.atlasCoords.min().y());
        const Vector4f tr(tl.x()+g.bmp.width(), tl.y(), g.atlasCoords.max().x(), tl.w());
        const Vector4f bl(tl.x(), tl.y()+g.bmp.height(), tl.z(), g.atlasCoords.max().y());
        const Vector4f br(tr.x(), bl.y(), tr.z(), bl.w());
        buffer.pushMultiple(tl, bl, br, tl, br, tr);
        x += g.advance;
        wtr.dimensions.x() = max(wtr.dimensions.x(), x);
      }
    }
    wtr.mesh.load(GL_TRIANGLES, buffer.size(), &buffer[0], sizeof(Vector4f)*buffer.size(), {
      GL::Mesh::Attribute{0,2,GL_FLOAT,GL_FALSE,sizeof(Vector4f),0},
      GL::Mesh::Attribute{1,2,GL_FLOAT,GL_FALSE,sizeof(Vector4f),sizeof(float)*2}
    });
    return wtr;
  }
}
void Font::draw(int x, int y, const char* str, Vector2f anchor) {
  GL::Program& p(glyphProgram());
  TextMesh& tm(textMesh(str));

  p.use();
  p.uniform("atlas", _atlas.texture());
  p.uniform("position", Vector2f(x-anchor.x()*tm.dimensions.x(), y-anchor.y()*tm.dimensions.y()));

  tm.mesh.draw();
  tm.lastUsed = Time::now();
}

void Font::updateTextMeshes() {
  static Array<uint32_t> obsoleteTextMeshes;
  obsoleteTextMeshes.clear();
  for(auto& textMesh : _textMeshes)
    if(textMesh.value().lastUsed<Time::now()-Time(0, 0, 1)) // Not used for a second
      obsoleteTextMeshes.push(textMesh.key());
  for(uint32_t obsoleteTextMesh : obsoleteTextMeshes)
    _textMeshes.remove(obsoleteTextMesh);
}
