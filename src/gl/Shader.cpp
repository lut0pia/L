#include "Shader.h"

#include "../stream/CFileStream.h"

using namespace L;
using namespace GL;

void Shader::load(const char** src, uint32_t count, GLint* lengths) {
  GLint compile_status(GL_TRUE);
  glShaderSource(_id, count, (const GLchar**)src, lengths);
  glCompileShader(_id);
  glGetShaderiv(_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status != GL_TRUE) {
    GLchar buffer[2048];
    GLsizei count;
    glGetShaderInfoLog(_id, sizeof(buffer), &count, buffer);
    glDeleteShader(_id);
    L_ERRORF("Couldn't compile shader: %.*s", count, buffer);
  }
}
Shader::Shader(const char* src, GLenum type) : _id(glCreateShader(type)) {
  load(src);
}
Shader::Shader(const char** src, uint32_t count, GLenum type) : _id(glCreateShader(type)) {
  load(src, count);
}
Shader::Shader(const char** src, GLint* lengths, uint32_t count, GLenum type) : _id(glCreateShader(type)) {
  load(src, count, lengths);
}
Shader::~Shader() {
  glDeleteShader(_id);
}
