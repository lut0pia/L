#include "GL.h"

using namespace L;
using namespace GL;

Texture* whiteTex;

void GL::init(){
  static bool done(false);
  if(!done){
    done = true;
    glewInit();
    whiteTex = new Texture(Image::Bitmap(1,1,Color::white));
  }
}

const Texture& GL::whiteTexture(){
  return *whiteTex;
}
const char* GL::error() {
  static char invalidEnum[] = "GL_INVALID_ENUM";
  static char invalidValue[] = "GL_INVALID_VALUE";
  static char invalidOperation[] = "GL_INVALID_OPERATION";
  static char invalidFramebufferOperation[] = "GL_INVALID_FRAMEBUFFER_OPERATION";
  static char outOfMemory[] = "GL_OUT_OF_MEMORY";
  GLenum error(glGetError());
  switch(error) {
    case GL_INVALID_ENUM:
      return invalidEnum;
    case GL_INVALID_VALUE:
      return invalidValue;
    case GL_INVALID_OPERATION:
      return invalidOperation;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return invalidFramebufferOperation;
    case GL_OUT_OF_MEMORY:
      return outOfMemory;
    default:
      return NULL;
  }
}
void GL::drawAxes() {
  glBegin(GL_LINES);
  glColor3f(1,0,0);
  glVertex3f(0,0,0);
  glVertex3f(1024,0,0);
  glColor3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(0,1024,0);
  glColor3f(0,0,1);
  glVertex3f(0,0,0);
  glVertex3f(0,0,1024);
  glEnd();
}

void GL::draw2dLine(Point<2,float> a, Point<2,float> b, int size, const Color& c) {
  glColor3ub(c.r(),c.g(),c.b());
  glBegin(GL_LINES);
  glVertex2f(a.x(),a.y());
  glVertex2f(b.x(),b.y());
  glEnd();
}

