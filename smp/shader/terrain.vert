layout(location = 0) out vec3 ftexcoords;
layout(location = 1) out vec3 fnormal;

layout(binding = 2) uniform sampler2D height_tex;

layout(binding = 3) uniform Parameters {
  float size;
};

const ivec2[] vert = ivec2[](
  ivec2(0,0), ivec2(1,0), ivec2(0,1),
  ivec2(1,0), ivec2(1,1), ivec2(0,1)
);

float height_at(ivec2 icoords) {
  vec2 fcoords = vec2(icoords/size);
  return texture(height_tex, fcoords).r;
}

void main() {
  int size_int = int(size);
  int quad_id = gl_VertexIndex/6;
  int vert_id = gl_VertexIndex%6;

  ivec2 icoords = ivec2(quad_id%size_int, quad_id/size_int)+vert[vert_id];

  ivec2 offsetx = ivec2(1,0);
  ivec2 offsety = ivec2(0,1);
  float height = height_at(icoords);
  float heightx = height_at(icoords+offsetx);
  float heighty = height_at(icoords+offsety);

  vec3 p1 = vec3(offsetx, heightx - height);
  vec3 p2 = vec3(offsety, heighty - height);
  p1.z *= 0.1;
  p2.z *= 0.1;
  vec3 normal = normalize(cross(p1, p2));

  vec2 fcoords = vec2(icoords/size);
  ftexcoords.xy = fcoords;
  ftexcoords.z = height;
  fnormal = normalize(mat3(model) * normal);
  vec4 position = model * vec4(fcoords*2.f-1.f,height,1.0);
  gl_Position = viewProj * position;
}
