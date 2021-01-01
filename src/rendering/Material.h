#pragma once

#include "Color.h"
#include "../container/Handle.h"
#include "../container/KeyValue.h"
#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "Font.h"
#include "Pipeline.h"
#include "Texture.h"
#include "Mesh.h"

namespace L {
  class Material : public Handled<Material> {
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
      Interval2i scissor = Vector2i(0);
      uint32_t vertex_count = 0;
      uint32_t index_offset = 0;

      void apply(const State&);

      uint32_t pipeline_hash() const;
      uint32_t descriptor_hash() const;
    };
    struct DescSetPairing {
      Handle<class Camera> camera;
      PipelineImpl* pipeline;
      DescriptorSetImpl* desc_set;
      Time last_framebuffer_update;
      bool valid;
    };
    State _partial_state, _final_state;
    Ref<Pipeline> _pipeline;
    Array<DescSetPairing> _desc_set_pairings;
    Array<UniformBuffer*> _buffers;
    uint32_t _last_chain_hash = 0;
    uint32_t _last_pipeline_hash = 0;
    uint32_t _last_descriptor_hash = 0;
    bool _state_dirty = true;

    uint32_t chain_hash() const;
    DescriptorSetImpl* descriptor_set(const class Camera&, const Pipeline&);
    bool fill_desc_set(const Pipeline&, DescriptorSetImpl*) const;
    void create_uniform_buffers(const class Pipeline&);
    void clear_desc_set_pairings();

  public:
    typedef Material Intermediate;
    inline Material() : Handled<Material>(this) {}
    inline Material(const Material& other) : Handled<Material>(this), _parent(other._parent), _partial_state(other._partial_state) {}
    ~Material();

    void update();
    void draw(const class Camera&, const class RenderPass&, const Matrix44f& model = Matrix44f(1.f));
    void set_buffer(const Symbol& name, const void* data, size_t size);

    bool valid_for_render_pass(const class RenderPass&) const;
    bool is_text() const;
    Interval3f bounds() const;
    Vector2f gui_size() const;

    // Chain state
    inline void parent(const Resource<Material>& parent) { _parent = parent; _state_dirty = true; }

    // Pipeline state
    void shader(ShaderStage stage, const Resource<Shader>& shader);
    void render_pass(const Symbol& name) { _partial_state.pipeline.render_pass = name; _state_dirty = true; }
    void polygon_mode(PolygonMode polygon_mode) { _partial_state.pipeline.polygon_mode = polygon_mode; _state_dirty = true; }
    void cull_mode(CullMode cull_mode) { _partial_state.pipeline.cull_mode = cull_mode; _state_dirty = true; }
    void topology(PrimitiveTopology topology) { _partial_state.pipeline.topology = topology; _state_dirty = true; }
    void blend_mode(BlendMode blend_mode) { _partial_state.pipeline.blend_mode = blend_mode; _state_dirty = true; }
    void depth_func(DepthFunc depth_func) { _partial_state.pipeline.depth_func = depth_func; _state_dirty = true; }

    // Descriptor state
    void texture(const Symbol& name, const Resource<Texture>& texture);
    void vector(const Symbol& name, const Vector4f& vector);
    void scalar(const Symbol& name, float scalar);
    inline void color(const Symbol& name, const Color& color) { vector(name, Color::to_float_vector(color)); }
    inline void font(const Resource<Font>& font) { _partial_state.font = font; _state_dirty = true; }

    // Dynamic state
    inline void mesh(const Resource<Mesh>& mesh) { _partial_state.mesh = mesh; _state_dirty = true; }
    inline void text(const String& text) { _partial_state.text = text; _state_dirty = true; }
    inline void scissor(Interval2i interval) { _partial_state.scissor = interval; _state_dirty = true; }
    inline void vertex_count(uint32_t count) { _partial_state.vertex_count = count; _state_dirty = true; }
    inline void index_offset(uint32_t offset) { _partial_state.index_offset = offset; _state_dirty = true; }

    static void script_registration();

    friend inline void resource_write(Stream& s, const Material& v) { s <= v._parent <= v._partial_state; }
    friend inline void resource_read(Stream& s, Material& v) { s >= v._parent >= v._partial_state; }
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
