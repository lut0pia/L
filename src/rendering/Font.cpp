#include "Font.h"

#include "../engine/Resource.inl"
#include "../text/encoding.h"
#include "../rendering/DescriptorSet.h"

using namespace L;

Resource<Pipeline> Font::_pipeline;

Font::Font(const Intermediate& intermediate) : _atlas(intermediate.texture_intermediate), _last_update(0), _line_height(intermediate.line_height) {
  for(const auto& pair : intermediate.glyphs) {
    if(pair.key()<128) {
      _ascii[pair.key()] = pair.value();
    } else {
      _glyphs[pair.key()] = pair.value();
    }
  }
}
const Font::Glyph& Font::glyph(uint32_t utf32) {
  static const Font::Glyph no_glyph;
  const Glyph* glyph((utf32<128) ? (_ascii + utf32) : _glyphs.find(utf32));
  if(!glyph) glyph = &no_glyph;
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
    wtr.dimensions.y() = _line_height;
    int x(0), y(0);
    while(*str) {
      const uint32_t utf32(utf8_to_utf32(str));
      if(utf32=='\n') { // End line
        x = 0;
        y += _line_height;
        wtr.dimensions.y() = max(wtr.dimensions.y(), y+_line_height);
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
    warning("Attempting to draw empty string");
    return;
  }
  if(!_pipeline) {
    return;
  }
  vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline);
  vkCmdPushConstants(cmd_buffer, *_pipeline, _pipeline->find_binding("Constants")->stage, 0, sizeof(model), &model);

  DescriptorSet desc_set(*_pipeline);
  desc_set.set_descriptor("atlas", VkDescriptorImageInfo {Vulkan::sampler(), _atlas, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
  desc_set.set_value("color", Vector4f(1.f)); // TODO: Change API to allow colored text rendering
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

  Array<uint32_t> obsolete_text_meshes;
  obsolete_text_meshes.clear();
  for(const auto& tm : _text_meshes)
    if(now-tm.value().last_used>second) // Not used for a second
      obsolete_text_meshes.push(tm.key());
  for(uint32_t obsolete_text_mesh : obsolete_text_meshes)
    _text_meshes.remove(obsolete_text_mesh);
}
