#pragma once

#include "../macros.h"
#include <GL/glew.h>

namespace L {
  namespace GL {
    class Buffer {
      L_NOCOPY(Buffer)
    private:
      GLuint _id,_target;
    public:
      Buffer(GLuint target);
      Buffer(GLuint target,GLsizeiptr size,const void* data,GLuint usage,GLuint base=-1);
      ~Buffer();

      void bind();
      void unbind();
      void data(GLsizeiptr size,const void* data,GLuint usage);
      void subData(GLintptr offset,GLsizeiptr size,const void* data);
      void bindBase(GLuint index);
    };
  }
}
