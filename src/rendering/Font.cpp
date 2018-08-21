#include "Font.h"

#include "../engine/Resource.inl"
#include "../rendering/Pipeline.h"
#include "../rendering/shader_lib.h"
#include "../text/encoding.h"
#include "../rendering/DescriptorSet.h"

using namespace L;

Resource<Pipeline> Font::_pipeline;

const Font::Glyph& Font::glyph(uint32_t utf32) {
  Glyph* glyph;
  if(utf32<128) glyph = _ascii + utf32;
  else glyph = &_glyphs[utf32];
  if(!glyph->init) {
    load_glyph(utf32, *glyph, _bmp);
    if(_bmp.width() && _bmp.height()) {
      glyph->atlas_coords = _atlas.add(_bmp.width(), _bmp.height(), &_bmp[0]);
    }
    glyph->size.x() = _bmp.width();
    glyph->size.y() = _bmp.height();
    glyph->init = true;
  }
  return *glyph;
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
      const uint32_t utf32(utf8_to_utf32(str));
      if(utf32=='\n') { // End line
        x = 0;
        y += _lineheight;
        wtr.dimensions.y() = max(wtr.dimensions.y(), y+_lineheight);
      } else { // Character
        const Glyph& g(glyph(utf32));
        if(g.size.x() && g.size.y()) {
          const Vector4f tl(x+g.origin.x(), y+g.origin.y(), g.atlas_coords.min().x(), g.atlas_coords.min().y());
          const Vector4f tr(tl.x()+g.size.x(), tl.y(), g.atlas_coords.max().x(), tl.w());
          const Vector4f bl(tl.x(), tl.y()+g.size.y(), tl.z(), g.atlas_coords.max().y());
          const Vector4f br(tr.x(), bl.y(), tr.z(), bl.w());
          buffer.pushMultiple(tl, bl, br, tl, br, tr);
        }
        x += g.advance;
        wtr.dimensions.x() = max(wtr.dimensions.x(), x);
      }
    }
    static const VkFormat formats[] {
      VK_FORMAT_R32G32_SFLOAT,
      VK_FORMAT_R32G32_SFLOAT,
    };
    wtr.mesh.load(buffer.size(), &buffer[0], sizeof(Vector4f)*buffer.size(), formats, L_COUNT_OF(formats));
    return wtr;
  }
}
void Font::draw(VkCommandBuffer cmd_buffer, const Matrix44f& model, const char* str, Vector2f anchor) {
  if(!*str) {
    warning("Attempting to draw emptry string");
    return;
  }
  if(!_pipeline) {
    return;
  }
  vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline);
  vkCmdPushConstants(cmd_buffer, *_pipeline, _pipeline->find_binding("Constants")->stage, 0, sizeof(model), &model);

  DescriptorSet desc_set(*_pipeline);
  desc_set.set_descriptor("tex", VkDescriptorImageInfo {Vulkan::sampler(), _atlas.texture(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
  vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline, 0, 1, &(const VkDescriptorSet&)desc_set, 0, nullptr);

  TextMesh& tm(text_mesh(str));
  tm.mesh.draw(cmd_buffer);
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
