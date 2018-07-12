layout(binding = 1) uniform Parameters {
  vec4 color;
};

layout(location = 0) in vec3 fnormal;
layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;

void main() {
    if(alpha(color.a))
      discard;
    ocolor.rgb = linearize(color.rgb);
    ocolor.a = 0.f; /* Metalness */
    onormal.xy = encodeNormal(fnormal);
    onormal.z = 0.8f; /* Roughness */
    onormal.w = 0.f; /* Emission */
}
