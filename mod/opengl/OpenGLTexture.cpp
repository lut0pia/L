#include "OpenGLRenderer.h"

using namespace L;

TextureImpl* OpenGLRenderer::create_texture(uint32_t width, uint32_t height, L::RenderFormat format, const void* data, size_t size) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLTexture* tex = Memory::new_type<OpenGLTexture>();
  tex->width = width;
  tex->height = height;
  tex->format = format;
  tex->target = height == width * 6 ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
  glCreateTextures(tex->target, 1, &tex->id);

  glTextureParameteri(tex->id, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(tex->id, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTextureParameteri(tex->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(tex->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  auto load = [&](OpenGLTexture* tex, GLenum target, const void* data, size_t size) {
    if(is_block_format(format)) {
      glCompressedTextureImage2DEXT(tex->id, target, 0, to_gl_internal_format(format), width, height, 0, GLsizei(size), data);
    } else {
      glTextureImage2DEXT(tex->id, target, 0, to_gl_internal_format(format), width, height, 0, to_gl_format(format), to_gl_type(format), data);
    }
  };

  if(tex->target == GL_TEXTURE_2D) { // Regular texture
    load(tex, GL_TEXTURE_2D, data, size);
  } else { // Cubemap
    height /= 6;
    size /= 6;
    for(uint32_t i = 0; i < 6; i++) {
      load(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (uint8_t*)data + size * i, size);
    }
  }

  return tex;
}
void OpenGLRenderer::destroy_texture(TextureImpl* tex) {
  OpenGLTexture* gl_tex = (OpenGLTexture*)tex;
  glDeleteTextures(1, &gl_tex->id);
}

void OpenGLRenderer::load_texture(TextureImpl* tex, const void* data, size_t, const L::Vector3i& offset, const L::Vector3i& extent) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLTexture* gl_tex = (OpenGLTexture*)tex;
  L_ASSERT(!is_block_format(gl_tex->format));
  glTextureSubImage2D(gl_tex->id, 0, offset.x(), offset.y(), extent.x(), extent.y(), to_gl_format(gl_tex->format), to_gl_type(gl_tex->format), data);
}
