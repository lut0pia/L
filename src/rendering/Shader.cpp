#include "Shader.h"

#include "../stream/CFileStream.h"

using namespace L;

void Shader::load(const char** src, uint32_t count, GLint* lengths) {
  glShaderSource(_id, count, (const GLchar**)src, lengths);
  glCompileShader(_id);
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
bool Shader::check() const {
  GLint compile_status(GL_TRUE);
  glGetShaderiv(_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status != GL_TRUE) {
    GLchar buffer[2048];
    GLsizei count;
    glGetShaderInfoLog(_id, sizeof(buffer), &count, buffer);
    err << "Couldn't compile shader:\n";
    err.write(buffer, count);
  }
  return compile_status == GL_TRUE;
}
