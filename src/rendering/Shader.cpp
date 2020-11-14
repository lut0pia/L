#include "Shader.h"

using namespace L;

Shader::Shader(const Intermediate& intermediate) : _stage(intermediate.stage), _bindings(intermediate.bindings) {
  _impl = Renderer::get()->create_shader(intermediate.stage, intermediate.binary.data(), intermediate.binary.size());
}
Shader::~Shader() {
  Renderer::get()->destroy_shader(_impl);
}
