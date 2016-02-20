#ifndef DEF_L_GL_Light
#define DEF_L_GL_Light

#include <GL/glew.h>
#include "Shader.h"
#include "../math/Vector.h"
#include "../containers/Map.h"

namespace L {
  namespace GL {
    class Light {
      private:
        Vector4f _position, _color;
      public:
        Light();
        void position(float x, float y, float z, float w = 1);
        void color(float r, float g, float b, float a = 1);
        Vector3f position() const;
        void set(GLenum light);
    };
  }
}

#endif



