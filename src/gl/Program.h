#pragma once

#include <GL/glew.h>
#include "Shader.h"
#include "Texture.h"
#include "../containers/Map.h"
#include "../image/Color.h"
#include "../math/Matrix.h"

namespace L {
  namespace GL {
    class Program {
      L_NOCOPY(Program)
    private:
      GLuint _id;
      Map<uint32_t, GLuint> _uniformLocation;
    public:
      Program(const Shader*, uint32_t);
      Program(const Shader&);
      Program(const Shader&, const Shader&);
      Program(const Shader&, const Shader&, const Shader&);
      ~Program();
      void link(const GLuint*, uint32_t count);
      bool check() const;
      void use() const;
      GLuint uniformLocation(const char* name);
      GLuint uniformBlockIndex(const char* name);
      void uniformBlockBinding(const char* name, GLuint binding);
      void uniform(const char* name, int);
      void uniform(const char* name, unsigned int);
      void uniform(const char* name, float);
      void uniform(const char* name, float, float);
      void uniform(const char* name, float, float, float);
      void uniform(const char* name, float, float, float, float);
      inline void uniform(const char* name, const Vector2f& v) { uniform(name, v.x(), v.y()); }
      inline void uniform(const char* name, const Vector3f& v) { uniform(name, v.x(), v.y(), v.z()); }
      inline void uniform(const char* name, const Vector4f& v) { uniform(name, v.x(), v.y(), v.z(), v.w()); }
      inline void uniform(const char* name, const Color& c) { uniform(name, c.rf(), c.gf(), c.bf(), c.af()); }
      void uniform(const char* name, const Matrix44f&);
      void uniform(const char* name, const Texture&, GLenum unit = GL_TEXTURE0);

      static void unuse();
    };
  }
}
