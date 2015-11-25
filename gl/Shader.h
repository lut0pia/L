#ifndef DEF_L_Shader
#define DEF_L_Shader

#include <GL/glew.h>
#include "../system/File.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Shader {
      private:
        GLuint _id;
        void load(const char* src);
      public:
        Shader(File file, GLenum type);
        Shader(const char* src, GLenum type);
        ~Shader();
        L_NoCopy(Shader)
        inline GLuint id() const {return _id;}
    };
  }
}

#endif



