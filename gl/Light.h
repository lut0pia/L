#ifndef DEF_L_GL_Light
#define DEF_L_GL_Light

#include <GL/glew.h>
#include "Shader.h"
#include "../stl/Map.h"

namespace L {
  namespace GL {
    class Light {
      private:
        float _position[4],_color[4];
      public:
        Light();
        void setPosition(float x, float y, float z, float w = 1);
        void setColor(float r, float g, float b, float a = 1);
        void set(GLenum light);
    };
  }
}

#endif



