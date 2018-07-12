#pragma once

#include "../container/Table.h"
#include "../engine/Resource.h"
#include "../image/Bitmap.h"
#include "../rendering/Atlas.h"
#include "../rendering/Mesh.h"
#include "../rendering/Pipeline.h"
#include "../math/Vector.h"
#include "../time/Time.h"

namespace L {
  class Font {
  private:
    Bitmap _bmp;
  protected:
    static Resource<Pipeline> _pipeline;
    struct Glyph {
      Vector2i origin, size;
      int advance;
      Interval2f atlas_coords;
      bool init = false;
    };
    struct TextMesh {
      String str;
      Mesh mesh;
      Vector2i dimensions = {0,0};
      Time last_used;
    };
    Atlas _atlas;
    Glyph _ascii[128];
    Table<uint32_t, Glyph> _glyphs;
    Table<uint32_t, TextMesh> _text_meshes;
    Time _last_update;
    int _lineheight;

  public:
    inline Font() : _last_update(0) {}
    virtual ~Font() {}
    const Glyph& glyph(uint32_t utf32);
    virtual TextMesh& text_mesh(const char*);
    virtual void draw(VkCommandBuffer cmd_buffer, const Matrix44f& model, const char*, Vector2f anchor = Vector2f(0.f, 0.f));
    virtual void load_glyph(uint32_t utf32, Glyph& out_glyph, Bitmap& out_bmp) = 0;

    void update();

    inline static void pipeline(const char* path) { _pipeline = path; }
  };
  template <> void load_resource(ResourceSlot<Font>& slot);
}
