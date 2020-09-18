layout(location = 0) in vec3 vposition;
layout(location = 1) in vec3 vnormal;

layout(location = 0) out vec3 fnormal;

void main() {
  fnormal = normalize(mat3(model) * vnormal);
  gl_Position = viewProj * model * vec4(vposition, 1.f);
}
