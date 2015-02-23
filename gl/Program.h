#ifndef DEF_L_GL_Program
#define DEF_L_GL_Program

#include <GL/glew.h>
#include "Shader.h"
#include "../stl/Map.h"
#include "../math/Matrix.h"

namespace L {
  namespace GL {
    class Program {
      private:
        GLuint _id;
        Map<String,GLuint> _uniformLocation;
        void attach(const Shader&);
        void detach(const Shader&);
        void link();
      public:
        Program(const Shader&, const Shader&);
        ~Program();
        void use();
        GLuint uniformLocation(const String& name);
        void uniform(const String& name, float);
        void uniform(const String& name, float,float,float);
        void uniform(const String& name, const Matrix44f&);

        static void unuse();
    };
  }
}

#endif



