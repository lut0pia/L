#include "Material.h"

using namespace L;

void Material::use(const Matrix44f& model) {
  Resource<GL::Program> program(final_program());
  if(program) {
    program->use();
    apply_parameters(*program);
    program->uniform("model", model);
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
void Material::apply_parameters(GL::Program& program, ApplyHelper* helper) {
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
  return final_program();
}
bool Material::drawable() const {
  return valid() && final_primitive_mode()!=GL_INVALID_ENUM && final_vertex_count()!=0;
}

void Material::scalar(const Symbol& name, float scalar) {
  for(uintptr_t i(0); i<_scalars.size(); i++)
    if(_scalars[i].key()==name) {
      _scalars[i].value() = scalar;
      return;
    }
  _scalars.push(name, scalar);
}
void Material::texture(const Symbol& name, const Resource<GL::Texture>& texture) {
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
