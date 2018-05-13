#include "Material.h"

using namespace L;

void Material::draw(const Matrix44f& model) {
  if(Resource<Program> program = final_program()) {
    program->use();
    apply_parameters(*program);
    program->uniform("model", model);
    if(Resource<Mesh> mesh = final_mesh()) {
      mesh->draw();
    } else {
      const GLenum prim_mode(final_primitive_mode());
      const GLsizei vert_count(final_vertex_count());
      if(prim_mode!=GL_INVALID_ENUM && vert_count!=0) {
        GL::draw(final_primitive_mode(), final_vertex_count());
      }
    }
  }
}

struct Material::ApplyHelper {
  Symbol applied[128];
  uintptr_t next = 0;
  GLenum texture_unit = GL_TEXTURE0;
  bool is_applied(const Symbol& name) const {
    for(uintptr_t i(0); i<next; i++)
      if(applied[i]==name)
        return true;
    return false;
  }
  void add_applied(const Symbol& name) {
    applied[next++] = name;
  }
};
void Material::apply_parameters(Program& program, ApplyHelper* helper) {
  if(!helper) {
    ApplyHelper local_helper;
    return apply_parameters(program, &local_helper);
  } else {
    for(const auto& parameter : _scalars)
      if(parameter.value() && !helper->is_applied(parameter.key())) {
        program.uniform(parameter.key(), parameter.value());
        helper->add_applied(parameter.key());
      }
    for(const auto& parameter : _textures)
      if(parameter.value() && !helper->is_applied(parameter.key())) {
        program.uniform(parameter.key(), *parameter.value(), helper->texture_unit++);
        helper->add_applied(parameter.key());
      }
    for(const auto& parameter : _vectors)
      if(!helper->is_applied(parameter.key())) {
        program.uniform(parameter.key(), parameter.value());
        helper->add_applied(parameter.key());
      }
    if(_parent)
      _parent->apply_parameters(program, helper);
  }
}
bool Material::valid() const {
  const bool can_draw_without_mesh(final_primitive_mode()!=GL_INVALID_ENUM && final_vertex_count()!=0);
  return final_program().is_set() && (can_draw_without_mesh || final_mesh().is_set());
}
Interval3f Material::bounds() const {
  if(Resource<Mesh> mesh = final_mesh())
    return mesh->bounds();
  else return Interval3f(Vector3f(-1.f),Vector3f(1.f));
}

void Material::scalar(const Symbol& name, float scalar) {
  for(uintptr_t i(0); i<_scalars.size(); i++)
    if(_scalars[i].key()==name) {
      _scalars[i].value() = scalar;
      return;
    }
  _scalars.push(name, scalar);
}
void Material::texture(const Symbol& name, const Resource<Texture>& texture) {
  for(uintptr_t i(0); i<_textures.size(); i++)
    if(_textures[i].key()==name) {
      _textures[i].value() = texture;
      return;
    }
  _textures.push(name, texture);
}
void Material::vector(const Symbol& name, const Vector4f& vector) {
  for(uintptr_t i(0); i<_vectors.size(); i++)
    if(_vectors[i].key()==name) {
      _vectors[i].value() = vector;
      return;
    }
  _vectors.push(name, vector);
}
