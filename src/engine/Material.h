#pragma once

#include "../container/KeyValue.h"
#include "../gl/Program.h"
#include "Resource.h"

namespace L {
  class Material {
  protected:
    struct ApplyHelper;
    Resource<Material> _parent;
    Resource<GL::Program> _program;
    Array<KeyValue<Symbol, float>> _scalars;
    Array<KeyValue<Symbol, Resource<GL::Texture>>> _textures;
    Array<KeyValue<Symbol, Vector4f>> _vectors;
    GLenum _primitive_mode = GL_INVALID_ENUM;
    GLsizei _vertex_count = 0;
  public:
    void use(const Matrix44f& model = Matrix44f(1.f));
    void apply_parameters(GL::Program&, ApplyHelper* = nullptr);
    bool valid() const;
    bool drawable() const;
    inline void parent(const Resource<Material>& parent) { _parent = parent; }
    inline void program(const Resource<GL::Program>& program) { _program = program; }
    void scalar(const Symbol& name, float scalar);
    void texture(const Symbol& name, const Resource<GL::Texture>& texture);
    void vector(const Symbol& name, const Vector4f& vector);
    inline void color(const Symbol& name, const Color& color) { vector(name, Color::to_float_vector(color)); }
    inline void primitive_mode(GLenum mode) { _primitive_mode = mode; }
    inline void vertex_count(GLsizei count) { _vertex_count = count; }

    inline Resource<GL::Program> final_program() const { return _program ? _program : (_parent ? _parent->final_program() : _program); }
    inline GLenum final_primitive_mode() const { return _primitive_mode!=GL_INVALID_ENUM ? _primitive_mode : (_parent ? _parent->final_primitive_mode() : _primitive_mode); }
    inline GLsizei final_vertex_count() const { return _vertex_count ? _vertex_count : (_parent ? _parent->final_vertex_count() : _vertex_count); }
  };
}
