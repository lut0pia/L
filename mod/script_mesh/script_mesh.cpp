#include <L/src/container/Handle.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Color.h>
#include <L/src/rendering/Mesh.h>
#include <L/src/script/ScriptContext.h>
#include <L/src/script/script_binding.h>

using namespace L;

class ScriptMesh : public Handled<ScriptMesh> {
protected:
  const ResourceSlot& _slot;
  Mesh::Intermediate& _mesh;
  Array<Vector3f> _positions, _normals, _tangents;
  Array<Vector2f> _texcoords;
  Array<Color> _colors;
  size_t _vertex_size = 0;

  template<class T>
  bool check_attribute(const char* name, const Array<T>& arr, RenderFormat format, VertexAttributeType type, uintptr_t& offset) {
    if(arr.size() == _positions.size()) {
      offset = _vertex_size;
      _mesh.attributes.push(VertexAttribute {format, type});
      _vertex_size += Renderer::format_size(format);
    } else if(arr.size() > 0) {
      warning("script_mesh: %s: missing %s vertex data", _slot.id, name);
      return false;
    }
    return true;
  }
  template<class T>
  void copy_attribute(uint8_t* dst, const Array<T>& arr, uintptr_t offset) {
    if(arr.size() == _positions.size()) {
      for(uintptr_t i = 0; i < arr.size(); i++) {
        memcpy(dst + _vertex_size * i + offset, &arr[i], sizeof(T));
      }
    }
  }

public:
  inline ScriptMesh(const ResourceSlot& slot, Mesh::Intermediate& intermediate)
    : Handled<ScriptMesh>(this), _slot(slot), _mesh(intermediate) {}

  void push_position(const Vector3f& p) { _positions.push(p); }
  void push_normal(const Vector3f& n) { _normals.push(n); }
  void push_tangent(const Vector3f& t) { _tangents.push(t); }

  void push_texcoord(const Vector2f& t) { _texcoords.push(t); }

  void push_color(const Color& c) { _colors.push(c); }

  bool finalize() {
    if(_positions.size() == 0) {
      warning("script_mesh: %s: no vertex data", _slot.id);
      return false;
    }

    uintptr_t position_offset = 0, normal_offset = 0, tangent_offset = 0, texcoord_offset = 0, color_offset = 0;

    const bool check_success =
      check_attribute("position", _positions, RenderFormat::R32G32B32_SFloat, VertexAttributeType::Position, position_offset) &&
      check_attribute("normal", _normals, RenderFormat::R32G32B32_SFloat, VertexAttributeType::Normal, normal_offset) &&
      check_attribute("tangent", _tangents, RenderFormat::R32G32B32_SFloat, VertexAttributeType::Tangent, tangent_offset) &&
      check_attribute("texcoord", _texcoords, RenderFormat::R32G32_SFloat, VertexAttributeType::TexCoord, texcoord_offset) &&
      check_attribute("color", _colors, RenderFormat::R8G8B8A8_UNorm, VertexAttributeType::Color, color_offset);

    if(!check_success) {
      return false;
    }

    _mesh.vertices = Buffer(_positions.size() * _vertex_size);

    uint8_t* dst = (uint8_t*)_mesh.vertices.data();
    copy_attribute(dst, _positions, position_offset);
    copy_attribute(dst, _normals, normal_offset);
    copy_attribute(dst, _tangents, tangent_offset);
    copy_attribute(dst, _texcoords, texcoord_offset);
    copy_attribute(dst, _colors, color_offset);

    return true;
  }
};

static bool script_mesh_loader(ResourceSlot& slot, Mesh::Intermediate& intermediate) {
  ScriptFunction::Intermediate script_intermediate;
  if(ResourceLoading<ScriptFunction>::load_internal(slot, script_intermediate)) {
    ScriptMesh script_texture(slot, intermediate);
    ScriptContext context(script_texture.handle());
    context.execute(ref<ScriptFunction>(script_intermediate));
    return script_texture.finalize();
  }
  return false;
}

void script_mesh_module_init() {
  ResourceLoading<Mesh>::add_loader(script_mesh_loader);

  L_SCRIPT_METHOD(ScriptMesh, "push_position", 1, push_position(c.param(0)));
  L_SCRIPT_METHOD(ScriptMesh, "push_normal", 1, push_normal(c.param(0)));
  L_SCRIPT_METHOD(ScriptMesh, "push_tangent", 1, push_tangent(c.param(0)));
  L_SCRIPT_METHOD(ScriptMesh, "push_texcoord", 1, push_texcoord(c.param(0)));
  L_SCRIPT_METHOD(ScriptMesh, "push_color", 1, push_color(c.param(0)));
}
