#pragma once

#include "../container/KeyValue.h"
#include "../engine/Resource.h"
#include "../rendering/Program.h"
#include "../rendering/Mesh.h"

namespace L {
  class Material {
  protected:
    struct ApplyHelper;
    Resource<Material> _parent;
    Resource<Program> _program;
    Resource<Mesh> _mesh;
    Array<KeyValue<Symbol, float>> _scalars;
    Array<KeyValue<Symbol, Resource<Texture>>> _textures;
    Array<KeyValue<Symbol, Vector4f>> _vectors;
    GLenum _primitive_mode = GL_INVALID_ENUM;
    GLsizei _vertex_count = 0;
  public:
    void draw(const Matrix44f& model = Matrix44f(1.f));
    void apply_parameters(Program&, ApplyHelper* = nullptr);
    bool valid() const;
    Interval3f bounds() const;

    inline void parent(const Resource<Material>& parent) { _parent = parent; }
    inline void program(const Resource<Program>& program) { _program = program; }
    inline void mesh(const Resource<Mesh>& mesh) { _mesh = mesh; }
    void scalar(const Symbol& name, float scalar);
    void texture(const Symbol& name, const Resource<Texture>& texture);
    void vector(const Symbol& name, const Vector4f& vector);
    inline void color(const Symbol& name, const Color& color) { vector(name, Color::to_float_vector(color)); }
    inline void primitive_mode(GLenum mode) { _primitive_mode = mode; }
    inline void vertex_count(GLsizei count) { _vertex_count = count; }

    inline Resource<Program> final_program() const { return _program.is_set() ? _program : (_parent ? _parent->final_program() : _program); }
    inline Resource<Mesh> final_mesh() const { return _mesh.is_set() ? _mesh : (_parent ? _parent->final_mesh() : _mesh); }
    inline GLenum final_primitive_mode() const { return _primitive_mode!=GL_INVALID_ENUM ? _primitive_mode : (_parent ? _parent->final_primitive_mode() : _primitive_mode); }
    inline GLsizei final_vertex_count() const { return _vertex_count ? _vertex_count : (_parent ? _parent->final_vertex_count() : _vertex_count); }
  };
}
