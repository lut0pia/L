#version 330 core
#stage vertex
uniform mat4 model;
out vec3 fnormal;

const vec3 vertices[] = vec3[](
  // Bottom face
  vec3(-1,-1,-1), vec3(-1,1,-1), vec3(1,-1,-1),
  vec3(-1,1,-1), vec3(1,1,-1), vec3(1,-1,-1),
  // Top face
  vec3(-1,-1,1), vec3(1,-1,1), vec3(-1,1,1),
  vec3(-1,1,1), vec3(1,-1,1), vec3(1,1,1),
  // Back face
  vec3(-1,-1,-1), vec3(1,-1,-1), vec3(-1,-1,1),
  vec3(1,-1,-1),  vec3(1,-1,1),vec3(-1,-1,1),
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

void main() {
  fnormal = mat3(model) * normals[gl_VertexID/6];
  gl_Position = viewProj * model * vec4(vertices[gl_VertexID], 1.f);
}

#stage fragment
layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 onormal;
uniform vec4 color;
in vec3 fnormal;

void main() {
  if(alpha(color.a))
    discard;
  ocolor.rgb = linearize(color.rgb);
  ocolor.a = 0.f; /* Metalness */
  onormal.xy = encodeNormal(fnormal);
  onormal.z = 0.8f; /* Roughness */
  onormal.w = 0.f; /* Emission */
}
