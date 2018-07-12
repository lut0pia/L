const vec2 vertices[] = vec2[](vec2(-1.f,-1.f),vec2(-1.f,3.f),vec2(3.f,-1.f));

void main() {
  gl_Position = vec4(vertices[gl_VertexIndex], 0.f, 1.f);
}
