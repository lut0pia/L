#ifndef DEF_L_GL_Program
#define DEF_L_GL_Program

#include <GL/glew.h>
#include "Shader.h"
#include "Texture.h"
#include "../containers/Map.h"
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
        Program(const Shader&);
        Program(const Shader&, const Shader&);
        Program(const Shader&, const Shader&, const Shader&);
        ~Program();
        L_NoCopy(Program)
        void use() const;
        GLuint uniformLocation(const String& name);
        void uniform(const String& name, float);
        void uniform(const String& name, float,float,float);
        void uniform(const String& name, const Vector3f&);
        void uniform(const String& name, const Matrix44f&);
        void uniform(const String& name, const Texture&, GLenum unit = GL_TEXTURE0);

        static void unuse();
    };
  }
}

#endif



