#include <shader/alpha_lib.glsl>

layout(location = 0) in vec2 ftexcoords;
layout(location = 1) in vec3 fnormal;

layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;

layout(binding = 2) uniform sampler2D tex;

void main() {
  vec4 color = texture(tex,ftexcoords);
  if(alpha(color.a)) {
    discard;
  }
  ocolor.rgb = linearize(color.rgb);
  ocolor.a = 0.f; // Metalness
  onormal.xy = encodeNormal(fnormal);
  onormal.z = 1.f; // Roughness
  onormal.w = 0.f; // Emission
}
