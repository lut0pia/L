#ifndef DEF_L_GL_Light
#define DEF_L_GL_Light

#include <GL/glew.h>
#include "Shader.h"
#include "../geometry/Point.h"
#include "../stl/Map.h"

namespace L {
  namespace GL {
    class Light {
      private:
        Point4f _position, _color;
      public:
        Light();
        void position(float x, float y, float z, float w = 1);
        void color(float r, float g, float b, float a = 1);
        Point3f position() const;
        void set(GLenum light);
    };
  }
}

#endif



