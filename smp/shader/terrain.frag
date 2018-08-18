layout(location = 0) in vec2 ftexcoords;
layout(location = 1) in vec3 fnormal;

layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;

void main() {
  ocolor.rgb = vec3(1,1,1);
  ocolor.a = 0.f; // Metalness
  onormal.xy = encodeNormal(fnormal);
  onormal.z = 0.8f; // Roughness
  onormal.w = 0.f; // Emission
}
