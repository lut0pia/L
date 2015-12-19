#include "GL.h"

#include "../constants.h"
#include "MeshBuilder.h"
#include "Shader.h"

using namespace L;
using namespace GL;

Texture* whiteTex;
Program* program;
MeshBuilder meshBuilder;

void GL::init() {
  static bool done(false);
  if(!done) {
    done = true;
    glewInit();
    whiteTex = new Texture(Bitmap(1,1,Color::white));
    program = new Program(Shader(
                            "#version 130\n"
                            "uniform mat4 projection;"
                            "varying vec4 color;"
                            "void main() {"
                            "gl_Position =  projection * gl_ModelViewMatrix * gl_Vertex;"
                            "gl_TexCoord[0] = gl_MultiTexCoord0;"
                            "color = gl_Color;"
                            "}",GL_VERTEX_SHADER),
                          Shader(
                            "#version 130\n"
                            "uniform sampler2D texture;"
                            "varying vec4 color;"
                            "void main(){"
                            "gl_FragColor = color*texture2D(texture,gl_TexCoord[0].xy);"
                            "}",GL_FRAGMENT_SHADER));
  }
}
const Texture& GL::whiteTexture() {
  return *whiteTex;
}
Program& GL::baseProgram() {
  return *program;
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
void GL::makeDisc(Mesh& mesh, int slices) {
  meshBuilder.reset(Mesh::VERTEX,128,128);
  meshBuilder.setVertex(Vector3f(0,0,0));
  uint center(meshBuilder.addVertex());
  meshBuilder.setVertex(Vector3f(1,0,0));
  uint first(meshBuilder.addVertex());
  uint last(first);
  for(int i(1); i<slices; i++) {
    float angle(((float)i/slices)*PI<float>()*2);
    meshBuilder.setVertex(Vector3f(cos(angle),sin(angle),0));
    uint current(meshBuilder.addVertex());
    meshBuilder.addTriangle(center,current,last);
    last = current;
  }
  meshBuilder.addTriangle(center,first,last);
  reconstruct(mesh,meshBuilder);
}
void GL::draw2dLine(Vector<2,float> a, Vector<2,float> b, int size, const Color& c) {
  color(c);
  glBegin(GL_LINES);
  glVertex2f(a.x(),a.y());
  glVertex2f(b.x(),b.y());
  glEnd();
}
