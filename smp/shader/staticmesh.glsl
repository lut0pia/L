#version 330 core
#stage vertex
layout(location = 0) in vec3 vposition;
layout(location = 1) in vec2 vtexcoords;
layout(location = 2) in vec3 vnormal;
uniform mat4 model;
out vec2 ftexcoords;
out vec3 fnormal;
out vec3 fposition;

void main() {
  ftexcoords = vtexcoords;
  fnormal = normalize(mat3(model) * vnormal);
  vec4 position = model * vec4(vposition,1.0);
  fposition = position.xyz;
  gl_Position = viewProj * position;
}

#stage fragment
layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;
uniform sampler2D tex;
uniform sampler2D mat_tex;
in vec2 ftexcoords;
in vec3 fnormal;
in vec3 fposition;

void main() {
  vec4 color = texture(tex,ftexcoords);
  if(alpha(color.a)) discard;
  vec4 material = texture(mat_tex,ftexcoords);
  ocolor.rgb = linearize(color.rgb);
  ocolor.a = material.y; /* Metalness */
  onormal.xy = encodeNormal(derive_normal(fposition,fnormal,material.x));
  onormal.z = material.z; /* Roughness */
  onormal.w = material.w; /* Emission */
}
