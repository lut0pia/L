#include "PostProcess.h"

using namespace L;
using namespace GL;

PostProcess::PostProcess(int width, int height) : _frameBuffer(GL_FRAMEBUFFER) {
  resize(width, height);
  _color.parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  _color.parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  _frameBuffer.attach(GL_COLOR_ATTACHMENT0,_color);
  _frameBuffer.attach(GL_DEPTH_ATTACHMENT,_depth);
  _frameBuffer.unbind();
}
void PostProcess::resize(int width, int height) {
  _aspect = (float)width/height;
  _color.load(width,height);
  _depth.storage(GL_DEPTH_COMPONENT32F,width,height);
}
void PostProcess::prerender() {
  _frameBuffer.bind();
}
void PostProcess::postrender(Program& program) {
  _frameBuffer.unbind();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  program.use();
  program.uniform("aspect",_aspect);
  program.uniform("color",_color,GL_TEXTURE0);
  //program.uniform("depth",_depth,GL_TEXTURE1);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0);
  glVertex2f(-1,-1);
  glTexCoord2f(1,0);
  glVertex2f(1,-1);
  glTexCoord2f(1,1);
  glVertex2f(1,1);
  glTexCoord2f(0,1);
  glVertex2f(-1,1);
  glEnd();
  program.unuse();
}
