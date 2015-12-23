#include "Sprite.h"

using namespace L;

void Sprite::render(const Camera&) {
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
