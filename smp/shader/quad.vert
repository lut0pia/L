layout(location = 0) out vec2 ftexcoords;

const vec2 vertices[] = vec2[](
  vec2(-1.f,-1.f),
  vec2(-1.f,1.f),
  vec2(1.f,1.f),
  vec2(1.f,-1.f),
  vec2(-1.f,-1.f),
  vec2(1.f,1.f)
);

void main() {
  ftexcoords = clamp(vertices[gl_VertexIndex], 0.f, 1.f);
  gl_Position = model * vec4(vertices[gl_VertexIndex], 0.f, 1.f);
}
