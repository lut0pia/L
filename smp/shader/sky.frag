layout(location = 0) in vec3 fnormal;

layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;

layout(binding = 1) uniform samplerCube cubemap;

void main() {
  vec4 color = texture(cubemap, fnormal);
  ocolor.rgb = linearize(color.rgb);
  ocolor.a = 0.f; // Metalness
  onormal.xy = encodeNormal(vec3(0.f, 0.f, 1.f));
  onormal.z = 0.f; // Roughness
  onormal.w = 1.f; // Emission
}
