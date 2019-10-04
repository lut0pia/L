layout(location = 0) in vec3 vposition;
layout(location = 1) in vec3 vnormal;
layout(location = 2) in vec2 vtexcoords;

layout(location = 0) out vec3 fposition;
layout(location = 1) out vec2 ftexcoords;
layout(location = 2) out vec3 fnormal;

void main() {
  ftexcoords = vtexcoords;
  fnormal = normalize(mat3(model) * vnormal);
  vec4 position = model * vec4(vposition,1.0);
  fposition = position.xyz;
  gl_Position = viewProj * position;
}
