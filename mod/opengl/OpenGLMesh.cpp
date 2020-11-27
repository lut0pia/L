#include "OpenGLRenderer.h"

using namespace L;

MeshImpl* OpenGLRenderer::create_mesh(size_t count, const void* data, size_t size, const VertexAttribute* attributes, size_t attribute_count, const uint16_t* iarray, size_t icount) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLMesh* mesh = Memory::new_type<OpenGLMesh>();
  glCreateVertexArrays(1, &mesh->vao);
  glCreateBuffers(1, &mesh->vbo);
  glNamedBufferData(mesh->vbo, size, data, GL_STATIC_DRAW);
  glBindVertexArray(mesh->vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  mesh->stride = GLsizei(size / count);
  GLintptr offset = 0;
  for(GLuint i = 0; i < attribute_count; i++) {
    const VertexAttribute& attribute = attributes[i];
    glEnableVertexArrayAttrib(mesh->vao, i);
    if(to_gl_integer(attribute.format)) {
      glVertexAttribIPointer(i, to_gl_size(attribute.format), to_gl_type(attribute.format), mesh->stride, (const void*)offset);
    } else {
      glVertexAttribPointer(i, to_gl_size(attribute.format), to_gl_type(attribute.format), to_gl_normalized(attribute.format), mesh->stride, (const void*)offset);
    }
    offset += format_size(attribute.format);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  if(iarray) {
    glCreateBuffers(1, &mesh->eab);
    glNamedBufferData(mesh->eab, icount * sizeof(GLushort), iarray, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(mesh->vao, mesh->eab);
  }

  return mesh;
}

void OpenGLRenderer::destroy_mesh(MeshImpl* mesh) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLMesh* gl_mesh = (OpenGLMesh*)mesh;
  if(gl_mesh->vao) {
    glDeleteVertexArrays(1, &gl_mesh->vao);
  }
  if(gl_mesh->vbo) {
    glDeleteBuffers(1, &gl_mesh->vbo);
  }
  if(gl_mesh->eab) {
    glDeleteBuffers(1, &gl_mesh->eab);
  }

  Memory::delete_type(gl_mesh);
}
