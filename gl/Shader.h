#ifndef DEF_L_Shader
#define DEF_L_Shader

#include <GL/glew.h>
#include "../stl/String.h"
#include "../system/File.h"

namespace L {
  namespace GL {
    class Shader {
      private:
        GLuint _id;
        void load(const String& src);
      public:
        Shader(File file, GLenum type);
        Shader(const String& src, GLenum type);
        ~Shader();
        L_NoCopy(Shader)
        inline GLuint id() const{return _id;}
    };
  }
}

#endif



