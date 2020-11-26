layout(location = 0) in vec3 fposition;
layout(location = 1) in vec2 ftexcoords;
layout(location = 2) in vec3 fnormal;

layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D mat_tex;

vec3 derive_normal(vec3 p, vec3 n, float h){
  vec3 dpdx = dFdx(p);
  vec3 dpdy = dFdy(p);
  float dhdx = dFdx(h);
  float dhdy = dFdy(h);
  vec3 r1 = cross(dpdy, n);
  vec3 r2 = cross(n, dpdx);
  vec3 g = (r1 * dhdx + r2 * dhdy) / dot(dpdx, r1);
  return normalize(n - g);
}

void main() {
  vec4 color = texture(tex,ftexcoords);
  if(alpha(color.a)) discard;
  vec4 material = texture(mat_tex,ftexcoords);
  ocolor.rgb = linearize(color.rgb);
  ocolor.a = material.y; /* Metalness */
  onormal.xy = encodeNormal(derive_normal(fposition, fnormal, material.x));
  onormal.z = material.z; /* Roughness */
  onormal.w = material.w; /* Emission */
}
