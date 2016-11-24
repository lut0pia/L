#include "Shader.h"

#include "../streams/FileStream.h"

using namespace L;
using namespace GL;

void Shader::load(const char* src) {
  GLint compile_status(GL_TRUE);
  glShaderSource(_id,1,(const GLchar**)&src,nullptr);
  glCompileShader(_id);
  glGetShaderiv(_id,GL_COMPILE_STATUS,&compile_status);
  if(compile_status != GL_TRUE) {
    GLchar buffer[2048];
    GLsizei count;
    glGetShaderInfoLog(_id,sizeof(buffer),&count,buffer);
    glDeleteShader(_id);
    L_ERROR("Couldn't compile shader: %.*s",count,buffer);
  }
}
Shader::Shader(File file,GLenum type) : _id(glCreateShader(type)) {
  FileStream fs(file.path(),"rb");
  String src;
  while(!fs.end()) {
    char c(fs.get());
    if(c==EOF) break;
    else src.push(c);
  }
  load(src);
}
Shader::Shader(const char* src,GLenum type) : _id(glCreateShader(type)) {
  load(src);
}
Shader::~Shader() {
  glDeleteShader(_id);
}
