#include "Mesh.h"

#include "MeshBuilder.h"
#include "GL.h"

using namespace L;

Mesh::Mesh(GLenum mode, GLsizei count, const void* data, GLsizeiptr size, const std::initializer_list<Attribute>& attributes, const GLushort* iarray, GLsizei icount)
  : Mesh() {
  load(mode, count, data, size, attributes, iarray, icount);
}
Mesh::Mesh(const MeshBuilder& mb, GLenum mode, const std::initializer_list<Attribute>& attributes)
  : Mesh() {
  load(mb, mode, attributes);
}
Mesh::~Mesh() {
  if(_vao) glDeleteVertexArrays(1, &_vao);
  if(_vbo) glDeleteBuffers(1, &_vbo);
  if(_eab) glDeleteBuffers(1, &_eab);
  _vao = _vbo = _eab = 0;
}
void Mesh::indices(const GLushort* data, GLsizei count) {
  _count = count;
  glCreateBuffers(1, &_eab);
  glNamedBufferData(_eab, count*sizeof(GLushort), data, GL_STATIC_DRAW);
  glVertexArrayElementBuffer(_vao, _eab);
}
void Mesh::load(GLenum mode, GLsizei count, const void* data, GLsizeiptr size, const std::initializer_list<Attribute>& attributes, const GLushort* iarray, GLsizei icount) {
  this->~Mesh();
  _mode = mode;
  _count = count;
  glCreateVertexArrays(1, &_vao);
  glCreateBuffers(1, &_vbo);
  glNamedBufferData(_vbo, size, data, GL_STATIC_DRAW);
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  GLuint index(0);
  GLsizei stride(size/count);
  GLintptr offset(0);
  for(auto&& a : attributes) {
    glEnableVertexArrayAttrib(_vao, index);
    glVertexAttribPointer(index, a.size, a.type, a.normalized, stride, (const void*)offset);
    switch(a.type) {
      case GL_FLOAT: offset += sizeof(float)*a.size; break;
      default: error("Unknown GL type");
    }
    index++;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  if(iarray) indices(iarray, icount);

  // Compute bounds
  _bounds = (*(Vector3f*)data);
  for(uintptr_t i(1);i<count;i++)
    _bounds.add(*(Vector3f*)((uint8_t*)data+stride*i));
}
void Mesh::load(const MeshBuilder& mb, GLenum mode, const std::initializer_list<Attribute>& attributes) {
  load(mode, GLsizei(mb.vertexCount()), mb.vertices(), mb.verticesSize(), attributes, mb.indices(), GLsizei(mb.indexCount()));
}
void Mesh::draw() const {
  if(_vao) {
    glBindVertexArray(_vao);
    if(_eab) glDrawElements(_mode, _count, GL_UNSIGNED_SHORT, 0);
    else glDrawArrays(_mode, 0, _count);
  }
}

const Mesh& Mesh::quad() {
  static const GLfloat quad[] = {
    -1,-1,0,
    1,-1,0,
    -1,1,0,
    1,1,0,
  };
  static Mesh mesh(GL_TRIANGLE_STRIP, 4, quad, sizeof(quad), {Mesh::Attribute{3,GL_FLOAT,GL_FALSE}});
  return mesh;
}
const Mesh& Mesh::wire_cube() {
  static const GLfloat wireCube[] = {
    // Bottom face
    -1,-1,-1, -1,1,-1,
    -1,-1,-1, 1,-1,-1,
    1,-1,-1,  1,1,-1,
    -1,1,-1,  1,1,-1,
    // Top face
    -1,-1,1, -1,1,1,
    -1,-1,1, 1,-1,1,
    1,-1,1,  1,1,1,
    -1,1,1,  1,1,1,
    // Sides
    -1,-1,-1, -1,-1,1,
    -1,1,-1,  -1,1,1,
    1,-1,-1,  1,-1,1,
    1,1,-1,   1,1,1,
  };
  static Mesh mesh(GL_LINES, 12*2, wireCube, sizeof(wireCube), {Mesh::Attribute{3,GL_FLOAT,GL_FALSE}});
  return mesh;
}
const Mesh& Mesh::wire_sphere() {
  static const float d(sqrt(.5f));
  static const GLfloat wireSphere[] = {
    // X circle
    0,0,-1, 0,-d,-d, 0,-d,-d, 0,-1,0,
    0,-1,0, 0,-d,d,  0,-d,d,  0,0,1,
    0,0,1,  0,d,d,   0,d,d,   0,1,0,
    0,1,0,  0,d,-d,  0,d,-d,  0,0,-1,
    // Y circle
    0,0,-1, -d,0,-d, -d,0,-d, -1,0,0,
    -1,0,0, -d,0,d,  -d,0,d,  0,0,1,
    0,0,1,  d,0,d,   d,0,d,   1,0,0,
    1,0,0,  d,0,-d,  d,0,-d,  0,0,-1,
    // Z circle
    0,-1,0, -d,-d,0, -d,-d,0, -1,0,0,
    -1,0,0, -d,d,0,  -d,d,0,  0,1,0,
    0,1,0,  d,d,0,   d,d,0,   1,0,0,
    1,0,0, d,-d,0,   d,-d,0,  0,-1,0,
  };
  static Mesh mesh(GL_LINES, 24*2, wireSphere, sizeof(wireSphere), {Mesh::Attribute{3,GL_FLOAT,GL_FALSE}});
  return mesh;
}
