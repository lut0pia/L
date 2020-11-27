#include "OpenGLRenderer.h"

#include <spirv_cross/spirv_cross_c.h>

#include <L/src/parallelism/TaskSystem.h>
#include <L/src/text/String.h>

using namespace L;

static void error_callback(void*, const char* error) {
  warning("opengl: spirv-cross: %s", error);
}

ShaderImpl* OpenGLRenderer::create_shader(L::ShaderStage stage, const void* binary, size_t binary_size) {
  L_SCOPE_THREAD_MASK(1);

  // Create context.
  spvc_context context = nullptr;
  spvc_context_create(&context);

  // Set debug callback.
  spvc_context_set_error_callback(context, error_callback, nullptr);

  spvc_compiler compiler_glsl = nullptr;
  {
    spvc_parsed_ir ir = NULL;
    // Parse the SPIR-V.
    spvc_context_parse_spirv(context, (const SpvId*)binary, binary_size / 4, &ir);

    // Hand it off to a compiler instance and give it ownership of the IR.
    spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);
  }

  // Modify options.
  spvc_compiler_options options = nullptr;
  spvc_compiler_create_compiler_options(compiler_glsl, &options);
  spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, 450);
  spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_FALSE);
  spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_EMIT_PUSH_CONSTANT_AS_UNIFORM_BUFFER, SPVC_TRUE);
  spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_FIXUP_DEPTH_CONVENTION, SPVC_TRUE);
  spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_FLIP_VERTEX_Y, SPVC_TRUE);
  spvc_compiler_install_compiler_options(compiler_glsl, options);

  const char* result = nullptr;
  spvc_compiler_compile(compiler_glsl, &result);

  String result2 = result;
  result2.replace_all("layout(std140)", "layout(binding = 0, std140)");
  result2.replace_all("return _coords;", "return _coords * vec2(1.f, -1.f);");
  result = result2.begin();

  //printf("Cross-compiled source: %s\n", result);

  GLuint id = glCreateShader(to_gl_shader_stage(stage));
  glShaderSource(id, 1, &result, nullptr);
  glCompileShader(id);
  //glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, binary, (GLsizei)binary_size);
  //glSpecializeShader(id, "main", 0, nullptr, nullptr);

  // Frees all memory we allocated so far.
  spvc_context_destroy(context);

  GLint compile_status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status == GL_FALSE) {
    GLchar info_log[4096];
    GLsizei info_log_length = sizeof(info_log);
    glGetShaderInfoLog(id, info_log_length, &info_log_length, info_log);

    error("opengl: Could not compile shader: %s", info_log);

    glDeleteShader(id);

    return nullptr;
  }
  return (ShaderImpl*)(uintptr_t)id;
}
void OpenGLRenderer::destroy_shader(ShaderImpl* shader) {
  L_SCOPE_THREAD_MASK(1);

  glDeleteShader((GLuint)(uintptr_t)shader);
}
