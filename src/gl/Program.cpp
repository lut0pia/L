#include "Program.h"

#include "GL.h"
#include "../hash.h"
#include "../stream/CFileStream.h"

using namespace L;
using namespace GL;

Program::Program(const Shader* s, uint32_t count) : _id(glCreateProgram()) {
  GLuint shaders[8];
  L_ASSERT(count<=L_COUNT_OF(shaders));
  for(uintptr_t i(0); i<count; i++)
    shaders[i] = s[i].id();
  link(shaders, count);
}
Program::Program(const Shader& s1) : _id(glCreateProgram()) {
  const GLuint shaders[] = {s1.id()};
  link(shaders, 1);
}
Program::Program(const Shader& s1, const Shader& s2) : _id(glCreateProgram()) {
  const GLuint shaders[] = {s1.id(),s2.id()};
  link(shaders, 2);
}
Program::Program(const Shader& s1, const Shader& s2, const Shader& s3) : _id(glCreateProgram()) {
  const GLuint shaders[] = {s1.id(),s2.id(),s3.id()};
  link(shaders, 3);
}
Program::~Program() {
  glDeleteProgram(_id);
}

void Program::link(const GLuint* shaders, uint32_t count) {
  for(uintptr_t i(0); i<count; i++)
    glAttachShader(_id, shaders[i]);

  glLinkProgram(_id);
  
  for(uintptr_t i(0); i<count; i++)
    glDetachShader(_id, shaders[i]);
}
bool Program::check() const {
  GLint link_status;
  glGetProgramiv(_id, GL_LINK_STATUS, &link_status);
  if(link_status != GL_TRUE) {
    GLchar buffer[2048];
    GLsizei count;
    glGetProgramInfoLog(_id, sizeof(buffer), &count, buffer);
    err << "Couldn't link program:\n";
    err.write(buffer, count);
  }
  return link_status == GL_TRUE;
}
void Program::use() const {
  glUseProgram(_id);
}
GLuint Program::uniformLocation(const char* name) {
  KeyValue<uint32_t, GLuint>* it(_uniformLocation.find(fnv1a(name)));
  if(it) return it->value();
  else {
    GLuint location(glGetUniformLocation(_id, name));
    L_ASSERT(location>=0);
    return _uniformLocation[fnv1a(name)] = location;
  }
}
GLuint Program::uniformBlockIndex(const char* name) {
  GLuint wtr(glGetUniformBlockIndex(_id, name));
  L_ASSERT(wtr != GL_INVALID_INDEX);
  return wtr;
}
void Program::uniformBlockBinding(const char* name, GLuint binding) {
  glUniformBlockBinding(_id, uniformBlockIndex(name), binding);
}
void Program::uniform(const char* name, int v) {
  glUniform1i(uniformLocation(name), v);
}
void Program::uniform(const char* name, unsigned int v) {
  glUniform1ui(uniformLocation(name), v);
}
void Program::uniform(const char* name, float v) {
  glUniform1f(uniformLocation(name), v);
}
void Program::uniform(const char* name, float x, float y) {
  glUniform2f(uniformLocation(name), x, y);
}
void Program::uniform(const char* name, float x, float y, float z) {
  glUniform3f(uniformLocation(name), x, y, z);
}
void Program::uniform(const char* name, float x, float y, float z, float w) {
  glUniform4f(uniformLocation(name), x, y, z, w);
}
void Program::uniform(const char* name, const Matrix44f& m) {
  glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, m.array());
}
void Program::uniform(const char* name, const Texture& texture, GLenum unit) {
  glUniform1i(uniformLocation(name), unit-GL_TEXTURE0);
  glActiveTexture(unit);
  texture.bind();
}

void Program::unuse() {
  glUseProgram(0);
}
