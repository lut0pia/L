#pragma once

#include "../container/KeyValue.h"
#include "../engine/Resource.h"
#include "../image/Color.h"
#include "Font.h"
#include "Pipeline.h"
#include "Texture.h"
#include "Mesh.h"

namespace L {
  class Material {
  protected:
    Resource<Material> _parent;
    struct State {
      Resource<Pipeline> pipeline;
      Resource<Mesh> mesh;
      Resource<Font> font;
      String text;
      Array<KeyValue<Symbol, float>> scalars;
      Array<KeyValue<Symbol, Resource<Texture>>> textures;
      Array<KeyValue<Symbol, Vector4f>> vectors;
      uint32_t vertex_count = 0;

      void apply(const State&);
      void fill_desc_set(class DescriptorSet&) const;
      void reset();
      uint64_t loading_state() const;
    };
    struct DescSetPairing {
      const class Camera* camera;
      class DescriptorSet* desc_set;
      Time last_framebuffer_update;
    };
    State _partial_state, _final_state;
    Array<DescSetPairing> _desc_set_pairings;
    Time _last_state_dirty = 0, _last_state_update = 0;
    uint64_t _loading_state = 0;
    bool _state_dirty = true;
  public:
    typedef Material Intermediate;
    Material();
    void update();
    bool valid_for_render_pass(const class RenderPass&) const;
    void draw(const class Camera&, const class RenderPass&, const Matrix44f& model = Matrix44f(1.f));
    Interval3f bounds() const;
    Vector2f gui_size() const;
    const class DescriptorSet& descriptor_set(const class Camera&, const Pipeline&);
    void mark_state_dirty();

    inline void parent(const Resource<Material>& parent) { _parent = parent; mark_state_dirty(); }
    inline void pipeline(const Resource<Pipeline>& pipeline) { _partial_state.pipeline = pipeline; mark_state_dirty(); }
    inline void mesh(const Resource<Mesh>& mesh) { _partial_state.mesh = mesh; mark_state_dirty(); }
    inline void font(const Resource<Font>& font) { _partial_state.font = font; mark_state_dirty(); }
    inline void text(const String& text) { _partial_state.text = text; mark_state_dirty(); }
    void scalar(const Symbol& name, float scalar, bool override = true);
    void texture(const Symbol& name, const Resource<Texture>& texture, bool override = true);
    void vector(const Symbol& name, const Vector4f& vector, bool override = true);
    inline void color(const Symbol& name, const Color& color) { vector(name, Color::to_float_vector(color)); }
    inline void vertex_count(uint32_t count) { _partial_state.vertex_count = count; }

    inline const State& final_state() const { return _final_state; }

    friend inline Stream& operator<=(Stream& s, const Material& v) { return s <= v._parent <= v._partial_state; }
    friend inline Stream& operator>=(Stream& s, Material& v) { return s >= v._parent >= v._partial_state; }
    friend inline Stream& operator<=(Stream& s, const Material::State& v) { return s <= v.pipeline <= v.mesh <= v.font <= v.text <= v.scalars <= v.textures <= v.vectors <= v.vertex_count; }
    friend inline Stream& operator>=(Stream& s, Material::State& v) { return s >= v.pipeline >= v.mesh >= v.font >= v.text >= v.scalars >= v.textures >= v.vectors >= v.vertex_count; }
  };
}
