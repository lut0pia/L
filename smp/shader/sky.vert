layout(location = 0) in vec3 vnormal;

layout(location = 0) out vec3 fnormal;

void main() {
  fnormal = vnormal;
  gl_Position = viewProj * vec4(vnormal * 512.f + eye.xyz, 1.f);
}
