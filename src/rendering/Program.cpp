#include "Program.h"

#include "GL.h"
#include "../hash.h"
#include "shader_lib.h"
#include "../stream/CFileStream.h"

using namespace L;

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
GLint Program::uniform_location(const char* name) {
  uint32_t hash(fnv1a(name));
  if(auto* found = _uniform_location.find(hash)) return *found;
  else return _uniform_location[hash] = glGetUniformLocation(_id, name);
}
GLuint Program::uniform_block_index(const char* name) {
  GLuint wtr(glGetUniformBlockIndex(_id, name));
  L_ASSERT(wtr != GL_INVALID_INDEX);
  return wtr;
}
void Program::uniform_block_binding(const char* name, GLuint binding) {
  glUniformBlockBinding(_id, uniform_block_index(name), binding);
}
void Program::uniform(const char* name, int v) {
  glUniform1i(uniform_location(name), v);
}
void Program::uniform(const char* name, float v) {
  glUniform1f(uniform_location(name), v);
}
void Program::uniform(const char* name, float x, float y) {
  glUniform2f(uniform_location(name), x, y);
}
void Program::uniform(const char* name, float x, float y, float z) {
  glUniform3f(uniform_location(name), x, y, z);
}
void Program::uniform(const char* name, float x, float y, float z, float w) {
  glUniform4f(uniform_location(name), x, y, z, w);
}
void Program::uniform(const char* name, const Matrix44f& m) {
  glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, m.array());
}

void Program::uniform(GLint location, const Texture& texture, GLenum unit) {
  glUniform1i(location, unit-GL_TEXTURE0);
  glActiveTexture(unit);
  texture.bind();
}

#define L_VERTEX_SHADER \
L_GLSL_INTRO \
L_SHAREDUNIFORM \
"layout (location = 0) in vec3 modelPosition;" \
"uniform mat4 model;" \
"smooth out vec4 position;" \
"void main(){" \
  "position = model * vec4(modelPosition,1.0);" \
  "gl_Position = viewProj * position;" \
"}"
Program& Program::default() {
  static Program program(Shader(L_VERTEX_SHADER, GL_VERTEX_SHADER), Shader(
    L_GLSL_INTRO
    L_SHAREDUNIFORM
    "layout(location = 0) out vec4 ocolor;"
    "layout(location = 1) out vec4 onormal;"
    "smooth in vec4 position;"
    "void main(){"
    "vec3 normal = cross(dFdx(position.xyz),dFdy(position.xyz)).xyz;"
    "if(isnan(normal.x) || length(normal)<=0.f) normal = eye.xyz-position.xyz;"
    "ocolor.rgb = vec3(1,1,1);"
    "ocolor.a = 0.f; /* Metalness */"
    "onormal.xy = encodeNormal(normal);"
    "onormal.z = 0.5f; /* Roughness */"
    "onormal.w = 0.f; /* Emission */"
    "}", GL_FRAGMENT_SHADER));
  return program;
}
Program& Program::default_color() {
  static Program program(Shader(L_VERTEX_SHADER, GL_VERTEX_SHADER), Shader(
    L_GLSL_INTRO
    L_SHAREDUNIFORM
    L_SHADER_LIB
    "layout(location = 0) out vec4 ocolor;"
    "layout(location = 1) out vec4 onormal;"
    "uniform vec4 color;"
    "smooth in vec4 position;"
    "void main(){"
    "if(alpha(color.a)) discard;"
    "vec3 normal = cross(dFdx(position.xyz),dFdy(position.xyz)).xyz;"
    "if(isnan(normal.x) || length(normal)<=0.f) normal = eye.xyz-position.xyz;"
    "ocolor.rgb = linearize(color.rgb);"
    "ocolor.a = 0.f; /* Metalness */"
    "onormal.xy = encodeNormal(normal);"
    "onormal.z = 0.8f; /* Roughness */"
    "onormal.w = 0.f; /* Emission */"
    "}", GL_FRAGMENT_SHADER));
  return program;
}
