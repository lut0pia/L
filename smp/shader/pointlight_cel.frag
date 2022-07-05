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
  vec3 light_pos = model[3].xyz;
  float radius = length(model[0].xyz) * 0.83;
  GBufferSample gbuffer = sample_gbuffer(color_buffer, normal_buffer, depth_buffer);
  vec3 frag_to_light = light_pos - gbuffer.position;
  vec3 to_light_dir = normalize(frag_to_light);
  vec3 light_color = color.rgb * intensity;
  float dist = length(frag_to_light);
  fragcolor = cel_shading(gbuffer, light_color, to_light_dir, dist / radius);
}
