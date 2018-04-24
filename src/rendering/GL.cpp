#include "GL.h"

#include "../constants.h"
#include "Mesh.h"
#include "MeshBuilder.h"
#include "Program.h"
#include "shader_lib.h"
#include "../stream/CFileStream.h"
#include "../image/Color.h"
#include "../text/Symbol.h"

using namespace L;

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

  out << "OpenGL: " << (const char*)glGetString(GL_VERSION) << ' ' << (const char*)glGetString(GL_RENDERER) << '\n';

  { // Fetch supported extensions
    GLint num_exts;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts);
    for(GLint i(0); i<num_exts; i++)
      extensions.push((const char*)glGetStringi(GL_EXTENSIONS, i));
  }

#define L_GL_STATIC_FUNC(type,name) static type name(type(load_function(#name)))
  if(extensions.find("GL_ARB_direct_state_access")==uintptr_t(-1)) {
    L_GL_STATIC_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);
    L_GL_STATIC_FUNC(PFNGLBUFFERDATAPROC, glBufferData);
    L_GL_STATIC_FUNC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
    glCreateBuffers = glGenBuffers;
    glNamedBufferData = [](GLuint buffer, GLsizeiptr size, const void* data, GLenum usage) {
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    };
    glNamedBufferSubData = [](GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    };

    L_GL_STATIC_FUNC(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
    L_GL_STATIC_FUNC(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
    L_GL_STATIC_FUNC(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
    L_GL_STATIC_FUNC(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    glCreateFramebuffers = glGenFramebuffers;
    glNamedFramebufferDrawBuffers = [](GLuint framebuffer, GLsizei n, const GLenum* bufs) {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
      glDrawBuffers(n, bufs);
    };
    glNamedFramebufferTexture = [](GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);
    };
    glCheckNamedFramebufferStatus = [](GLuint framebuffer, GLenum target) -> GLenum {
      glBindFramebuffer(target, framebuffer);
      return glCheckFramebufferStatus(target);
    };

    L_GL_STATIC_FUNC(PFNGLGENTEXTURESEXTPROC, glGenTexturesEXT);
    L_GL_STATIC_FUNC(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
    glCreateTextures = [](GLenum target, GLsizei n, GLuint* textures) {
      glGenTexturesEXT(n, textures);
      for(uintptr_t i(0); i<n; i++)
        glBindTexture(target, textures[i]);
    };
    glTextureImage2DEXT = [](GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data) {
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, data);
    };
    glTextureSubImage2D = [](GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data) {
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, data);
    };
    glTextureParameteri = [](GLuint texture, GLenum pname, GLint param) {
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexParameteri(GL_TEXTURE_2D, pname, param);
    };
    glGenerateTextureMipmap = [](GLuint texture) {
      glBindTexture(GL_TEXTURE_2D, texture);
      glGenerateMipmap(GL_TEXTURE_2D);
    };

    L_GL_STATIC_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    L_GL_STATIC_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    glCreateVertexArrays = glGenVertexArrays;
    glVertexArrayElementBuffer = [](GLuint vaobj, GLuint buffer) {
      glBindVertexArray(vaobj);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    };
    glEnableVertexArrayAttrib = [](GLuint vaobj, GLuint index) {
      glBindVertexArray(vaobj);
      glEnableVertexAttribArray(index);
    };
  }

  L_ASSERT(extensions.find("GL_ARB_uniform_buffer_object")!=uintptr_t(-1));

#ifdef L_DEBUG
  glDebugMessageCallback(debug_callback, NULL);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  glEnable(GL_CULL_FACE);
}

void GL::draw(GLenum mode, GLsizei count) {
  static GLuint dummy_vao;
  L_DO_ONCE
    glCreateVertexArrays(1, &dummy_vao);
  glBindVertexArray(dummy_vao);
  glDrawArrays(mode, 0, count);
}
