#pragma once

#include "GL.h"
#include "../macros.h"

namespace L {
  class GPUBuffer {
    L_NOCOPY(GPUBuffer)
  private:
    GLuint _id, _target;
  public:
    GPUBuffer(GLuint target);
    GPUBuffer(GLuint target, GLsizeiptr size, const void* data, GLuint usage, GLuint base = -1);
    ~GPUBuffer();

    void bind();
    void unbind();
    void data(GLsizeiptr size, const void* data, GLuint usage);
    void subData(GLintptr offset, GLsizeiptr size, const void* data);
    template<class T> void subData(GLintptr offset, const T& value) { subData(offset, sizeof(T), &value); }
    void bindBase(GLuint index);
  };
}
