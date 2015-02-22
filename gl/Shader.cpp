#include "Shader.h"

#include "../Exception.h"

using namespace L;
using namespace GL;

void Shader::load(const String& src) {
  GLint compile_status = GL_TRUE;
  GLchar* srcStr((GLchar*)src.c_str());
  glShaderSource(_id, 1, &srcStr, NULL);
  glCompileShader(_id);
  glGetShaderiv(_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status != GL_TRUE) {
    GLchar buffer[512];
    GLsizei count;
    glGetShaderInfoLog(_id,512,&count,buffer);
    glDeleteShader(_id);
    throw Exception("Couldn't compile shader: "+String(buffer,count));
  }
}
Shader::Shader(File file, GLenum type) : _id(glCreateShader(type)) {
  load(file.open("r").readAll());
}
Shader::Shader(const String& src, GLenum type) : _id(glCreateShader(type)) {
  load(src);
}
Shader::~Shader() {
  glDeleteShader(_id);
}
