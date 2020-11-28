#include "OpenGLRenderer.h"

#if L_WINDOWS
#include "wglext.h"
#endif

#include <L/src/dev/debug.h>
#include <L/src/dev/profiling.h>
#include <L/src/macros.h>
#include <L/src/rendering/RenderPass.h>
#include <L/src/system/Window.h>

using namespace L;

// Define GL functions
#define L_GL_FUNC(type,name) type name;
#include "gl_functions.def"
#undef L_GL_FUNC

static Array<Symbol> extensions;

static void APIENTRY debug_callback(GLenum, GLenum type, GLuint, GLenum, GLsizei, const char* message, const void*) {
  if(type != GL_DEBUG_TYPE_PERFORMANCE && type != GL_DEBUG_TYPE_OTHER) {
    warning("opengl: %s", message);
  }
}
void OpenGLRenderer::init(const char*, uintptr_t, uintptr_t data2) {
  // Init surface
  {
#if L_WINDOWS
    HWND hWnd = (HWND)data2;
    HDC hDC = GetDC(hWnd);
    _hdc = (void*)hDC;

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd)); // Initialize pixel format descriptor
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 0;
    pfd.cStencilBits = 0;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixel_format = ChoosePixelFormat(hDC, &pfd);
    if(!pixel_format) {
      error("opengl: ChoosePixelFormat failed");
    }

    if(!SetPixelFormat(hDC, pixel_format, &pfd)) {
      error("opengl: SetPixelFormat failed");
    }

    HGLRC hRCFake = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRCFake);

    int context_attributes[] =
    {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 0,
#ifdef L_DBG
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0 // End of attributes list
    };

    static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB(
      PFNWGLCREATECONTEXTATTRIBSARBPROC(wglGetProcAddress("wglCreateContextAttribsARB")));
    HGLRC hRC = wglCreateContextAttribsARB(hDC, 0, context_attributes);
    if(!hRC) {
      error("opengl: wglCreateContextAttribsARB failed");
    }

    wglMakeCurrent(hDC, hRC);
    wglDeleteContext(hRCFake);
#elif L_LINUX

#endif
  }

  // Load GL functions
#define L_GL_FUNC(type,name) name = type(load_function(#name));
#include "gl_functions.def"
#undef L_GL_FUNC

  log("OpenGL version: %s", (const char*)glGetString(GL_VERSION));
  log("GPU: %s", (const char*)glGetString(GL_RENDERER));

  { // Fetch supported extensions
    GLint num_exts;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts);
    for(GLint i(0); i < num_exts; i++)
      extensions.push((const char*)glGetStringi(GL_EXTENSIONS, i));
  }

