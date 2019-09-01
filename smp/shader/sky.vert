const vec3 vertices[] = vec3[](
  // Bottom face
  vec3(-1,-1,-1), vec3(-1,1,-1), vec3(1,-1,-1),
  vec3(-1,1,-1), vec3(1,1,-1), vec3(1,-1,-1),
  // Top face
  vec3(-1,-1,1), vec3(1,-1,1), vec3(-1,1,1),
  vec3(-1,1,1), vec3(1,-1,1), vec3(1,1,1),
  // Back face
  vec3(-1,-1,-1), vec3(1,-1,-1), vec3(-1,-1,1),
  vec3(1,-1,-1), vec3(1,-1,1), vec3(-1,-1,1),
  // Front face
  vec3(-1,1,-1), vec3(-1,1,1), vec3(1,1,-1),
  vec3(1,1,-1), vec3(-1,1,1), vec3(1,1,1),
  // Left face
  vec3(-1,-1,-1), vec3(-1,-1,1), vec3(-1,1,-1),
  vec3(-1,-1,1), vec3(-1,1,1), vec3(-1,1,-1),
  // Right face
  vec3(1,-1,-1), vec3(1,1,-1), vec3(1,-1,1),
  vec3(1,-1,1), vec3(1,1,-1), vec3(1,1,1));

layout(location = 0) out vec3 fposition;

void main() {
  fposition = vertices[gl_VertexIndex] * 512.f + eye.xyz;
  gl_Position = viewProj * vec4(fposition, 1.f);
}
