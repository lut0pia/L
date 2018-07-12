layout(location = 0) out vec3 fnormal;

const vec3 vertices[] = vec3[](
  vec3(0,0,1), vec3(1,0,1), vec3(0,1,1),
  vec3(0,1,0), vec3(0,1,1), vec3(1,1,0),
  vec3(1,0,0), vec3(1,1,0), vec3(1,0,1),
  vec3(0,1,1), vec3(1,0,1), vec3(1,1,0));

void main() {
  int vertex_id = gl_VertexIndex%vertices.length();
  int quadrant = gl_VertexIndex/12;
  vec3 mult = vec3(1,1,1);
  if((quadrant&0x1)!=0){
      mult.x = -1;
      vertex_id = vertices.length()-vertex_id-1;
  }
  if((quadrant&0x2)!=0){
      mult.y = -1;
      vertex_id = vertices.length()-vertex_id-1;
  }
  if((quadrant&0x4)!=0){
      mult.z = -1;
      vertex_id = vertices.length()-vertex_id-1;
  }
  vec3 vertex = normalize(vertices[vertex_id]*mult);
  fnormal = mat3(model) * vertex;
  gl_Position = viewProj * model * vec4(vertex, 1.f);
}
