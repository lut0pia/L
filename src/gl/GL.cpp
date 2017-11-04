#include "GL.h"

#include "../constants.h"
#include "Mesh.h"
#include "MeshBuilder.h"
#include "Program.h"
#include "../stream/CFileStream.h"
#include "../image/Color.h"
#include "../engine/SharedUniform.h"
#include "../text/Symbol.h"

using namespace L;
using namespace GL;

// Define GL functions
#define L_GL_FUNC(type,name) type L::name;
#include "gl_functions.def"
#undef L_GL_FUNC

static Array<Symbol> extensions;

static void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void *user_param) {
  if(type != GL_DEBUG_TYPE_PERFORMANCE && type != GL_DEBUG_TYPE_OTHER)
    err << "OpenGL debug: " << message << '\n';
}
void GL::init() {
  // Load GL functions
#define L_GL_FUNC(type,name) name = type(load_function(#name));
#include "gl_functions.def"
#undef L_GL_FUNC

  out << "OpenGL:" << (const char*)glGetString(GL_VERSION) << ' ' << (const char*)glGetString(GL_RENDERER) << '\n';

  { // Fetch supported extensions
    GLint num_exts;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts);
    for(GLint i(0); i<num_exts; i++)
      extensions.push((const char*)glGetStringi(GL_EXTENSIONS, i));
  }

#ifdef L_DEBUG
  glDebugMessageCallback(debug_callback, NULL);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
}

void GL::draw(GLenum mode, GLsizei count) {
  static GLuint dummy_vao;
  L_DO_ONCE
    glCreateVertexArrays(1, &dummy_vao);
  glBindVertexArray(dummy_vao);
  glDrawArrays(mode, 0, count);
}
#define L_VERTEX_SHADER \
"#version 330 core\n" \
L_SHAREDUNIFORM \
"layout (location = 0) in vec3 modelPosition;" \
"uniform mat4 model;" \
"smooth out vec4 position;" \
"void main(){" \
  "position = model * vec4(modelPosition,1.0);" \
  "gl_Position = viewProj * position;" \
"}"
Program& GL::baseProgram() {
  static Program program(Shader(L_VERTEX_SHADER,GL_VERTEX_SHADER),Shader(
    "#version 330 core\n"
    L_SHAREDUNIFORM
    "layout(location = 0) out vec3 ocolor;"
    "layout(location = 1) out vec3 onormal;"
    "smooth in vec4 position;"
    "void main(){"
    "vec3 normal = cross(dFdx(position.xyz),dFdy(position.xyz)).xyz;"
    "if(isnan(normal.x) || length(normal)<=0.f) normal = eye.xyz-position.xyz;"
    "onormal.xy = encodeNormal(normal);"
    "ocolor = vec3(1,1,1);"
    "}",GL_FRAGMENT_SHADER));
  return program;
}
Program& GL::baseColorProgram() {
  static Program program(Shader(L_VERTEX_SHADER,GL_VERTEX_SHADER),Shader(
    "#version 330 core\n"
    L_SHAREDUNIFORM
    L_SHADER_LIB
    "layout(location = 0) out vec3 ocolor;"
    "layout(location = 1) out vec3 onormal;"
    "uniform vec4 color;"
    "smooth in vec4 position;"
    "void main(){"
    "if(alpha(color.a)) discard;"
    "vec3 normal = cross(dFdx(position.xyz),dFdy(position.xyz)).xyz;"
    "if(isnan(normal.x) || length(normal)<=0.f) normal = eye.xyz-position.xyz;"
    "onormal.xy = encodeNormal(normal);"
    "ocolor = color.rgb;"
    "}",GL_FRAGMENT_SHADER));
  return program;
}
const Mesh& GL::quad(){
  static const GLfloat quad[] = {
    -1,-1,0,
    1,-1,0,
    -1,1,0,
    1,1,0,
  };
  static Mesh mesh(GL_TRIANGLE_STRIP,4,quad,sizeof(quad),{Mesh::Attribute{0,3,GL_FLOAT,GL_FALSE,0,0}});
  return mesh;
}
const Mesh& GL::wireCube(){
  static const GLfloat wireCube[] = {
    // Bottom face
    -1,-1,-1, -1,1,-1,
    -1,-1,-1, 1,-1,-1,
    1,-1,-1,  1,1,-1,
    -1,1,-1,  1,1,-1,
    // Top face
    -1,-1,1, -1,1,1,
    -1,-1,1, 1,-1,1,
    1,-1,1,  1,1,1,
    -1,1,1,  1,1,1,
    // Sides
    -1,-1,-1, -1,-1,1,
    -1,1,-1,  -1,1,1,
    1,-1,-1,  1,-1,1,
    1,1,-1,   1,1,1,
  };
  static Mesh mesh(GL_LINES,12*2,wireCube,sizeof(wireCube),{Mesh::Attribute{0,3,GL_FLOAT,GL_FALSE,0,0}});
  return mesh;
}
const Mesh& GL::wireSphere(){
  static const float d(sqrt(.5f));
  static const GLfloat wireSphere[] = {
    // X circle
    0,0,-1, 0,-d,-d, 0,-d,-d, 0,-1,0,
    0,-1,0, 0,-d,d,  0,-d,d,  0,0,1,
    0,0,1,  0,d,d,   0,d,d,   0,1,0,
    0,1,0,  0,d,-d,  0,d,-d,  0,0,-1,
    // Y circle
    0,0,-1, -d,0,-d, -d,0,-d, -1,0,0,
    -1,0,0, -d,0,d,  -d,0,d,  0,0,1,
    0,0,1,  d,0,d,   d,0,d,   1,0,0,
    1,0,0,  d,0,-d,  d,0,-d,  0,0,-1,
    // Z circle
    0,-1,0, -d,-d,0, -d,-d,0, -1,0,0,
    -1,0,0, -d,d,0,  -d,d,0,  0,1,0,
    0,1,0,  d,d,0,   d,d,0,   1,0,0,
    1,0,0, d,-d,0,   d,-d,0,  0,-1,0,
  };
  static Mesh mesh(GL_LINES,24*2,wireSphere,sizeof(wireSphere),{Mesh::Attribute{0,3,GL_FLOAT,GL_FALSE,0,0}});
  return mesh;
}
