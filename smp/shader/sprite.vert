layout(location = 0) out vec2 ftexcoords;
layout(location = 1) out vec3 fnormal;

layout(binding = 1) uniform Parameters {
  vec4 uv;
};

const int[] indices = int[](1,0,2,1,2,3);
const vec4 vert = vec4(-1,1,-1,1);
void main() {
  int index = indices[gl_VertexIndex];
  ftexcoords = vec2(uv[index/2],uv[1-index%2+2]);
  fnormal = mat3(model)*vec3(0,-1,0);
  gl_Position = viewProj * model * vec4(vert[index/2],0.f,vert[index%2+2],1.f);
}
