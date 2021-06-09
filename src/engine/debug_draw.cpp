#include "debug_draw.h"

#include "../component/Camera.h"
#include "Engine.h"
#include "Resource.inl"

using namespace L;

#if !L_RLS

struct DebugDrawing {
  Material material;
  Matrix44f model;
};

static Array<DebugDrawing> debug_drawings;

static const Symbol debug_draw_color_frag_shader("debug_draw_color_frag_shader.dbg?stage=frag");
static const Symbol debug_draw_x_vert_shader("debug_draw_x_vert_shader.dbg?stage=vert");

static bool debug_shader_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  if(slot.ext != Symbol("dbg")) {
    return false;
  }

  String source;

  if(slot.id == debug_draw_color_frag_shader) {
    source =
      "layout(binding = 1) uniform Parameters {"
      "vec4 color;"
      "};"

      "layout(location = 0) out vec4 ocolor;"
      "layout(location = 1) out vec4 onormal;"

      "void main() {"
      "ocolor.rgb = linearize(color.rgb);"
      "ocolor.a = 0.f; /* Metalness */"
      "onormal.xy = encodeNormal(vec3(0.f, 0.f, 1.f));"
      "onormal.z = 0.8f; /* Roughness */"
      "onormal.w = 0.f; /* Emission */"
      "}";
  } else if(slot.id == debug_draw_x_vert_shader) {
    source =
      "void main() {"
      "vec4 position = model * vec4(gl_VertexIndex, 0.f, 0.f, 1.f);"
      "gl_Position = viewProj * position;"
      "}";
  } else {
    return false;
  }

  slot.source_buffer = Buffer(source.begin(), source.size());
  slot.ext = "glsl";
  return ResourceLoading<Shader>::load_internal(slot, intermediate);
}

static void debug_draw_render(const Camera& camera, const RenderPassImpl* render_pass) {
  if(render_pass == Renderer::get()->get_geometry_pass()) {
    for(DebugDrawing& debug_drawing : debug_drawings) {
      debug_drawing.material.update();
      debug_drawing.material.draw(camera, render_pass, debug_drawing.model);
    }
    debug_drawings.clear();
  }
}

void L::init_debug_draw() {
  Engine::add_render(debug_draw_render);
  ResourceLoading<Shader>::add_loader(debug_shader_loader);
}

void L::debug_draw_line(const Vector3f& a, const Vector3f& b, const Color& color) {
  static Resource<Shader> debug_draw_x_vert_shader_res = debug_draw_x_vert_shader;
  static Resource<Shader> debug_draw_color_frag_shader_res = debug_draw_color_frag_shader;
  debug_drawings.push();
  DebugDrawing& debug_drawing = debug_drawings.back();
  debug_drawing.material.shader(ShaderStage::Vertex, debug_draw_x_vert_shader_res);
  debug_drawing.material.shader(ShaderStage::Fragment, debug_draw_color_frag_shader_res);
  debug_drawing.material.vertex_count(2);
  debug_drawing.material.topology(PrimitiveTopology::LineStrip);
  debug_drawing.material.color("color", color);
  debug_drawing.model = translation_matrix(a);
  debug_drawing.model.vector<3>(0) = b - a;
}

#endif
