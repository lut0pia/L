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
      // Pipeline state
      Array<KeyValue<VkShaderStageFlags, Resource<Shader>>> shaders;
      Symbol render_pass;
      VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT;
      BlendMode blend_mode = BlendMode::None;

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
      void fill_desc_set(class DescriptorSet&) const;

      uint32_t pipeline_hash() const;
      uint32_t descriptor_hash() const;
    };
    struct DescSetPairing {
      const class Camera* camera;
      class DescriptorSet* desc_set;
      Time last_framebuffer_update;
    };
    State _partial_state, _final_state;
    Pipeline* _pipeline = nullptr;
    Array<DescSetPairing> _desc_set_pairings;
    uint32_t _last_chain_hash = 0;
    uint32_t _last_pipeline_hash = 0;
    uint32_t _last_descriptor_hash = 0;
    bool _state_dirty = true;

    uint32_t chain_hash() const;
  public:
    typedef Material Intermediate;
    void update();
    bool valid_for_render_pass(const class RenderPass&) const;
    void draw(const class Camera&, const class RenderPass&, const Matrix44f& model = Matrix44f(1.f));
    Interval3f bounds() const;
    Vector2f gui_size() const;
    const class DescriptorSet& descriptor_set(const class Camera&, const Pipeline&);
    void mark_state_dirty();

    // Chain state
    inline void parent(const Resource<Material>& parent) { _parent = parent; mark_state_dirty(); }

    // Pipeline state
    void shader(VkShaderStageFlags stage, const Resource<Shader>& shader, bool override = true);
    void render_pass(const Symbol& name) { _partial_state.render_pass = name; mark_state_dirty(); }
    void cull_mode(VkCullModeFlags cull_mode) { _partial_state.cull_mode = cull_mode; mark_state_dirty(); }
    void blend_mode(BlendMode blend_mode) { _partial_state.blend_mode = blend_mode; mark_state_dirty(); }

    // Descriptor state
    void texture(const Symbol& name, const Resource<Texture>& texture, bool override = true);
    void vector(const Symbol& name, const Vector4f& vector, bool override = true);
    void scalar(const Symbol& name, float scalar, bool override = true);
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
      s <= v.shaders <= v.render_pass <= v.cull_mode <= v.blend_mode;
      // Descriptor state
      s <= v.textures <= v.vectors <= v.scalars <= v.font;
      // Dynamic state
      return s <= v.mesh <= v.text <= v.vertex_count;
    }
    friend inline Stream& operator>=(Stream& s, Material::State& v) {
      // Pipeline state
      s >= v.shaders >= v.render_pass >= v.cull_mode >= v.blend_mode;
      // Descriptor state
      s >= v.textures >= v.vectors >= v.scalars >= v.font;
      // Dynamic state
      return s >= v.mesh >= v.text >= v.vertex_count;
    }
  };
}
