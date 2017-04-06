#pragma once

#include "../containers/Ref.h"
#include "../font/Font.h"
#include "../gl/Texture.h"
#include "../gl/Mesh.h"
#include "../script/Context.h"

namespace L {
  namespace Resource {
    void update();
    const Ref<GL::Texture>& texture(const char* filepath);
    const Ref<GL::Mesh>& mesh(const char* filepath);
    const Ref<Script::CodeFunction>& script(const char* filepath);
    const Ref<Font>& font(const char* filepath = "default");
  };
}
