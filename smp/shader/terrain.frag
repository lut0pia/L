layout(location = 0) in vec3 ftexcoords;
layout(location = 1) in vec3 fnormal;

layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;

layout(binding = 4) uniform sampler2D color_tex;

void main() {
  ocolor.rgb = mix(vec3(0, 0.3, 0), vec3(0.8, 0.8, 1), ftexcoords.z);
  ocolor.rgb *= mix(0.75, 1.0, texture(color_tex, ftexcoords.xy * 4.f).x);
  ocolor.a = 0.f; // Metalness
  onormal.xy = encodeNormal(fnormal);
  onormal.z = 0.8f; // Roughness
  onormal.w = 0.f; // Emission
}
