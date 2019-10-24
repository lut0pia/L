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
const vec3 normals[] = vec3[](
  vec3(0,0,-1),
  vec3(0,0,1),
  vec3(0,-1,0),
  vec3(0,1,0),
  vec3(-1,0,0),
  vec3(1,0,0));

layout(location = 0) out vec3 fnormal;

void main() {
  fnormal = mat3(model) * normals[gl_VertexIndex/6];
  gl_Position = viewProj * model * vec4(vertices[gl_VertexIndex], 1.f);
}
