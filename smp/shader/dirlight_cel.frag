#include <shader/light_lib.glsl>

layout(location = 0) out vec4 fragcolor;

layout(binding = 1) uniform Parameters {
  vec4 color;
  float intensity;
};

layout(binding = 2) uniform sampler2D color_buffer;
layout(binding = 3) uniform sampler2D normal_buffer;
layout(binding = 4) uniform sampler2D depth_buffer;

void main() {
  GBufferSample gbuffer = sample_gbuffer(color_buffer, normal_buffer, depth_buffer);
  vec3 world_dir = mat3(model) * vec3(0,1,0);
  vec3 to_light_dir = normalize(-world_dir);
  vec3 light_color = color.rgb * intensity;
  fragcolor = cel_shading(gbuffer, light_color, to_light_dir, 0.f);
}
