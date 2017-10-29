#pragma once

#include <initializer_list>
#include "GL.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Buffer;
    class MeshBuilder;
    class Mesh {
      L_NOCOPY(Mesh)
    private:
      GLuint _vao,_vbo,_eab;
      GLenum _mode;
      GLsizei _count;
    public:
      struct Attribute{
        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        GLintptr offset;
      };
      inline Mesh() : _vao(0),_vbo(0),_eab(0){}
      Mesh(GLenum mode,GLsizei count,const void* data,GLsizeiptr size,const std::initializer_list<Attribute>&,const GLushort* indices = nullptr,GLsizei icount = 0);
      Mesh(const MeshBuilder&, GLenum mode, const std::initializer_list<Attribute>&);
      ~Mesh();
      void indices(const GLushort* data,GLsizei count);
      void load(GLenum mode,GLsizei count,const void* data,GLsizeiptr size,const std::initializer_list<Attribute>& attributes,const GLushort* indices = nullptr,GLsizei icount = 0);
      void load(const MeshBuilder&,GLenum mode,const std::initializer_list<Attribute>&);
      void draw() const;
    };
  }
}
