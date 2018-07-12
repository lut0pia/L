layout(binding = 1) uniform Light { 
  vec3 l_dir;
  vec3 l_color;
  float l_int;
  float l_rad;
  float l_in_ang;
  float l_out_ang;
  int l_type;
};

vec4 sphere() {
  const vec3 vertices[] = vec3[](
    vec3(0,0,1), vec3(0,1,1), vec3(1,0,1),
    vec3(0,1,0), vec3(1,1,0), vec3(0,1,1),
    vec3(1,0,0), vec3(1,0,1), vec3(1,1,0),
    vec3(0,1,1), vec3(1,1,0), vec3(1,0,1));

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
  return viewProj * model * vec4(vertex*l_rad*1.2f, 1.f);
}
void main() {
  const vec2 vertices[] = vec2[](vec2(-1.f,-1.f),vec2(-1.f,3.f),vec2(3.f,-1.f));
  switch(l_type) {
    case 0: case 2: gl_Position = vec4(vertices[gl_VertexIndex],0.f,1.f); break;
    case 1: gl_Position = sphere(); break;
  }
}