#define L_GL_STATIC_FUNC(type,name) static type name(type(load_function(#name)))
  if(extensions.find(Symbol("GL_ARB_direct_state_access")) == nullptr) {
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
    glCreateTextures = [](GLenum target, GLsizei n, GLuint* textures) {
      glGenTexturesEXT(n, textures);
      for(GLsizei i = 0; i < n; i++) {
        glBindTexture(target, textures[i]);
      }
    };
    glTextureImage2DEXT = [](GLuint texture, GLenum, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data) {
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

  L_ASSERT(extensions.find(Symbol("GL_ARB_uniform_buffer_object")) != nullptr);

#if !L_RLS
  glDebugMessageCallback(debug_callback, NULL);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  glCreateVertexArrays(1, &_dummy_vao);

  glFrontFace(GL_CCW);
  //glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
}
void OpenGLRenderer::recreate_swapchain() {

}
void OpenGLRenderer::draw(
  L::RenderCommandBuffer*,
  L::PipelineImpl* pipeline,
  L::DescriptorSetImpl* desc_set,
  const float* model,
  L::MeshImpl* mesh,
  uint32_t vertex_count,
  uint32_t index_offset) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLPipeline* gl_pipeline = (OpenGLPipeline*)pipeline;
  OpenGLDescriptorSet* gl_desc_set = (OpenGLDescriptorSet*)desc_set;
  OpenGLMesh* gl_mesh = (OpenGLMesh*)mesh;

  if(gl_pipeline->cull_mode != GL_INVALID_ENUM) {
    glEnable(GL_CULL_FACE);
    glCullFace(gl_pipeline->cull_mode);
  } else {
    glDisable(GL_CULL_FACE);
  }

  if(gl_pipeline->blend_src != GL_ONE || gl_pipeline->blend_dst != GL_ZERO) {
    glEnable(GL_BLEND);
    glBlendFunc(gl_pipeline->blend_src, gl_pipeline->blend_dst);
  } else {
    glDisable(GL_BLEND);
  }

  glUseProgram(gl_pipeline->id);

  // Bind textures
  GLenum texture_unit = GL_TEXTURE0;
  for(const auto& tex_pair : gl_desc_set->textures) {
    GLint location = glGetUniformLocation(gl_pipeline->id, tex_pair.key().name);
    glUniform1i(location, texture_unit - GL_TEXTURE0);
    glActiveTexture(texture_unit);
    glBindTexture(tex_pair.value()->target, tex_pair.value()->id);
    texture_unit++;
  }

  // Bind uniform buffers
  for(const auto& ub_pair : gl_desc_set->uniform_buffers) {
    //GLuint index = glGetUniformBlockIndex(gl_pipeline->id, ub_pair.key().name);
    //if(index != GL_INVALID_INDEX) {
      glBindBufferBase(GL_UNIFORM_BUFFER, ub_pair.key().binding, ub_pair.value()->id);
    //}
  }

  if(model) { // Load model matrix
    if(!gl_desc_set->constants) {
      gl_desc_set->constants = (OpenGLUniformBuffer*)create_uniform_buffer(sizeof(Matrix44f));
    }
    load_uniform_buffer(gl_desc_set->constants, model, sizeof(Matrix44f), 0);
    //GLuint index = glGetUniformBlockIndex(gl_pipeline->id, "Constants");
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, gl_desc_set->constants->id);
  }

  if(gl_mesh && gl_mesh->vao) {
    const GLvoid* gl_index_offset = (const GLvoid*)(uintptr_t(index_offset) * sizeof(uint16_t));
    glBindVertexArray(gl_mesh->vao);
    if(gl_mesh->eab) {
      glDrawElements(gl_pipeline->topology, vertex_count, GL_UNSIGNED_SHORT, gl_index_offset);
    } else {
      glDrawArrays(gl_pipeline->topology, 0, vertex_count);
    }
  } else {
    glBindVertexArray(_dummy_vao);
    glDrawArrays(gl_pipeline->topology, 0, vertex_count);
  }
}

RenderCommandBuffer* OpenGLRenderer::begin_render_command_buffer() {
  return nullptr;
}
void OpenGLRenderer::end_render_command_buffer() {

}
void OpenGLRenderer::begin_present_pass() {

}
void OpenGLRenderer::end_present_pass() {
#if L_WINDOWS
  SwapBuffers((HDC)_hdc);
#endif
}

void OpenGLRenderer::set_scissor(RenderCommandBuffer*, const L::Interval2i& scissor) {
  const Vector2i scissor_size = scissor.size();
  glScissor(scissor.min().x(), scissor.min().y(), scissor_size.x(), scissor_size.y());
}
void OpenGLRenderer::reset_scissor(RenderCommandBuffer*) {
  glScissor(0, 0, 1 << 14, 1 << 14);
}

void OpenGLRenderer::set_viewport(RenderCommandBuffer*, const Interval2i& viewport) {
  const Vector2i viewport_size = viewport.size();
  glViewport(viewport.min().x(), viewport.min().y(), viewport_size.x(), viewport_size.y());
}
void OpenGLRenderer::reset_viewport(RenderCommandBuffer*) {

}

