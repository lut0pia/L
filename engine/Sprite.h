#ifndef DEF_L_Sprite
#define DEF_L_Sprite

#include "Transform.h"
#include "../geometry/Interval.h"
#include "../gl/Texture.h"
#include "../gl/GL.h"

namespace L {
  class Sprite : public Component {
    private:
      Transform* _transform;
      Ref<GL::Texture> _texture;
      Interval2f _vertex, _uv;
    public:
      void start() {
        _transform = entity().component<Transform>();
      }
      static const bool enableRender = true;
      void render(const Camera&) {
        if(!_texture.null()) {
          glPushMatrix();
          glMultTransposeMatrixf(_transform->absolute().array());
          _texture->bind();
          glBegin(GL_QUADS);
          glTexCoord2f(_uv.min().x(),_uv.max().y());
          glVertex2f(_vertex.min().x(),_vertex.min().y());
          glTexCoord2f(_uv.min().x(),_uv.min().y());
          glVertex2f(_vertex.min().x(),_vertex.max().y());
          glTexCoord2f(_uv.max().x(),_uv.min().y());
          glVertex2f(_vertex.max().x(),_vertex.max().y());
          glTexCoord2f(_uv.max().x(),_uv.max().y());
          glVertex2f(_vertex.max().x(),_vertex.min().y());
          glEnd();
          glPopMatrix();
        }
      }
      inline void texture(const char* filename) {_texture = Engine::texture(filename);}
      inline void texture(const Ref<GL::Texture>& tex) {_texture = tex;}
      inline void vertex(const Interval2f& v) {_vertex = v;}
      inline void uv(const Interval2f& u) {_uv = u;}
  };
}


#endif

