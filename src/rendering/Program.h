#pragma once

#include "GL.h"
#include "Shader.h"
#include "Texture.h"
#include "../container/Table.h"
#include "../image/Color.h"
#include "../math/Matrix.h"

namespace L {
  class Program {
    L_NOCOPY(Program)
  private:
    GLuint _id;
    Table<uint32_t, GLint> _uniform_location;
  public:
    Program(const Shader*, uint32_t);
    Program(const Shader&);
    Program(const Shader&, const Shader&);
    Program(const Shader&, const Shader&, const Shader&);
    ~Program();
    void link(const GLuint*, uint32_t count);
    bool check() const;
    void use() const;
    GLint uniform_location(const char* name);
    GLuint uniform_block_index(const char* name);
    void uniform_block_binding(const char* name, GLuint binding);
    void uniform(const char* name, int);
    void uniform(const char* name, float);
    void uniform(const char* name, float, float);
    void uniform(const char* name, float, float, float);
    void uniform(const char* name, float, float, float, float);
    inline void uniform(const char* name, const Vector2f& v) { uniform(name, v.x(), v.y()); }
    inline void uniform(const char* name, const Vector3f& v) { uniform(name, v.x(), v.y(), v.z()); }
    inline void uniform(const char* name, const Vector4f& v) { uniform(name, v.x(), v.y(), v.z(), v.w()); }
    inline void uniform(const char* name, const Color& c) { uniform(name, c.rf(), c.gf(), c.bf(), c.af()); }
    void uniform(const char* name, const Matrix44f&);
    inline void uniform(const char* name, const Texture& texture, GLenum unit = GL_TEXTURE0) {
      uniform(uniform_location(name), texture, unit);
    }

    void uniform(GLint location, const Texture&, GLenum unit = GL_TEXTURE0);

    static Program& default();
    static Program& default_color();
  };
}
