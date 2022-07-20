#include "OpenGLRenderer.h"

using namespace L;

TextureImpl* OpenGLRenderer::create_texture(uint32_t width, uint32_t height, L::RenderFormat format, const void** mip_data, size_t* mip_size, size_t mip_count) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLTexture* tex = Memory::new_type<OpenGLTexture>();
  if(height == width * 6) {
    height /= 6;
    tex->target = GL_TEXTURE_CUBE_MAP;
  } else {
    tex->target = GL_TEXTURE_2D;
  }
  tex->width = width;
  tex->height = height;
  tex->format = format;
  glCreateTextures(tex->target, 1, &tex->id);

  glTextureParameteri(tex->id, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(tex->id, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTextureParameteri(tex->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(tex->id, GL_TEXTURE_MIN_FILTER, mip_count == 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);

  auto load = [&](GLenum target, GLint level, const void* data, size_t size) {
    if(is_block_format(format)) {
      glCompressedTextureImage2DEXT(tex->id, target, level, to_gl_internal_format(format), width >> level, height >> level, 0, GLsizei(size), data);
    } else {
      glTextureImage2DEXT(tex->id, target, level, to_gl_internal_format(format), width >> level, height >> level, 0, to_gl_format(format), to_gl_type(format), data);
    }
  };

  for(uint32_t mip = 0; mip < mip_count; mip++) {
    if(tex->target == GL_TEXTURE_2D) { // Regular texture
      load(GL_TEXTURE_2D, mip, mip_data ? mip_data[mip] : nullptr, mip_size ? mip_size[mip] : 0);
    } else { // Cubemap
      const size_t mip_face_size = mip_size[mip] / 6;
      for(uint32_t i = 0; i < 6; i++) {
        load(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, (uint8_t*)mip_data[mip] + mip_face_size * i, mip_face_size);
      }
    }
  }

  return tex;
}
void OpenGLRenderer::destroy_texture(TextureImpl* tex) {
  OpenGLTexture* gl_tex = (OpenGLTexture*)tex;
  glDeleteTextures(1, &gl_tex->id);
  Memory::delete_type(gl_tex);
}

void OpenGLRenderer::load_texture(TextureImpl* tex, const void* data, size_t, const L::Vector3i& offset, const L::Vector3i& extent, uint32_t mip_level) {
  L_SCOPE_THREAD_MASK(1);

  OpenGLTexture* gl_tex = (OpenGLTexture*)tex;
  L_ASSERT(!is_block_format(gl_tex->format));
  glTextureSubImage2D(gl_tex->id, mip_level, offset.x(), offset.y(), extent.x(), extent.y(), to_gl_format(gl_tex->format), to_gl_type(gl_tex->format), data);
}
