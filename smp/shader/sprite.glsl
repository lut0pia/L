#version 330 core
#stage vertex
uniform vec4 uv;
uniform mat4 model;
out vec2 ftexcoords;
out vec3 fnormal;
const int[] indices = int[](1,0,2,1,2,3);
const vec4 vert = vec4(-1,1,-1,1);
void main() {
  int index = indices[gl_VertexID];
  ftexcoords = vec2(uv[index/2],uv[1-index%2+2]);
  fnormal = mat3(model)*vec3(0,-1,0);
  gl_Position = viewProj * model * vec4(vert[index/2],0.f,vert[index%2+2],1.f);
}

#stage fragment
layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;
uniform sampler2D tex;
in vec2 ftexcoords;
in vec3 fnormal;
void main() {
  vec4 color = texture(tex,ftexcoords);
  if(alpha(color.a)) discard;
  ocolor.rgb = linearize(color.rgb);
  ocolor.a = 0.f; // Metalness
  onormal.xy = encodeNormal(fnormal);
  onormal.z = 1.f; // Roughness
  onormal.w = 0.f; // Emission
}