GLuint OpenGLRenderer::to_gl_format(L::RenderFormat format) {
  switch(format) {
    case RenderFormat::R8_UNorm: return GL_RED;
    case RenderFormat::R8G8B8A8_UNorm: return GL_RGBA;
    case RenderFormat::B8G8R8A8_UNorm: return GL_BGRA;

    case RenderFormat::R16G16B16A16_UNorm: return GL_RGBA;
    case RenderFormat::R16G16B16A16_SFloat: return GL_RGBA;

    case RenderFormat::D24_UNorm_S8_UInt: return GL_DEPTH_STENCIL;

    default: L::error("opengl: Unknown render format");
  }
  return GL_INVALID_ENUM;
}
GLint OpenGLRenderer::to_gl_size(L::RenderFormat format) {
  switch(format) {
    case RenderFormat::R8_UNorm:

    case RenderFormat::D16_UNorm:
    case RenderFormat::R16_SFloat:
    case RenderFormat::R16_SInt:
    case RenderFormat::R16_UInt:

    case RenderFormat::D32_SFloat:
    case RenderFormat::R32_SFloat:
    case RenderFormat::R32_SInt:
    case RenderFormat::R32_UInt:
      return 1;

    case RenderFormat::R16G16_SFloat:
    case RenderFormat::R16G16_SInt:
    case RenderFormat::R16G16_UInt:

    case RenderFormat::R32G32_SFloat:
    case RenderFormat::R32G32_SInt:
    case RenderFormat::R32G32_UInt:
      return 2;

    case RenderFormat::R16G16B16_SFloat:
    case RenderFormat::R16G16B16_SInt:
    case RenderFormat::R16G16B16_UInt:

    case RenderFormat::R32G32B32_SFloat:
    case RenderFormat::R32G32B32_SInt:
    case RenderFormat::R32G32B32_UInt:
      return 3;

    case RenderFormat::R8G8B8A8_UNorm:
    case RenderFormat::B8G8R8A8_UNorm:

    case RenderFormat::R16G16B16A16_SFloat:
    case RenderFormat::R16G16B16A16_SInt:
    case RenderFormat::R16G16B16A16_UInt:
    case RenderFormat::R16G16B16A16_UNorm:

    case RenderFormat::R32G32B32A32_SFloat:
    case RenderFormat::R32G32B32A32_SInt:
    case RenderFormat::R32G32B32A32_UInt:
      return 4;

    default: L::error("opengl: Unknown size for render format");
  }
  return GL_INVALID_ENUM;
}
GLenum OpenGLRenderer::to_gl_type(L::RenderFormat format) {
  switch(format) {
    case RenderFormat::R8_UNorm:
    case RenderFormat::R8G8B8A8_UNorm:
    case RenderFormat::B8G8R8A8_UNorm:
      return GL_UNSIGNED_BYTE;

    case RenderFormat::R16_SFloat:
    case RenderFormat::R16G16_SFloat:
    case RenderFormat::R16G16B16_SFloat:
    case RenderFormat::R16G16B16A16_SFloat:
      return GL_HALF_FLOAT;

    case RenderFormat::R16G16B16A16_UInt:
    case RenderFormat::R16G16B16A16_UNorm:
      return GL_UNSIGNED_SHORT;

    case RenderFormat::R32_SFloat:
    case RenderFormat::R32G32_SFloat:
    case RenderFormat::R32G32B32_SFloat:
    case RenderFormat::R32G32B32A32_SFloat:
      return GL_FLOAT;

    case RenderFormat::D24_UNorm_S8_UInt: return GL_UNSIGNED_INT_24_8;

    default: L::error("opengl: Unknown type for render format");
  }
  return GL_INVALID_ENUM;
}
GLint OpenGLRenderer::to_gl_internal_format(L::RenderFormat format) {
  switch(format) {
    case RenderFormat::R8_UNorm: return GL_R8;
    case RenderFormat::R8G8B8A8_UNorm: return GL_RGBA8;

    case RenderFormat::R16G16B16A16_UNorm: return GL_RGBA16;
    case RenderFormat::R16G16B16A16_SFloat: return GL_RGBA16F;

    case RenderFormat::D24_UNorm_S8_UInt: return GL_DEPTH24_STENCIL8;

    case RenderFormat::BC1_RGB_UNorm_Block: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case RenderFormat::BC3_UNorm_Block: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    default: L::error("opengl: Unknown internal format for render format");
  }
  return GL_INVALID_ENUM;
}
bool OpenGLRenderer::to_gl_normalized(L::RenderFormat format) {
  switch(format) {
    case RenderFormat::R8G8B8A8_UNorm: return true;
    default: return false;
  }
  return false;
}
bool OpenGLRenderer::to_gl_integer(L::RenderFormat format) {
  switch(format) {
    case RenderFormat::R16G16B16A16_UInt: return true;
    default: return false;
  }
  return false;
}
GLuint OpenGLRenderer::to_gl_shader_stage(L::ShaderStage stage) {
  switch(stage) {
    case L::ShaderStage::Vertex: return GL_VERTEX_SHADER;
    case L::ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
    case L::ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
    default: L::error("opengl: Unknown shader stage");
  }
  return GL_INVALID_ENUM;
}
GLuint OpenGLRenderer::to_gl_topology(L::PrimitiveTopology topology) {
  switch(topology) {
    case PrimitiveTopology::PointList: return GL_POINTS;
    case PrimitiveTopology::LineList: return GL_LINES;
    case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
    case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
    case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
    case PrimitiveTopology::TriangleFan: return GL_TRIANGLE_FAN;
    default: L::error("opengl: Unknown primitive topology");
  }
  return GL_INVALID_ENUM;
}
GLuint OpenGLRenderer::to_gl_polygon_mode(L::PolygonMode polygon_mode) {
  switch(polygon_mode) {
    case PolygonMode::Fill: return GL_FILL;
    case PolygonMode::Line: return GL_LINE;
    case PolygonMode::Point: return GL_POINT;
    default: L::error("opengl: Unknown polygon mode");
  }
  return GL_INVALID_ENUM;
}
GLuint OpenGLRenderer::to_gl_cull_mode(L::CullMode cull_mode) {
  switch(cull_mode) {
    case CullMode::Undefined: return GL_INVALID_ENUM;
    case CullMode::None: return GL_INVALID_ENUM;
    case CullMode::Front: return GL_FRONT;
    case CullMode::Back: return GL_BACK;
    case CullMode::FrontAndBack: return GL_FRONT_AND_BACK;
  }
  return GL_INVALID_ENUM;
}
GLuint OpenGLRenderer::to_gl_blend_src(BlendMode blend_mode) {
  switch(blend_mode) {
    case BlendMode::Undefined: return GL_INVALID_ENUM;
    case BlendMode::None: return GL_ONE;
    case BlendMode::Mult: return GL_ZERO;
    case BlendMode::Add: return GL_ONE;
    case BlendMode::Alpha: return GL_SRC_ALPHA;
  }
  return GL_INVALID_ENUM;
}
GLuint OpenGLRenderer::to_gl_blend_dst(BlendMode blend_mode) {
  switch(blend_mode) {
    case BlendMode::Undefined: return GL_INVALID_ENUM;
    case BlendMode::None: return GL_ZERO;
    case BlendMode::Mult: return GL_SRC_COLOR;
    case BlendMode::Add: return GL_ONE;
    case BlendMode::Alpha: return GL_ONE_MINUS_SRC_ALPHA;
  }
  return GL_INVALID_ENUM;
}

void* OpenGLRenderer::load_function(const char* name) {
#if L_WINDOWS
  static HMODULE ogl_module = LoadLibraryA("opengl32.dll");
  void* p = wglGetProcAddress(name);
  switch(intptr_t(p)) {
    case 0:case 1:case 2:case 3: case -1:
      p = GetProcAddress(ogl_module, name);
  }
  return p;
#elif L_LINUX
  return (void*)glXGetProcAddressARB((const GLubyte*)name);
#endif
}
const char* OpenGLRenderer::error_str(GLenum result) {
#define CASE(v) case v: return #v
  switch(result) {
    CASE(GL_NO_ERROR);
    CASE(GL_INVALID_ENUM);
    CASE(GL_INVALID_VALUE);
    CASE(GL_INVALID_OPERATION);
    CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
    CASE(GL_OUT_OF_MEMORY);
    CASE(GL_STACK_UNDERFLOW);
    CASE(GL_STACK_OVERFLOW);
    default: return "unknown";
  }
#undef CASE
}