#include "Shader.h"

#include "../Exception.h"

using namespace L;
using namespace GL;

void Shader::load(const char* src) {
  GLint compile_status(GL_TRUE);
  glShaderSource(_id, 1, &(const GLchar*)src, NULL);
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
Shader::Shader(const char* src, GLenum type) : _id(glCreateShader(type)) {
  load(src);
}
Shader::~Shader() {
  glDeleteShader(_id);
}
