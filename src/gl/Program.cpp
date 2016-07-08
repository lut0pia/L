#include "Program.h"

#include "GL.h"
#include "../hash.h"

using namespace L;
using namespace GL;

Program::Program(const Shader& s1) : _id(glCreateProgram()) {
  attach(s1);
  link();
  detach(s1);
}
Program::Program(const Shader& s1,const Shader& s2) : _id(glCreateProgram()) {
  attach(s1);
  attach(s2);
  link();
  detach(s1);
  detach(s2);
}
Program::Program(const Shader& s1,const Shader& s2,const Shader& s3) : _id(glCreateProgram()) {
  attach(s1);
  attach(s2);
  attach(s3);
  link();
  detach(s1);
  detach(s2);
  detach(s3);
}
Program::~Program() {
  glDeleteProgram(_id);
}
void Program::attach(const Shader& shader) {
  glAttachShader(_id,shader.id());
}
void Program::detach(const Shader& shader) {
  glDetachShader(_id,shader.id());
}

void Program::link() {
  glLinkProgram(_id);
  GLint p;
  glGetProgramiv(_id,GL_LINK_STATUS,&p);
  if(p != GL_TRUE)
    L_ERROR("Couldn't link program");
}
void Program::use() const {
  glUseProgram(_id);
}
GLuint Program::uniformLocation(const char* name) {
  KeyValue<uint32_t,GLuint>* it(_uniformLocation.find(fnv1a(name)));
  if(it) return it->value();
  else {
    GLuint location(glGetUniformLocation(_id,name));
    L_ASSERT(location>=0);
    return _uniformLocation[fnv1a(name)] = location;
  }
}
GLuint Program::uniformBlockIndex(const char* name){
  GLuint wtr(glGetUniformBlockIndex(_id,name));
  L_ASSERT(wtr != GL_INVALID_INDEX);
  return wtr;
}
void Program::uniformBlockBinding(const char* name,GLuint binding){
  glUniformBlockBinding(_id,uniformBlockIndex(name),binding);
}
void Program::uniform(const char* name,float v) {
  glUniform1f(uniformLocation(name),v);
}
void Program::uniform(const char* name,float x,float y,float z) {
  glUniform3f(uniformLocation(name),x,y,z);
}
void Program::uniform(const char* name,const Vector3f& p) {
  uniform(name,p.x(),p.y(),p.z());
}
void Program::uniform(const char* name,const Matrix44f& m) {
  glUniformMatrix4fv(uniformLocation(name),1,GL_FALSE,m.array());
}
void Program::uniform(const char* name,const Texture& texture,GLenum unit) {
  glUniform1i(uniformLocation(name),unit-GL_TEXTURE0);
  glActiveTexture(unit);
  texture.bind();
}

void Program::unuse() {
  glUseProgram(0);
}
