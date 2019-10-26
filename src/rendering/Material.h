#pragma once

#include "../container/KeyValue.h"
#include "../container/Ref.h"
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
      // Pipeline state
      Pipeline::Parameters pipeline;

      // Descriptor state
      Array<KeyValue<Symbol, Resource<Texture>>> textures;
      Array<KeyValue<Symbol, Vector4f>> vectors;
      Array<KeyValue<Symbol, float>> scalars;
      Resource<Font> font;

      // Dynamic state
      Resource<Mesh> mesh;
      String text;
      uint32_t vertex_count = 0;

      void apply(const State&);
      bool fill_desc_set(class DescriptorSet&) const;

      uint32_t pipeline_hash() const;
      uint32_t descriptor_hash() const;
    };
    struct DescSetPairing {
      const class Camera* camera;
      class DescriptorSet* desc_set;
      Time last_framebuffer_update;
      bool valid;
    };
    State _partial_state, _final_state;
    Ref<Pipeline> _pipeline;
    Array<DescSetPairing> _desc_set_pairings;
    uint32_t _last_chain_hash = 0;
    uint32_t _last_pipeline_hash = 0;
    uint32_t _last_descriptor_hash = 0;
    bool _state_dirty = true;

    uint32_t chain_hash() const;
  public:
    typedef Material Intermediate;
    ~Material();
    void update();
    void clear_desc_set_pairings();
    bool valid_for_render_pass(const class RenderPass&) const;
    void draw(const class Camera&, const class RenderPass&, const Matrix44f& model = Matrix44f(1.f));
    Interval3f bounds() const;
    Vector2f gui_size() const;
    const class DescriptorSet* descriptor_set(const class Camera&, const Pipeline&);
    void mark_state_dirty();

    // Chain state
    inline void parent(const Resource<Material>& parent) { _parent = parent; mark_state_dirty(); }

    // Pipeline state
    void shader(VkShaderStageFlags stage, const Resource<Shader>& shader);
    void render_pass(const Symbol& name) { _partial_state.pipeline.render_pass = name; mark_state_dirty(); }
    void polygon_mode(VkPolygonMode polygon_mode) { _partial_state.pipeline.polygon_mode = polygon_mode; mark_state_dirty(); }
    void cull_mode(VkCullModeFlags cull_mode) { _partial_state.pipeline.cull_mode = cull_mode; mark_state_dirty(); }
    void topology(VkPrimitiveTopology topology) { _partial_state.pipeline.topology = topology; mark_state_dirty(); }
    void blend_mode(BlendMode blend_mode) { _partial_state.pipeline.blend_mode = blend_mode; mark_state_dirty(); }

    // Descriptor state
    void texture(const Symbol& name, const Resource<Texture>& texture);
    void vector(const Symbol& name, const Vector4f& vector);
    void scalar(const Symbol& name, float scalar);
    inline void color(const Symbol& name, const Color& color) { vector(name, Color::to_float_vector(color)); }
    inline void font(const Resource<Font>& font) { _partial_state.font = font; mark_state_dirty(); }

    // Dynamic state
    inline void mesh(const Resource<Mesh>& mesh) { _partial_state.mesh = mesh; mark_state_dirty(); }
    inline void text(const String& text) { _partial_state.text = text; mark_state_dirty(); }
    inline void vertex_count(uint32_t count) { _partial_state.vertex_count = count; mark_state_dirty(); }

    inline const State& final_state() const { return _final_state; }

    static void script_registration();

    friend inline Stream& operator<=(Stream& s, const Material& v) { return s <= v._parent <= v._partial_state; }
    friend inline Stream& operator>=(Stream& s, Material& v) { return s >= v._parent >= v._partial_state; }
    friend inline Stream& operator<=(Stream& s, const Material::State& v) {
      // Pipeline state
      s <= v.pipeline;
      // Descriptor state
      s <= v.textures <= v.vectors <= v.scalars <= v.font;
      // Dynamic state
      return s <= v.mesh <= v.text <= v.vertex_count;
    }
    friend inline Stream& operator>=(Stream& s, Material::State& v) {
      // Pipeline state
      s >= v.pipeline;
      // Descriptor state
      s >= v.textures >= v.vectors >= v.scalars >= v.font;
      // Dynamic state
      return s >= v.mesh >= v.text >= v.vertex_count;
    }
  };
}
